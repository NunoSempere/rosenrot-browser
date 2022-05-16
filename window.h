#pragma once

#include "rose.h"

#include <gtk/gtk.h>
#include <gdk/x11/gdkx.h>

#define ROSE_TYPE_WINDOW rose_window_get_type()

G_DECLARE_FINAL_TYPE(RoseWindow, rose_window, ROSE, WINDOW, GtkApplicationWindow)

/* Create a new GObject, which is our window. */
RoseWindow* rose_window_new();

/* Set up all the things and show the window. */
guint rose_window_show(GtkApplication *app, RoseWindow *window, const char *url);

void rose_window_set_webview(RoseWindow *window, GtkWidget *webview);
