#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "plugins/plugins.h"
#include <webkit/webkit.h>

#define NOTNULL(x)                                       \
    do {                                                 \
        if (x == NULL) {                                 \
            printf("\nNull found");                      \
            printf("@ %s (%d): ", __FILE__, __LINE__);   \
            exit(0);                                     \
        }                                                \
    } while (0)

static GtkNotebook* notebook;
static GtkWindow* window;
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
    WebKitWebView* view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
    NOTNULL(view);
    return view;
}

/* Load content */

void load_uri(WebKitWebView* view, const char* uri)
{

    NOTNULL(view);
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

void set_custom_style(WebKitWebView* view)
{
    NOTNULL(view);
    if (custom_style_enabled) {
        char* style_js = malloc(STYLE_N + 1);
        read_style_js(style_js);
        webkit_web_view_evaluate_javascript(view, style_js, -1, NULL, "rosenrot-style-plugin", NULL, NULL, NULL);
        free(style_js);
    }
}

/* Handle shortcuts */
int handle_shortcut(func id)
{

    WebKitWebView* view = notebook_get_webview(notebook);
    NOTNULL(notebook);
    NOTNULL(view);

    switch (id) {
        case show_searchbar:
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 1);
            break;
        case hide_bar:
            gtk_widget_set_visible(GTK_WIDGET(bar.widget), 0);
            break;
    }

    return 1;
}

/* Listen to keypresses */

static int handle_signal_keypress(void *self, int keyval, int keycode,
							   GdkModifierType state, void *controller)
{
	// (void) self, (void) keycode, (void) controller;

    // fprintf(stdout, "New keypress!\n");
    printf("New keypress\n");
    
    if (1) {
        printf("Keypress state: %d\n", state);
        printf("Keypress value: %d\n", keyval);
    }
    for (int i = 0; i < sizeof(shortcut) / sizeof(shortcut[0]); i++){
        if ((state & shortcut[i].mod || shortcut[i].mod == 0x0) && keyval == shortcut[i].key) {
            return handle_shortcut(shortcut[i].id);
        }
    }
    
    return 0;

}

WebKitWebView* create_new_webview()
{

    WebKitWebView* view = g_object_new(WEBKIT_TYPE_WEB_VIEW, NULL);
	GtkEventController *event_controller = gtk_event_controller_key_new();
	g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress), NULL);
	gtk_widget_add_controller(GTK_WIDGET(view), event_controller);
    NOTNULL(view);
    return view;
}

GtkWidget* handle_signal_create_new_tab(WebKitWebView* self,
    WebKitNavigationAction* navigation_action,
    GtkNotebook* notebook)
{
    NOTNULL(self);
    NOTNULL(notebook);
    if (num_tabs < MAX_NUM_TABS || num_tabs == 0) {
        WebKitURIRequest* uri_request = webkit_navigation_action_get_request(navigation_action);
        const char* uri = webkit_uri_request_get_uri(uri_request);
        printf("Creating new window: %s\n", uri);
        notebook_create_new_tab(notebook, uri);
        gtk_notebook_set_show_tabs(notebook, true);
    } else {
        webkit_web_view_evaluate_javascript(self, "alert('Too many tabs, not opening a new one')", -1, NULL, "rosenrot-alert-numtabs", NULL, NULL, NULL);
    }
    return GTK_WIDGET(self); // NULL;
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
    WebKitWebView* view = create_new_webview();

    int n = gtk_notebook_append_page(notebook, GTK_WIDGET(view), NULL);
    gtk_notebook_set_tab_reorderable(notebook, GTK_WIDGET(view), true);
    gtk_widget_set_visible(GTK_WIDGET(window), 1);
    gtk_widget_hide(GTK_WIDGET(bar.widget));
    webkit_web_view_load_uri(view, (uri) ? uri : HOME);

    gtk_notebook_set_current_page(notebook, n);
    gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(view), "-");
}

/* Top bar */
void toggle_bar(GtkNotebook* notebook, Bar_entry_mode mode)
{
    NOTNULL(notebook);
    NOTNULL(window);
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
void handle_signal_bar_press_enter(void* data)
{
    WebKitWebView* view = notebook_get_webview(notebook);
    NOTNULL(notebook);
    NOTNULL(view);
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

int main(int argc, char** argv)
{
    /* Initialize GTK in general */
    gtk_init();

    // Create the main window
    window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_default_size(window, WIDTH, HEIGHT_4);

    // Set up notebook
    notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_notebook_set_show_tabs(notebook, true);
    gtk_notebook_set_show_border(notebook, false);
    gtk_window_set_child(window, GTK_WIDGET(notebook));

    // Bar
    bar.line_text = GTK_ENTRY_BUFFER(gtk_entry_buffer_new("", 0));
    bar.line = GTK_ENTRY(gtk_entry_new_with_buffer(bar.line_text));
    gtk_entry_set_alignment(bar.line, 0.48);
    gtk_widget_set_size_request(GTK_WIDGET(bar.line), BAR_SIZE, -1);

    bar.widget = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_set_title_widget(bar.widget, GTK_WIDGET(bar.line));
    gtk_window_set_titlebar(window, GTK_WIDGET(bar.widget));

    // g_signal_connect(window, "destroy", G_CALLBACK(exit), notebook);

	GtkEventController *event_controller = gtk_event_controller_key_new();
	g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress), NULL);
	gtk_widget_add_controller(GTK_WIDGET(window), event_controller);
    // GtkEventController* event_controller_keypress = gtk_event_controller_key_new();
    // g_signal_connect_object(event_controller_keypress, "key-pressed", G_CALLBACK(handle_signal_keypress), notebook, G_CONNECT_DEFAULT);
    // gtk_widget_add_controller(GTK_WIDGET(notebook), event_controller_keypress);

    // Show the application window
    gtk_window_present(window);

    char* first_uri = argc > 1 ? argv[1] : HOME;
    notebook_create_new_tab(notebook, first_uri);

    /* Show to user */
    gtk_widget_set_visible(GTK_WIDGET(window), 1);

    // Enter the main event loop, and wait for user interaction
    printf("\nEntering main loop");
    while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0)
        g_main_context_iteration(NULL, TRUE);

    return 0;
}
