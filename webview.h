#pragma once

#include "rose.h"

#include <webkit2/webkit2.h>
#include <gdk/gdk.h>

#define ROSE_TYPE_WEBVIEW rose_webview_get_type()

G_DECLARE_FINAL_TYPE(RoseWebView, rose_webview, ROSE, WEBVIEW, WebKitWebView)

typedef enum {
  ROSE_WEBVIEW_NAV_BACK = 1 << 0,
  ROSE_WEBVIEW_FORWARD  = 1 << 1
} RoseWebViewNavigationFlags;

/* Create a new webview in the form of a Gtk Object. */
GtkWidget* rose_webview_new();

/* Load the given `url` into the webview. */
void rose_webview_load_url(WebKitWebView *webview, const char *url);

/* Called when the URL changes, adds a new entry to the history
   file. */
void rose_load_changed_callback(WebKitWebView *webview,
                                WebKitLoadEvent event);
