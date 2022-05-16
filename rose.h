#pragma once

#include "window.h"
#include "webview.h"
#include "config.h"

#include <gtk/gtk.h>
#include <gdk/x11/gdkx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <stdlib.h>
#include <webkit2/webkit2.h>
#include <sys/wait.h>

enum {
	AtomFind,
	AtomGo,
	AtomUri,
	AtomUTF8,
	AtomLast
};

extern Display *glob_dpy;  /* declared in rose.c */

/* Set/get X11 window properties. `getatom` retuns an allocated string
   which has to be free'd. */
char *getatom(int aid);
void setatom(int aid, const char *val);
