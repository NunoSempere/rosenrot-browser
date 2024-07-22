#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "plugins/plugins.h"
#include <webkit/webkit.h>

/* Global variables */
static GtkNotebook* notebook;
static GtkWindow* window;
typedef enum { _SEARCH, _FIND, _HIDDEN } Bar_entry_mode;
static struct {
    GtkHeaderBar* widget;
    GtkEntry* line;
    GtkEntryBuffer* line_text;
    Bar_entry_mode entry_mode;
} bar;
static int num_tabs = 0;
static int custom_style_enabled = 1;

/* Forward declarations */
void toggle_bar(GtkNotebook* notebook, Bar_entry_mode mode);
void notebook_create_new_tab(GtkNotebook* notebook, const char* uri);
static int handle_signal_keypress(void* self, int keyval, int keycode,
    GdkModifierType state, void* controller);

/* Utils */
WebKitWebView* notebook_get_webview(GtkNotebook* notebook) /* TODO: Think through whether to pass global variables or not */ 
{
    WebKitWebView* view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
    NULLCHECK(view);
    return view;
}

/* Load content */
void load_uri(WebKitWebView* view, const char* uri)
{
    bool is_empty_uri = (strlen(uri) == 0);
    bool has_direct_uri_prefix = (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") || g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:"));
    bool has_common_domain_extension = (strstr(uri, ".com") || strstr(uri, ".org"));
    bool has_shortcut;

    int l = SHORTCUT_N + strlen(uri) + 1;
    char uri_expanded[l];
    str_init(uri_expanded, l);
    int check = shortcut_expand(uri, uri_expanded);
    has_shortcut = (check == 2);

    if (is_empty_uri) {
        webkit_web_view_load_uri(view, "");
        toggle_bar(notebook, _SEARCH);
    } else if (has_direct_uri_prefix){
        webkit_web_view_load_uri(view, uri);
    } else if (has_common_domain_extension){
        char tmp[strlen("https://") + strlen(uri) + 1];
        snprintf(tmp, sizeof(tmp) + 1, "https://%s", uri);
        webkit_web_view_load_uri(view, tmp);
    } else if (has_shortcut){
        webkit_web_view_load_uri(view, uri_expanded);
    } else {
        char tmp[strlen(uri) + strlen(SEARCH)];
        snprintf(tmp, sizeof(tmp), SEARCH, uri);
        webkit_web_view_load_uri(view, tmp);
    }
}

/* Deal with new load or changed load */
void redirect_if_annoying(WebKitWebView* view, const char* uri)
{
    if (LIBRE_REDIRECT_ENABLED) {
        int l = LIBRE_N + strlen(uri) + 1;
        char uri_filtered[l];
        str_init(uri_filtered, l);

        int check = libre_redirect(uri, uri_filtered);
        if (check == 2) webkit_web_view_load_uri(view, uri_filtered);
    }
}
void set_custom_style(WebKitWebView* view)
{
    if (custom_style_enabled) {
        char* style_js = malloc(STYLE_N + 1);
        read_style_js(style_js);
        webkit_web_view_evaluate_javascript(view, style_js, -1, NULL, "rosenrot-style-plugin", NULL, NULL, NULL);
        free(style_js);
    }
}

void handle_signal_load_changed(WebKitWebView* self, WebKitLoadEvent load_event,
    GtkNotebook* notebook)
{
    switch (load_event) {
        // https://webkitgtk.org/reference/webkit2gtk/2.5.1/WebKitWebView.html
        case WEBKIT_LOAD_STARTED:
        case WEBKIT_LOAD_COMMITTED:
            set_custom_style(self);
        case WEBKIT_LOAD_REDIRECTED:
            redirect_if_annoying(self, webkit_web_view_get_uri(self));
            break;
        case WEBKIT_LOAD_FINISHED: {
            set_custom_style(self);
            /* Add gtk tab title */
            const char* webpage_title = webkit_web_view_get_title(self);
            const int max_length = 25;
            char tab_title[max_length + 1];
            if (webpage_title != NULL) {
                for (int i = 0; i < (max_length); i++) {
                    tab_title[i] = webpage_title[i];
                    if (webpage_title[i] == '\0') {
                        break;
                    }
                }
                tab_title[max_length] = '\0';
            }
            gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(self),
                webpage_title == NULL ? "—" : tab_title);
        }
    }
}

/* New tabs */
WebKitWebView* create_new_webview()
{
    WebKitSettings* settings = webkit_settings_new_with_settings(WEBKIT_DEFAULT_SETTINGS, NULL);
    if (CUSTOM_USER_AGENT) {
        webkit_settings_set_user_agent(
            settings,
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
            "like Gecko) Chrome/120.0.0.0 Safari/537.3");
            // https://www.useragents.me
    }
    WebKitNetworkSession* network_session = webkit_network_session_new(DATA_DIR, DATA_DIR);
    WebKitUserContentManager* contentmanager = webkit_user_content_manager_new();
    WebKitCookieManager* cookiemanager = webkit_network_session_get_cookie_manager(network_session);
    webkit_cookie_manager_set_persistent_storage(cookiemanager, DATA_DIR "/cookies.sqlite", WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    WebKitWebView* view = g_object_new(WEBKIT_TYPE_WEB_VIEW, "settings", settings, "network-session", network_session, "user-content-manager", contentmanager, NULL);
    NULLCHECK(view);

    GtkEventController* event_controller = gtk_event_controller_key_new();
    g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress), NULL);
    gtk_widget_add_controller(GTK_WIDGET(view), event_controller);

    return view;
}

GtkWidget* handle_signal_create_new_tab(WebKitWebView* self,
    WebKitNavigationAction* navigation_action,
    GtkNotebook* notebook)
{
    NULLCHECK(self);
    NULLCHECK(notebook);
    if (num_tabs < MAX_NUM_TABS || num_tabs == 0) {
        WebKitURIRequest* uri_request = webkit_navigation_action_get_request(navigation_action);
        const char* uri = webkit_uri_request_get_uri(uri_request);
        webkit_web_view_stop_loading(self);
        printf("Creating new window: %s\n", uri);
        notebook_create_new_tab(notebook, uri);
        gtk_notebook_set_show_tabs(notebook, true);
    } else {
        webkit_web_view_evaluate_javascript(self, "alert('Too many tabs, not opening a new one')", -1, NULL, "rosenrot-alert-numtabs", NULL, NULL, NULL);
    }
    return ABORT_REQUEST_ON_CURRENT_TAB;
    // Could also return GTK_WIDGET(self), in which case the new uri would also be loaded in the current webview. This could be interesting if I wanted to e.g., open an alternative frontend in a new tab
}

void notebook_create_new_tab(GtkNotebook* notebook, const char* uri)
{
    if (num_tabs < MAX_NUM_TABS || MAX_NUM_TABS == 0) {
        WebKitWebView* view = create_new_webview();
        NULLCHECK(view);

        g_signal_connect(view, "load_changed", G_CALLBACK(handle_signal_load_changed), notebook);
        g_signal_connect(view, "create", G_CALLBACK(handle_signal_create_new_tab), notebook);

        int n = gtk_notebook_append_page(notebook, GTK_WIDGET(view), NULL);
        gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(view), true);
        NULLCHECK(window);
        NULLCHECK(bar.widget);
        gtk_widget_set_visible(GTK_WIDGET(window), 1);
        gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);
        load_uri(view, (uri) ? uri : HOME);

        set_custom_style(view);

        gtk_notebook_set_current_page(notebook, n);
        gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(view), "-");
        webkit_web_view_set_zoom_level(view, ZOOM_START_LEVEL);
        num_tabs += 1;
    } else {
        webkit_web_view_evaluate_javascript(notebook_get_webview(notebook), "alert('Too many tabs, not opening a new one')",
            -1, NULL, "rosenrot-alert-numtabs", NULL, NULL, NULL);
    }
}

/* Top bar */
void toggle_bar(GtkNotebook* notebook, Bar_entry_mode mode)
{
    bar.entry_mode = mode;
    switch (bar.entry_mode) {
        case _SEARCH: {
            const char* url = webkit_web_view_get_uri(notebook_get_webview(notebook));
            gtk_entry_set_placeholder_text(bar.line, "Search");
            gtk_entry_buffer_set_text(bar.line_text, url, strlen(url));
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 1);
            gtk_window_set_focus(window, GTK_WIDGET(bar.line));
            break;
        }
        case _FIND: {
            const char* search_text = webkit_find_controller_get_search_text(
                webkit_web_view_get_find_controller(notebook_get_webview(notebook)));

            if (search_text != NULL)
                gtk_entry_buffer_set_text(bar.line_text, search_text, strlen(search_text));

            gtk_entry_set_placeholder_text(bar.line, "Find");
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 1);
            gtk_window_set_focus(window, GTK_WIDGET(bar.line));
            break;
        }
        case _HIDDEN:
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);
    }
}

// Handle what happens when the user is on the bar and presses enter
void handle_signal_bar_press_enter(GtkEntry* self, GtkNotebook* notebook) /* consider passing notebook as the data here? */
{
    WebKitWebView* view = notebook_get_webview(notebook);
    if (bar.entry_mode == _SEARCH)
        load_uri(view, gtk_entry_buffer_get_text(bar.line_text));
    else if (bar.entry_mode == _FIND)
        webkit_find_controller_search(
            webkit_web_view_get_find_controller(view),
            gtk_entry_buffer_get_text(bar.line_text),
            WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE | WEBKIT_FIND_OPTIONS_WRAP_AROUND,
            G_MAXUINT);

    gtk_widget_hide(GTK_WIDGET(bar.widget));
}

/* Handle shortcuts */

int handle_shortcut(func id)
{
    static double zoom = ZOOM_START_LEVEL;
    static bool is_fullscreen = 0;

    WebKitWebView* view = notebook_get_webview(notebook);
    NULLCHECK(notebook);
    NULLCHECK(view);

    switch (id) {
        case goback:
            webkit_web_view_go_back(view);
            break;
        case goforward:
            webkit_web_view_go_forward(view);
            break;

        case toggle_custom_style: /* Ctrl s + Ctrl Shift R to reload */
            if (custom_style_enabled)
                custom_style_enabled = 0;
            else
                custom_style_enabled = 1;
            // break; passthrough
        case refresh:
            webkit_web_view_reload(view);
            break;
        case refresh_force:
            webkit_web_view_reload_bypass_cache(view);
            break;

        case back_to_home:
            load_uri(view, HOME);
            break;

        case zoomin:
            webkit_web_view_set_zoom_level(view,
                (zoom += ZOOM_STEPSIZE));
            break;
        case zoomout:
            webkit_web_view_set_zoom_level(view,
                (zoom -= ZOOM_STEPSIZE));
            break;
        case zoom_reset:
            webkit_web_view_set_zoom_level(view,
                (zoom = ZOOM_START_LEVEL));
            break;

        case prev_tab:; // declarations aren't statements
            // https://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
            int n = gtk_notebook_get_n_pages(notebook);
            int k = gtk_notebook_get_current_page(notebook);
            int l = (n + k - 1) % n;
            gtk_notebook_set_current_page(notebook, l);
            break;
        case next_tab:;
            int m = gtk_notebook_get_n_pages(notebook);
            int i = gtk_notebook_get_current_page(notebook);
            int j = (i + 1) % m;
            gtk_notebook_set_current_page(notebook, j);
            break;
        case close_tab:
            gtk_notebook_remove_page(notebook, gtk_notebook_get_current_page(notebook));
            num_tabs -= 1;

            switch (gtk_notebook_get_n_pages(notebook)) {
                case 0:
                    exit(0);
                    break;
                case 1:
                    gtk_notebook_set_show_tabs(notebook, false);
                    break;
            }

            break;
        case toggle_fullscreen:
            if (is_fullscreen)
                gtk_window_unfullscreen(window);
            else
                gtk_window_fullscreen(window);
            is_fullscreen = !is_fullscreen;
            break;
        case show_searchbar:
            toggle_bar(notebook, _SEARCH);
            break;
        case show_finder:
            toggle_bar(notebook, _FIND);
            break;

        case finder_next:
            webkit_find_controller_search_next(webkit_web_view_get_find_controller(view));
            break;
        case finder_prev:
            webkit_find_controller_search_previous(webkit_web_view_get_find_controller(view));
            break;

        case new_tab:
            notebook_create_new_tab(notebook, NULL);
            gtk_notebook_set_show_tabs(notebook, true);
            toggle_bar(notebook, _SEARCH);
            break;

        case hide_bar:
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);
            toggle_bar(notebook, _HIDDEN);
            break;

        case halve_window:
            gtk_window_set_default_size(window, FULL_WIDTH/2, HEIGHT_GTK4);
            break;

        case rebig_window:
            gtk_window_set_default_size(window, FULL_WIDTH, HEIGHT_GTK4);
            break;

        case prettify: {
            if (READABILITY_ENABLED) {
                char* readability_js = malloc(READABILITY_N + 1);
                read_readability_js(readability_js);
                webkit_web_view_evaluate_javascript(view, readability_js, -1, NULL, "rosenrot-readability-plugin", NULL, NULL, NULL);
                free(readability_js);
            }
            break;
        }
    }

    return 1;
}

/* Listen to keypresses */

static int handle_signal_keypress(void* self, int keyval, int keycode,
    GdkModifierType state, void* controller)
{

    if (0) {
        printf("New keypress\n");
        printf("Keypress state: %d\n", state);
        printf("Keypress value: %d\n", keyval);
    }
    for (int i = 0; i < sizeof(shortcut) / sizeof(shortcut[0]); i++) {
        if ((state & shortcut[i].mod || shortcut[i].mod == 0x0) && keyval == shortcut[i].key) {
            printf("New shortcut, with id: %d\n", shortcut[i].id);
            return handle_shortcut(shortcut[i].id);
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    /* Initialize GTK in general */
    gtk_init();
    g_object_set(gtk_settings_get_default(), GTK_SETTINGS_CONFIG_H, NULL); // https://docs.gtk.org/gobject/method.Object.set.html
    GtkCssProvider* css = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css, "/opt/rosenrot/style-gtk4.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Create the main window
    window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_default_size(window, WIDTH, HEIGHT_GTK4);

    // Set up notebook
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_show_tabs(notebook, false);
    gtk_notebook_set_show_border(notebook, false);
    gtk_window_set_child(window, GTK_WIDGET(notebook));

    // Bar
    bar.line_text = GTK_ENTRY_BUFFER(gtk_entry_buffer_new("", 0));
    bar.line = GTK_ENTRY(gtk_entry_new_with_buffer(bar.line_text));
    gtk_entry_set_alignment(bar.line, 0.48);
    gtk_widget_set_size_request(GTK_WIDGET(bar.line), BAR_WIDTH, -1);

    bar.widget = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_title_widget(bar.widget, GTK_WIDGET(bar.line));
    gtk_window_set_titlebar(window, GTK_WIDGET(bar.widget));

    // Signals
    GtkEventController* event_controller = gtk_event_controller_key_new();
    g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress), NULL);
    gtk_widget_add_controller(GTK_WIDGET(window), event_controller);

    g_signal_connect(bar.line, "activate", G_CALLBACK(handle_signal_bar_press_enter), notebook); 
    g_signal_connect(GTK_WIDGET(window), "destroy", G_CALLBACK(exit), notebook);

    // Show the application window
    gtk_window_present(window);

    char* first_uri = argc > 1 ? argv[1] : HOME;
    notebook_create_new_tab(notebook, first_uri);

    /* Show to user */
    gtk_widget_set_visible(GTK_WIDGET(window), 1);
    if (argc != 0) gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);

    /* Deal with more tabs */
    if (argc > 2) {
        gtk_notebook_set_show_tabs(notebook, true);
        for (int i = 2; i < argc; i++) {
            notebook_create_new_tab(notebook, argv[i]);
        }
    }

    // Enter the main event loop, and wait for user interaction
    printf("Entering main loop\n");
    while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0 && num_tabs > 0)
        g_main_context_iteration(NULL, TRUE);

    return 0;
}
