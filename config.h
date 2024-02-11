#include <gdk/gdkkeysyms.h>
#include <stdbool.h>

// Key user config
#define WIDTH 1920 // 960 for half-width, 1920 for full width
#define HEIGHT 1080
#define BAR_SIZE 1000

// More user config
#define ZOOM 1.6 /* Starting zoom level.*/
#define ZOOM_VAL .1 /* Zooming value in zoomin/zoomout functions */
#define BG_COLOR "#FEFEFE" /* "FEFEFE", "#1E1E2E" */
#define DEBUG false
#define MAX_NUM_TABS 8 // set to 0 or false if you want unlimited tabs, or look at the relevant rose.c code.
#define ROSE_HOMEPAGE false
#define SEARCH "https://lite.duckduckgo.com/html/?q=%s" // "https://search.nunosempere.com/search?q=%s"
#define HOME ROSE_HOMEPAGE ? "file:///home/loki/Documents/core/software/fresh/C/rose-browser/rosenrot/user-scripts/ubuntu-20.04/rose-images/rose-homepage.png" : "https://search.nunosempere.com/"

// Webkit settings
// See: https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html 
#define WEBKIT_DEFAULT_SETTINGS \
	"enable-back-forward-navigation-gestures", true, "enable-developer-extras", true, \
	"enable-smooth-scrolling", false, \
    "default-charset", "utf-8"
/* CACHE */
#define CACHE_DIR "/home/loki/.cache/rose"
#define CACHE                                                                \
    "base-cache-directory", CACHE_DIR, "base-data-directory", CACHE_DIR,     \
        "disk-cache-directory", CACHE_DIR, "dom-cache-directory", CACHE_DIR, \
        "hsts-cache-directory", CACHE_DIR, "indexeddb-directory", CACHE_DIR, \
        "itp-directory", CACHE_DIR, "local-storage-directory", CACHE_DIR,    \
        "offline-application-cache-directory", CACHE_DIR,                    \
        "service-worker-registrations-directory", CACHE_DIR

// GTK 
#define GTK "gtk-application-prefer-dark-theme", false, "gtk-enable-animations", false
#define KEY(x) GDK_KEY_##x

// Shortcuts
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
	new_tab,
	next_tab,
	prev_tab,
	close_tab,
	show_searchbar,
	show_finder,
	finder_next,
	finder_prev,
  prettify,
  hide_bar
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
    { CTRL,        KEY(Page_Up),    prev_tab           },
    { CTRL,        KEY(Page_Down),  next_tab           },
    { CTRL,        KEY(t),          new_tab            },
    { CTRL,        KEY(w),          close_tab          },
    { 0x0,         KEY(F11),        toggle_fullscreen  },
    { CTRL,        KEY(l),          show_searchbar     },
    { CTRL,        KEY(semicolon),  hide_bar           },
    { CTRL,        KEY(f),          show_finder        },
    { CTRL,        KEY(n),          finder_next        },
    { CTRL | SFT,  KEY(N),          finder_prev        },
    { CTRL,        KEY(p),          prettify           }
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
    { CTRL,        KEY(k),     hide_searchbar    },
    { ALT,	   KEY(l),     next_tab	         },
    { CTRL,	   KEY(w),     close_tab	 },
    { 0x0,	   KEY(F11),   toggle_fullscreen },
    { CTRL,	   KEY(e),     show_searchbar	 },
    { CTRL,	   KEY(f),     show_finder       },
    { CTRL,	   KEY(n),     finder_next       },
    { CTRL | SFT,  KEY(N),     finder_prev	 },
    { CTRL,        KEY(p),     prettify          }
};
*/

