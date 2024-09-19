#include <stdbool.h>
#include <gtk/gtk.h>

/* Key user config */
#define HEIGHT      1080 
#define FULL_WIDTH  1920
#define WIDTH       FULL_WIDTH
#define BAR_WIDTH   FULL_WIDTH/2

/* More user config */
#define ZOOM_START_LEVEL 1.8
#define ZOOM_STEPSIZE .1 
#define MAX_NUM_TABS 8 // 0 or false for unlimited tabs
#define SEARCH "https://search.brave.com/search?q=%s" 
#define HOME "https://search.brave.com/search" 
// #define SEARCH "https://search.nunosempere.com/search?q=%s"
// #define SEARCH "https://lite.duckduckgo.com/html/?q=%s" 
// #define HOME "file:///opt/rosenrot/flower-imgs/rose-homepage.png" 

/* Plugins */
#define LIBRE_REDIRECT_ENABLED true
#define READABILITY_ENABLED true
#define CUSTOM_USER_AGENT false
/* 
To disable plugins:
1. set their corresponding variable to false
2. recompile 

To remove plugins completely;
1. Remove the corresponding code in rosenrot.c by looking for the variables above, as well as custom_style_enabled
2. Remove PLUGIN and $(PLUGIN) from the makefile
3. Recompile

You could also look into commit afe93518a for an approach using stand-in code.
*/

/* Webkit */
// https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html 
#define WEBKIT_DEFAULT_SETTINGS \
	"enable-back-forward-navigation-gestures", true, \
	"enable-developer-extras", true, \
	"enable-smooth-scrolling", false, \
    "default-charset", "utf-8"
#define DATA_DIR "/home/nuno/.cache/rosenrot"
#define DATA_MANAGER_OPTS "base-cache-directory", DATA_DIR, "base-data-directory", DATA_DIR
#define NETWORK_SESSION_OPTS DATA_DIR, DATA_DIR

/* GTK  */
// https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h
// https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkenums.h
#define GTK_SETTINGS_CONFIG_H "gtk-application-prefer-dark-theme", false, "gtk-enable-animations", false
#define KEY(x) GDK_KEY_##x
#define SFT  1 << 0 
#define CTRL 1 << 2
#define ALT  1 << 3

/* Misc helpers */
#define ABORT_REQUEST_ON_CURRENT_TAB NULL
#define NULLCHECK(x)                                   \
    do {                                               \
        if (x == NULL) {                               \
            printf("\nNULL check not passed");         \
            printf("@ %s (%d): ", __FILE__, __LINE__); \
            exit(0);                                   \
        }                                              \
    } while (0)

/* Shortcuts */
typedef enum {
    goback,
    goforward,

    refresh,
    refresh_force,

    back_to_home,

    toggle_fullscreen,
    toggle_custom_style,

    zoomin,
    zoomout,
    zoom_reset,

    new_tab,
    next_tab,
    prev_tab,
    close_tab,

    show_searchbar,
    hide_bar,
    show_finder,
    finder_next,
    finder_prev,
    filter,

    halve_window,
    rebig_window,

    prettify,
    save_uri_to_txt,
    open_uri_in_brave,
} func;

static struct {
	unsigned mod;
	unsigned key;
	func id;
} shortcut[] = {
    { CTRL,        KEY(h),             goback               },
    { CTRL,        KEY(j),             goforward            },

    { CTRL,        KEY(r),             refresh              },
    { CTRL,        KEY(R),             refresh_force        },

    { CTRL,        KEY(H),             back_to_home         },

    { 0x0,         KEY(F11),           toggle_fullscreen    },
    { CTRL,        KEY(S),             toggle_custom_style  },

    { CTRL,        KEY(equal),         zoomin               },
    { CTRL,        KEY(minus),         zoomout              },
    { CTRL,        KEY(0),             zoom_reset           },

    { CTRL | SFT,  KEY(KP_Page_Up),    prev_tab             },  // use SFT just to show one can
    { CTRL | SFT,  KEY(KP_Page_Down),  next_tab             }, 
    { CTRL | SFT,  KEY(Page_Up),       prev_tab             }, 
    // working hypothesis: Page_UP vs KP_Page_Up might depend on whether the user has a numpad
    { CTRL | SFT,  KEY(Page_Down),     next_tab             }, 
    { CTRL,        KEY(t),             new_tab              },
    { CTRL,        KEY(w),             close_tab            },

    { CTRL,        KEY(l),             show_searchbar       },
    { CTRL,        KEY(o),             hide_bar             }, // previously: KEY(semicolon)
    { CTRL,        KEY(f),             show_finder          },
    { CTRL,        KEY(n),             finder_next          },
    { CTRL,        KEY(N),             finder_prev          },
    { CTRL,        KEY(F),             filter               },
    { CTRL,        KEY(Up),            halve_window         },
    { CTRL,        KEY(Down),          rebig_window         },
    { CTRL,        KEY(p),             prettify             },
    { CTRL,        KEY(s),             save_uri_to_txt      },
    { CTRL,        KEY(b),             open_uri_in_brave    }
};
