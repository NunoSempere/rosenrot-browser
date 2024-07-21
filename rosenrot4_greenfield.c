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
  gtk_notebook_set_show_tabs(notebook, false);
  gtk_notebook_set_show_border(notebook, false);

  // ...

  // Show the application window
  gtk_window_present (GTK_WINDOW (window));
  gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(notebook));

  // Enter the main event loop, and wait for user interaction
  while (!0)
    g_main_context_iteration (NULL, TRUE);

  // The user lost interest
  return 0;
}
