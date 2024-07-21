#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "plugins/plugins.h"
#include <webkit/webkit.h>

static GtkNotebook* notebook;
static GtkWidget* window;
typedef enum { _SEARCH,
    _FIND,
    _HIDDEN } Bar_entry_mode;
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

/* Utils */
WebKitWebView* notebook_get_webview(GtkNotebook* notebook)
{
    return WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
}

/* Load content */
void load_uri(WebKitWebView* view, const char* uri)
{
    if (strlen(uri) == 0) {
        webkit_web_view_load_uri(view, "");
        toggle_bar(notebook, _SEARCH);
    } else if (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") || g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:")) {
        webkit_web_view_load_uri(view, uri);
    } else if (strstr(uri, ".com") || strstr(uri, ".org")) {
        char tmp[strlen("https://") + strlen(uri) + 1];
        snprintf(tmp, sizeof(tmp) + 1, "https://%s", uri);
        webkit_web_view_load_uri(view, tmp);
    } else {
        // Check for shortcuts
        int l = SHORTCUT_N + strlen(uri) + 1;
        char uri_expanded[l];
        str_init(uri_expanded, l);
        int check = shortcut_expand(uri, uri_expanded);
        if (check == 2) {
            webkit_web_view_load_uri(view, uri_expanded);
        } else {
            // Feed into search engine.
            char tmp[strlen(uri) + strlen(SEARCH)];
            snprintf(tmp, sizeof(tmp), SEARCH, uri);
            webkit_web_view_load_uri(view, tmp);
        }
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
            gtk_widget_show(GTK_WIDGET(bar.widget));
            gtk_window_set_focus(window, GTK_WIDGET(bar.line));
            break;
        }
        case _FIND: {
            const char* search_text = webkit_find_controller_get_search_text(
                webkit_web_view_get_find_controller(notebook_get_webview(notebook)));

            if (search_text != NULL)
                gtk_entry_buffer_set_text(bar.line_text, search_text, strlen(search_text));

            gtk_entry_set_placeholder_text(bar.line, "Find");
            gtk_widget_show(GTK_WIDGET(bar.widget));
            gtk_window_set_focus(window, GTK_WIDGET(bar.line));
            break;
        }
        case _HIDDEN:
            gtk_widget_hide(GTK_WIDGET(bar.widget));
    }
}

// Handle what happens when the user is on the bar and presses enter
void handle_signal_bar_press_enter(GtkEntry* self, GtkNotebook* notebook)
{
    if (bar.entry_mode == _SEARCH)
        load_uri(notebook_get_webview(notebook), gtk_entry_buffer_get_text(bar.line_text));
    else if (bar.entry_mode == _FIND)
        webkit_find_controller_search(
            webkit_web_view_get_find_controller(notebook_get_webview(notebook)),
            gtk_entry_buffer_get_text(bar.line_text),
            WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE | WEBKIT_FIND_OPTIONS_WRAP_AROUND,
            G_MAXUINT);

    gtk_widget_hide(GTK_WIDGET(bar.widget));
}



/* Create new tabs */
WebKitWebView* create_new_webview()
{
    char* style;
    WebKitSettings* settings;
    WebKitCookieManager* cookiemanager;
    WebKitNetworkSession* network_session;
    WebKitUserContentManager* contentmanager;

    settings = webkit_settings_new_with_settings(WEBKIT_DEFAULT_SETTINGS, NULL);
    if (CUSTOM_USER_AGENT) {
        webkit_settings_set_user_agent(
            settings,
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
            "like Gecko) Chrome/120.0.0.0 Safari/537.3");
        // https://www.useragents.me
    }
    network_session = webkit_network_session_new(DATA_DIR, DATA_DIR);
    contentmanager = webkit_user_content_manager_new();
    cookiemanager = webkit_network_session_get_cookie_manager(network_session);

    webkit_cookie_manager_set_persistent_storage(cookiemanager, DATA_DIR "/cookies.sqlite", WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);

    webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    if (g_file_get_contents("~/opt/rosenrot/style.css", &style, NULL, NULL)) {
        webkit_user_content_manager_add_style_sheet(
            contentmanager, webkit_user_style_sheet_new(style, WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES, WEBKIT_USER_STYLE_LEVEL_USER, NULL, NULL));
    }

    return g_object_new(WEBKIT_TYPE_WEB_VIEW, "settings", settings, "network-session", network_session, "user-content-manager", contentmanager, NULL);
}

GtkWidget* handle_signal_create_new_tab(WebKitWebView* self,
    WebKitNavigationAction* navigation_action,
    GtkNotebook* notebook)
{
    if (num_tabs < MAX_NUM_TABS || num_tabs == 0) {
        WebKitURIRequest* uri_request = webkit_navigation_action_get_request(navigation_action);
        const char* uri = webkit_uri_request_get_uri(uri_request);
        printf("Creating new window: %s\n", uri);
        notebook_create_new_tab(notebook, uri);
        gtk_notebook_set_show_tabs(notebook, true);
    } else {
        webkit_web_view_evaluate_javascript(self, "alert('Too many tabs, not opening a new one')", -1, NULL, "rosenrot-alert-numtabs", NULL, NULL, NULL);
    }
    return NULL;
    /*
     WebKitGTK documentation recommends returning the new webview.
     I imagine that this might allow e.g., to go back in a new tab
     or generally to keep track of history.
     However, this would require either modifying notebook_create_new_tab
     or duplicating its contents, for unclear gain.
   */
}

void notebook_create_new_tab(GtkNotebook* notebook, const char* uri)
{
    if (num_tabs < MAX_NUM_TABS || MAX_NUM_TABS == 0) {
        WebKitWebView* view = create_new_webview();

        // g_signal_connect(view, "load_changed", G_CALLBACK(handle_signal_load_changed), notebook);
        // g_signal_connect(view, "create", G_CALLBACK(handle_signal_create_new_tab), notebook);

        int n = gtk_notebook_append_page(notebook, GTK_WIDGET(view), NULL);
        gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(view), true);
        gtk_widget_set_visible(GTK_WIDGET(window), 1);
        // gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);
        load_uri(view, (uri) ? uri : HOME);

        // set_custom_style(view);

        gtk_notebook_set_current_page(notebook, n);
        gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(view), "-");
        num_tabs += 1;
    } else {
    }
}

/* Listen to keypresses */

static int handle_signal_keypress(GtkEventControllerKey* event_controller, int keyval, int keycode,
    GdkModifierType state, GtkNotebook* notebook)
{

    printf("New keypress!\n");
    /*
    if (1) {
        printf("Keypress state: %d\n", state);
        printf("Keypress value: %d\n", keyval);
    }

    for (int i = 0; i < sizeof(shortcut) / sizeof(shortcut[0]); i++){
        if ((state & shortcut[i].mod || shortcut[i].mod == 0x0) && keyval == shortcut[i].key) {
            return handle_shortcut(shortcut[i].id, notebook);
        }
    }

*/
    return 0;
}

static gboolean
event_key_pressed_cb(GtkWidget* drawing_area,
    guint keyval,
    guint keycode,
    GdkModifierType state,
    GtkEventControllerKey* event_controller)
{

    fprintf(stdout, "New keypress!\n");
    if (1) {
        printf("Keypress state: %d\n", state);
        printf("Keypress value: %d\n", keyval);
    }
    for (int i = 0; i < sizeof(shortcut) / sizeof(shortcut[0]); i++){
        if ((state & shortcut[i].mod || shortcut[i].mod == 0x0) && keyval == shortcut[i].key) {
            printf("New shortcut: %d\n", shortcut[i].id);
            // return handle_shortcut(shortcut[i].id, notebook);
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Initialize i18n support with bindtextdomain(), etc.

    // ...

    // Initialize the widget set
    gtk_init();

    // Create the main window
    window = gtk_window_new();
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT_4);

    // Set up notebook
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_show_tabs(notebook, true);
    gtk_notebook_set_show_border(notebook, false);
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(notebook));

    // Bar
    bar.line_text = GTK_ENTRY_BUFFER(gtk_entry_buffer_new("", 0));
    bar.line = GTK_ENTRY(gtk_entry_new_with_buffer(bar.line_text));
    gtk_entry_set_alignment(bar.line, 0.48);
    gtk_widget_set_size_request(GTK_WIDGET(bar.line), BAR_SIZE, -1);
    g_signal_connect(bar.line, "activate", G_CALLBACK(handle_signal_bar_press_enter), notebook);

    bar.widget = GTK_HEADER_BAR(gtk_header_bar_new());
        gtk_header_bar_set_title_widget(bar.widget, GTK_WIDGET(bar.line));
    gtk_window_set_titlebar(window, GTK_WIDGET(bar.widget));

    // Listen to signals

    GtkEventController* event_controller;
    event_controller = gtk_event_controller_key_new();

    g_signal_connect_object(event_controller, "key-pressed", G_CALLBACK(event_key_pressed_cb), window, G_CONNECT_DEFAULT);
    gtk_widget_add_controller(GTK_WIDGET(window), event_controller);

    // Show the application window
    gtk_window_present(GTK_WINDOW(window));

    char* first_uri = argc > 1 ? argv[1] : HOME;
    notebook_create_new_tab(notebook, first_uri);

    fprintf(stdout, "Hello world!");

    // Enter the main event loop, and wait for user interaction
    while (!0)
        g_main_context_iteration(NULL, TRUE);

    // The user lost interest
    return 0;
}
