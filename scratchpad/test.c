#include <gtk/gtk.h>

#define WIDTH 1920 // 960 for half-width, 1920 for full width
// #define HEIGHT 1080
#define HEIGHT 1080

static GtkNotebook* notebook;

int
main (int argc, char **argv)
{
 GtkWidget *window;
  // Initialize i18n support with bindtextdomain(), etc.

  // ...

  // Initialize the widget set
  gtk_init ();

  // Create the main window
  window = gtk_window_new ();
  gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);

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
