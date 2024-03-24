#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>
#include <webkit2/webkit2.h>

// User config
#define WIDTH 1920 // 960 for half-width, 1920 for full width
#define HEIGHT 1080
#define BAR_SIZE 1000
#define SEARCH "https://lite.duckduckgo.com/html/?q=%s"
#define HOME "https://lite.duckduckgo.com/html"

// Minimal niceties
#define ZOOM 1 /* Starting zoom level.*/
#define ZOOM_VAL .1 /* Zooming value in zoomin/zoomout functions */
#define MAX_NUM_TABS 8

// Webkit settings
// See: https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html
#define WEBKIT_DEFAULT_SETTINGS                                                 \
    "enable-back-forward-navigation-gestures", 1, "enable-developer-extras", 1, \
        "enable-smooth-scrolling", 0,                                           \
        "default-charset", "utf-8"

/* CACHE */
#define DATA_DIR "/home/nuno/.cache/rosenrot"
#define DATA_MANAGER_OPTS "base-cache-directory", DATA_DIR, "base-data-directory", DATA_DIR

// GTK
#define GTK_SETTINGS_CONFIG_H "gtk-application-prefer-dark-theme", 0, "gtk-enable-animations", 0
#define KEY(x) GDK_KEY_##x
#define SFT 1 << 0
#define CTRL 1 << 2
#define ALT 1 << 3

/* Global declarations */
static GtkNotebook* notebook;
static GtkWindow* window;
static struct {
    GtkHeaderBar* widget;
    GtkEntry* line;
    GtkEntryBuffer* line_text;
    enum { _SEARCH,
        _FIND,
        _HIDDEN } entry_mode;
} bar;
static int num_tabs = 0;

// Forward declarations
void notebook_create_new_tab(GtkNotebook* notebook, const char* uri);

/* Utils */
WebKitWebView* notebook_get_webview(GtkNotebook* notebook)
{
    return WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
}

/* Load content*/

void load_uri(WebKitWebView* view, const char* uri)
{
    if (strlen(uri) == 0) {
        webkit_web_view_load_uri(view, "");
    } else if (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") || g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:")) {
        webkit_web_view_load_uri(view, uri);
    } else {
        char tmp[strlen(uri) + strlen(SEARCH)];
        snprintf(tmp, sizeof(tmp), SEARCH, uri);
        webkit_web_view_load_uri(view, tmp);
    }
}

void handle_signal_load_changed(WebKitWebView* self, WebKitLoadEvent load_event,
    GtkNotebook* notebook)
{
    switch (load_event) {
        case WEBKIT_LOAD_FINISHED: {
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

/* Create new tabs */
GtkWidget* handle_signal_create_new_tab(WebKitWebView* self,
    WebKitNavigationAction* navigation_action,
    GtkNotebook* notebook)
{
    if (num_tabs < MAX_NUM_TABS || num_tabs == 0) {
        WebKitURIRequest* uri_request = webkit_navigation_action_get_request(navigation_action);
        const char* uri = webkit_uri_request_get_uri(uri_request);
        notebook_create_new_tab(notebook, uri);
        gtk_notebook_set_show_tabs(notebook, 1);
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
WebKitWebView* create_new_webview()
{
    char* style;
    WebKitSettings* settings;
    WebKitWebContext* web_context;
    WebKitCookieManager* cookiemanager;
    WebKitUserContentManager* contentmanager;

    settings = webkit_settings_new_with_settings(WEBKIT_DEFAULT_SETTINGS, NULL);
    web_context = webkit_web_context_new_with_website_data_manager(webkit_website_data_manager_new(DATA_MANAGER_OPTS, NULL));
    contentmanager = webkit_user_content_manager_new();
    cookiemanager = webkit_web_context_get_cookie_manager(web_context);

    webkit_cookie_manager_set_persistent_storage(cookiemanager, DATA_DIR "/cookies.sqlite", WEBKIT_COOKIE_PERSISTENT_STORAGE_SQLITE);
    webkit_cookie_manager_set_accept_policy(cookiemanager, WEBKIT_COOKIE_POLICY_ACCEPT_ALWAYS);

    return g_object_new(WEBKIT_TYPE_WEB_VIEW, "settings", settings, "web-context", web_context, "user-content-manager", contentmanager, NULL);
}
void notebook_create_new_tab(GtkNotebook* notebook, const char* uri)
{
    if (num_tabs < MAX_NUM_TABS || MAX_NUM_TABS == 0) {
        WebKitWebView* view = create_new_webview();

        g_signal_connect(view, "load_changed", G_CALLBACK(handle_signal_load_changed), notebook);
        g_signal_connect(view, "create", G_CALLBACK(handle_signal_create_new_tab), notebook);

        int n = gtk_notebook_append_page(notebook, GTK_WIDGET(view), NULL);
        gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(view), 1);
        gtk_widget_show_all(GTK_WIDGET(window));
        load_uri(view, (uri) ? uri : HOME);

        gtk_notebook_set_current_page(notebook, n);
        gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(view), "-");
        webkit_web_view_set_zoom_level(view, ZOOM);
        num_tabs += 1;
    } else {
        webkit_web_view_evaluate_javascript(notebook_get_webview(notebook), "alert('Too many tabs, not opening a new one')",
            -1, NULL, "rosenrot-alert-numtabs", NULL, NULL, NULL);
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
}

/* Handle shortcuts */
int handle_signal_keypress(void* self, GdkEvent* event, GtkNotebook* notebook)
{
    // (void)self;

    guint event_keyval = 0;
    gdk_event_get_keyval(event, &event_keyval);
    GdkModifierType event_state = 0;
    gdk_event_get_state(event, &event_state);

    static double zoom = ZOOM;
    static bool is_fullscreen = 0;

    if (event_state & CTRL) {
        WebKitWebView* view = notebook_get_webview(notebook);
        switch (event_keyval) {
            case KEY(h): // go back
                webkit_web_view_go_back(view);
                break;
            case KEY(j): // go forward
                webkit_web_view_go_forward(view);
                break;
            case KEY(r): // reload
                webkit_web_view_reload(view);
                break;
            case KEY(R): // force reload
                webkit_web_view_reload_bypass_cache(view);
                break;
            case KEY(H): // back to home
                load_uri(view, HOME);
                break;
            case KEY(equal): // zoom in
                webkit_web_view_set_zoom_level(view, (zoom += ZOOM_VAL));
                break;
            case KEY(minus): // zoom out
                webkit_web_view_set_zoom_level(view, (zoom -= ZOOM_VAL));
                break;
            case KEY(0): // restore zoom
                webkit_web_view_set_zoom_level(view, (zoom = ZOOM));
                break;
            case KEY(KP_Page_Up): {
                int n = gtk_notebook_get_n_pages(notebook);
                int k = gtk_notebook_get_current_page(notebook);
                int l = (n + k - 1) % n;
                gtk_notebook_set_current_page(notebook, l);
                break;
            } // previous tab
            case KEY(KP_Page_Down): { // next tab
                int m = gtk_notebook_get_n_pages(notebook);
                int i = gtk_notebook_get_current_page(notebook);
                int j = (i + 1) % m;
                gtk_notebook_set_current_page(notebook, j);
                break;
            }
            case KEY(w): // close tab
                gtk_notebook_remove_page(notebook, gtk_notebook_get_current_page(notebook));
                num_tabs -= 1;
                if (gtk_notebook_get_n_pages(notebook) == 0) {
                    exit(0);
                }
                break;
            case KEY(l): // show search bar
            case KEY(semicolon): { // fallthrough; same
                bar.entry_mode = _SEARCH;
                const char* url = webkit_web_view_get_uri(notebook_get_webview(notebook));
                gtk_entry_set_placeholder_text(bar.line, "Search");
                gtk_entry_buffer_set_text(bar.line_text, url, strlen(url));
                gtk_widget_show(GTK_WIDGET(bar.widget));
                gtk_window_set_focus(window, GTK_WIDGET(bar.line));
                break;
            }
            case KEY(f): { // find
                bar.entry_mode = _FIND;
                const char* search_text = webkit_find_controller_get_search_text(webkit_web_view_get_find_controller(notebook_get_webview(notebook)));
                if (search_text != NULL) gtk_entry_buffer_set_text(bar.line_text, search_text, strlen(search_text));

                gtk_entry_set_placeholder_text(bar.line, "Find");
                gtk_window_set_focus(window, GTK_WIDGET(bar.line));
                break;
            } break;
            case KEY(n): // find next
                webkit_find_controller_search_next(webkit_web_view_get_find_controller(view));
                break;
            case KEY(N): // find previous
                webkit_find_controller_search_previous(webkit_web_view_get_find_controller(view));
                break;
            case KEY(t): // new tab
                notebook_create_new_tab(notebook, NULL);
                break;
        }
    } else if (event_state == 0x0 && event_keyval == KEY(F11)) {
        if (is_fullscreen)
            gtk_window_unfullscreen(window);
        else
            gtk_window_fullscreen(window);
        is_fullscreen = !is_fullscreen;
    }

    return 0;
}

int main(int argc, char** argv)
{
    /* Initialize GTK in general */
    gtk_init(NULL, NULL); // <https://docs.gtk.org/gtk3/func.init.html>
    g_object_set(gtk_settings_get_default(), GTK_SETTINGS_CONFIG_H, NULL); // <https://docs.gtk.org/gobject/method.Object.set.html>

    /* Initialize GTK objects. These are declared as static globals at the top of this file */
    // Notebook
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_show_tabs(notebook, 0);
    gtk_notebook_set_show_border(notebook, 0);

    // Window
    window = GTK_WINDOW(gtk_window_new(0));
    gtk_window_set_default_size(window, WIDTH, HEIGHT);
    g_signal_connect(window, "key-press-event", G_CALLBACK(handle_signal_keypress), notebook);
    g_signal_connect(window, "destroy", G_CALLBACK(exit), notebook);
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(notebook));

    // Bar
    bar.line_text = GTK_ENTRY_BUFFER(gtk_entry_buffer_new("", 0));
    bar.line = GTK_ENTRY(gtk_entry_new_with_buffer(bar.line_text));
    gtk_entry_set_alignment(bar.line, 0.48);
    gtk_widget_set_size_request(GTK_WIDGET(bar.line), BAR_SIZE, -1);
    g_signal_connect(bar.line, "activate", G_CALLBACK(handle_signal_bar_press_enter), notebook);

    bar.widget = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_custom_title(bar.widget, GTK_WIDGET(bar.line));
    gtk_window_set_titlebar(window, GTK_WIDGET(bar.widget));

    /* Load first tab */
    char* first_uri = argc > 1 ? argv[1] : HOME;
    notebook_create_new_tab(notebook, first_uri);

    /* Show to user */
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_notebook_set_show_tabs(notebook, 1);

    /* Deal with more tabs */
    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            notebook_create_new_tab(notebook, argv[i]);
        }
    }

    gtk_main();
}
