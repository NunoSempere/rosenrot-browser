#include <stdbool.h>
#include <gdk/gdk.h>
// Previously: #include <gdk/gdkkeysyms.h>
// But GTK3 discourages including the individual headers
// In GTK4, the location also changes to <gdk/gdkenums.h>

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
#define SEARCH "https://search.brave.com/search?q=%s"
// #define SEARCH "https://search.nunosempere.com/search?q=%s"
// #define SEARCH "https://lite.duckduckgo.com/html/?q=%s" 
#define HOME "https://search.brave.com/search?q=%s" 
// ^ Could also be a website ("https://search.nunosempere.com"), or a file ("file:///opt/rose/homepage.png")
// #define HOME "https://search.nunosempere.com/"
// #define HOME "file:///opt/rosenrot/rose.png" 

// Plugins
#define LIBRE_REDIRECT_ENABLED true
#define READABILITY_ENABLED true
#define CUSTOM_STYLE_ENABLED true
#define CUSTOM_USER_AGENT false
/*
To disable plugins:
1. set their corresponding variable to false
2. you could also look into this file at commit afe93518a for an approach using stand-in code.
3. recompile 

To remove plugins completely;
1. Remove the corresponding code in the rosenrot.c by looking for the variables above.
2. Remove PLUGIN and $(PLUGIN) from the makefile
3. Recompile
*/

// Webkit settings
// See: https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html 
#define WEBKIT_DEFAULT_SETTINGS \
	"enable-back-forward-navigation-gestures", true, "enable-developer-extras", true, \
	"enable-smooth-scrolling", false, \
    "default-charset", "utf-8"
/* CACHE */
#define DATA_DIR "/home/nuno/.cache/rosenrot"
#define DATA_MANAGER_OPTS "base-cache-directory", DATA_DIR, "base-data-directory", DATA_DIR

// GTK 
#define GTK_SETTINGS_CONFIG_H "gtk-application-prefer-dark-theme", false, "gtk-enable-animations", false
#define KEY(x) GDK_KEY_##x
/*
There are two different constants for Page_Up/Page_Down:
This could possibly have something to so with having Page_Down/Page_Up
as part of a keypad with/without NumLock
See: https://docs.gtk.org/gdk3/?q=Page_Up
See: https://docs.gtk.org/gdk3/?q=GDK_KEY_KP
*/

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
// reference: <https://github.com/GNOME/gtk/blob/7ea7d5c3906ccd231b04654101bb742f157d82f6/gdk/gdkenums.h#L140>

static struct {
	unsigned mod;
	unsigned key;
	func id;
} shortcut[] = {
    { CTRL,        KEY(h),             goback             },
    { CTRL,        KEY(j),             goforward          },
    { CTRL,        KEY(r),             refresh            },
    { CTRL,        KEY(R),             refresh_force      },
    { CTRL,        KEY(H),             back_to_home       },
    { CTRL,        KEY(equal),         zoomin             },
    { CTRL,        KEY(minus),         zoomout            },
    { CTRL,        KEY(0),             zoom_reset         },
    { CTRL,        KEY(KP_Page_Up),    prev_tab           }, /* also try KEY(Page_Up) if this doesn't work on your machine */
    { CTRL,        KEY(KP_Page_Down),  next_tab           }, /* ditto for KEY(Page_Down) */
    { CTRL,        KEY(t),             new_tab            },
    { CTRL,        KEY(w),             close_tab          },
    { 0x0,         KEY(F11),           toggle_fullscreen  },
    { CTRL,        KEY(l),             show_searchbar     },
    { CTRL,        KEY(semicolon),     hide_bar           },
    { CTRL,        KEY(f),             show_finder        },
    { CTRL,        KEY(n),             finder_next        },
    { CTRL,        KEY(N),             finder_prev        },
    { CTRL,        KEY(p),             prettify           }
};
/* ^ For controls more akin to normal browsers */
/* Reference for the key shorthand:
 * <https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h> */

/* Old controls: {
    { CTRL,	       KEY(h),     goback            },
    { CTRL,	       KEY(l),     goforward         },
    { CTRL,	       KEY(r),     refresh	         },
    { CTRL | SFT,  KEY(R),     refresh_force     },
    { CTRL | SFT,  KEY(H),     back_to_home      },
    { CTRL,	       KEY(equal), zoomin	           },
    { CTRL,	       KEY(minus), zoomout	         },
    { CTRL,	       KEY(0),     zoom_reset	       },
    { ALT,	       KEY(h),     prev_tab	         },
    { CTRL,        KEY(k),     hide_searchbar    },
    { ALT,	       KEY(l),     next_tab	         },
    { CTRL,	       KEY(w),     close_tab	       },
    { 0x0,	       KEY(F11),   toggle_fullscreen },
    { CTRL,	       KEY(e),     show_searchbar	   },
    { CTRL,	       KEY(f),     show_finder       },
    { CTRL,	       KEY(n),     finder_next       },
    { CTRL | SFT,  KEY(N),     finder_prev	     },
    { CTRL,        KEY(p),     prettify          }
};
*/

