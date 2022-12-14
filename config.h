/*
 * Copyright (c) 2022 Nuño Sempere.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and any associated documentation
 * files to modify, copy, merge, publish, distribute and/or
 * sublicense copies of this sotware for their own use.
 * This code does not come with any warranty.
 *
 */

#include <gdk/gdkkeysyms.h>
#include <stdbool.h>

/* See more:
 * https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html */
#define WEBKIT                                                                                     \
	"enable-back-forward-navigation-gestures", true, "enable-developer-extras", true,          \
	    "enable-smooth-scrolling", false

#define GTK "gtk-application-prefer-dark-theme", false, "gtk-enable-animations", false
#define ROSE_HOMEPAGE true
#define HOME ROSE_HOMEPAGE ? "file:///home/loki/Documents/core/software/fresh/C/rose-browser/rose-bud-personal/user-scripts/ubuntu-20.04/rose-images/rose-homepage.png" : "https://lite.duckduckgo.com/html"
#define SEARCH "https://lite.duckduckgo.com/html/?q=%s"
#define CACHE_DIR "/home/loki/.cache/rose"

#define WIDTH 1920
#define HEIGHT 1080
#define KEY(x) GDK_KEY_##x
#define ZOOM 1.4  /* Starting zoom level.*/
#define ZOOM_VAL .1 /* Zooming value in zoomin/zoomout functions */
#define BG_COLOR "#FEFEFE" /*"#1E1E2E" */
#define DEBUG false

typedef enum {
	goback,
	goforward,
	refresh,
	refresh_force,
	back_to_home,
	toggle_fullscreen,
	zoomin,
	zoomout,
	zoom_reset,
	next_tab,
	prev_tab,
	close_tab,
	show_searchbar,
	show_finder,
	finder_next,
	finder_prev
} func;

#define SFT  1 << 0
#define CTRL 1 << 2
#define ALT  1 << 3

static struct {
	unsigned mod;
	unsigned key;
	func id;
} keys[] = {
    { CTRL,        KEY(h),          goback             },
    { CTRL,        KEY(j),          goforward          },
    { CTRL,        KEY(r),          refresh            },
    { CTRL | SFT,  KEY(R),          refresh_force      },
    { CTRL | SFT,  KEY(H),          back_to_home       },
    { CTRL,        KEY(equal),      zoomin             },
    { CTRL,        KEY(minus),      zoomout            },
    { CTRL,        KEY(0),          zoom_reset         },
    { CTRL,        KEY(Page_Down),  prev_tab           },
    { CTRL,        KEY(Page_Up),    next_tab           },
    { CTRL,        KEY(t),          next_tab           },
    { CTRL,        KEY(w),          close_tab          },
    { 0x0,         KEY(F11),        toggle_fullscreen  },
    { CTRL,        KEY(l),          show_searchbar     },
    { CTRL,        KEY(f),          show_finder        },
    { CTRL,        KEY(n),          finder_next        },
    { CTRL | SFT,  KEY(N),          finder_prev        }
};
/* ^ For controls more akin to normal browsers */
/* Reference for the key shorthand:
 * <https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h> */

/* Old controls: {
    { CTRL,	   KEY(h),     goback            },
    { CTRL,	   KEY(l),     goforward         },
    { CTRL,	   KEY(r),     refresh	         },
    { CTRL | SFT,  KEY(R),     refresh_force     },
    { CTRL | SFT,  KEY(H),     back_to_home      },
    { CTRL,	   KEY(equal), zoomin	         },
    { CTRL,	   KEY(minus), zoomout	         },
    { CTRL,	   KEY(0),     zoom_reset	 },
    { ALT,	   KEY(h),     prev_tab	         },
    { ALT,	   KEY(l),     next_tab	         },
    { CTRL,	   KEY(w),     close_tab	 },
    { 0x0,	   KEY(F11),   toggle_fullscreen },
    { CTRL,	   KEY(e),     show_searchbar	 },
    { CTRL,	   KEY(f),     show_finder       },
    { CTRL,	   KEY(n),     finder_next       },
    { CTRL | SFT,  KEY(N),     finder_prev	 }
};
*/
