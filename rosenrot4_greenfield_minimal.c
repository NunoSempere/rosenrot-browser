#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "plugins/plugins.h"
#include <webkit/webkit.h>

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

#define NOTNULL(x)                                       \
    do {                                                 \
        if (x == NULL) {                                 \
            printf("\nNull found");                      \
            printf("@ %s (%d): ", __FILE__, __LINE__);   \
            exit(0);                                     \
        }                                                \
    } while (0)


/* Utils */
WebKitWebView* notebook_get_webview(GtkNotebook* notebook)
{
    WebKitWebView* view = WEBKIT_WEB_VIEW(gtk_notebook_get_nth_page(notebook, gtk_notebook_get_current_page(notebook)));
    NOTNULL(view);
    return view;
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

static gboolean handle_signal_keypress(GtkWidget* w,
    guint keyval,
    guint keycode,
    GdkModifierType state,
    GtkEventControllerKey* event_controller_keypress)
{

    fprintf(stdout, "New keypress!\n");
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
static int handle_signal_keypress2(void *self, int keyval, int keycode,
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
	g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress2), NULL);
	gtk_widget_add_controller(GTK_WIDGET(view), event_controller);
    NOTNULL(view);
    return view;
}

void notebook_create_first_tab(GtkNotebook* notebook, const char* uri)
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
	g_signal_connect(event_controller, "key-pressed", G_CALLBACK(handle_signal_keypress2), NULL);
	gtk_widget_add_controller(GTK_WIDGET(window), event_controller);
    // GtkEventController* event_controller_keypress = gtk_event_controller_key_new();
    // g_signal_connect_object(event_controller_keypress, "key-pressed", G_CALLBACK(handle_signal_keypress), notebook, G_CONNECT_DEFAULT);
    // gtk_widget_add_controller(GTK_WIDGET(notebook), event_controller_keypress);

    // Show the application window
    gtk_window_present(window);

    char* first_uri = argc > 1 ? argv[1] : HOME;
    notebook_create_first_tab(notebook, first_uri);

    /* Show to user */
    gtk_widget_set_visible(GTK_WIDGET(window), 1);

    // Enter the main event loop, and wait for user interaction
    printf("\nEntering main loop");
    while (g_list_model_get_n_items(gtk_window_get_toplevels()) > 0)
        g_main_context_iteration(NULL, TRUE);

    return 0;
}
