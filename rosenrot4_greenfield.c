#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#include <webkit/webkit.h>
#include "config.h"
#include "plugins/plugins.h"

static GtkNotebook* notebook;
static GtkWidget *window;
typedef enum { _SEARCH, _FIND, _HIDDEN } Bar_entry_mode;
static struct {
    GtkHeaderBar* widget;
    GtkEntry* line;
    GtkEntryBuffer* line_text;
    Bar_entry_mode entry_mode;
} bar;
static int num_tabs = 0;
static int custom_style_enabled = 1;

WebKitWebView* create_new_webview()
{
    char* style;
    WebKitSettings* settings;
    // WebKitWebContext* web_context;
    WebKitCookieManager* cookiemanager;
    WebKitNetworkSession* network_session; // new
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
    // web_context = webkit_web_context_new_with_website_data_manager(webkit_website_data_manager_new(DATA_MANAGER_OPTS, NULL));
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

void load_uri(WebKitWebView* view, const char* uri)
{
    if (strlen(uri) == 0) {
        webkit_web_view_load_uri(view, "");
        // toggle_bar(notebook, _SEARCH);
    } else if (g_str_has_prefix(uri, "http://") || g_str_has_prefix(uri, "https://") || g_str_has_prefix(uri, "file://") || g_str_has_prefix(uri, "about:")) {
        webkit_web_view_load_uri(view, uri);
    }
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
        // gtk_notebook_set_tab_label_text(notebook, GTK_WIDGET(view), "-");
        num_tabs += 1;
    } else {
    }
}


int
main (int argc, char **argv)
{
  // Initialize i18n support with bindtextdomain(), etc.

  // ...

  // Initialize the widget set
  gtk_init ();

  // Create the main window
  window = gtk_window_new ();
  gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT_4);

  // Set up our GUI elements

  notebook = GTK_NOTEBOOK(gtk_notebook_new());
  gtk_notebook_set_show_tabs(notebook, true);
  gtk_notebook_set_show_border(notebook, false);
  // ...

  // Show the application window
  gtk_window_present (GTK_WINDOW (window));
  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(notebook));


  char* first_uri = argc > 1 ? argv[1] : HOME;
  notebook_create_new_tab(notebook, first_uri);


  // Enter the main event loop, and wait for user interaction
  while (!0)
    g_main_context_iteration (NULL, TRUE);

  // The user lost interest
  return 0;
}
