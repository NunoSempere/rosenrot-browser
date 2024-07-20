#include <stdbool.h>
#include <gdk/gdk.h> // <gdk/gdkenums.h>, <gdk/gdkkeysyms.h>

// GKG 3 or GKG 4
# define GKG_NUM 4

// Key user config
#define WIDTH 1920 // 960 for half-width, 1920 for full width
// #define HEIGHT 1080
#define HEIGHT 990
#define BAR_SIZE 960

// More user config
#define ZOOM_START_LEVEL 1.6 
#define ZOOM_STEPSIZE .1 
#define MAX_NUM_TABS 8 // 0/false for unlimited tabs
#define SEARCH "https://search.brave.com/search?q=%s" // "https://search.nunosempere.com/search?q=%s", "https://lite.duckduckgo.com/html/?q=%s" 
#define HOME "https://search.brave.com/search"  // "file:///opt/rose/homepage.png", ""

// Plugins
#define LIBRE_REDIRECT_ENABLED true
#define READABILITY_ENABLED true
#define CUSTOM_USER_AGENT false
/*
To disable plugins:
1. set their corresponding variable to false
2. recompile 

To remove plugins completely;
1. Remove the corresponding code in rosenrot.c by looking for the variables above.
2. Remove PLUGIN and $(PLUGIN) from the makefile
3. Recompile

You could also look into commit afe93518a for an approach using stand-in code.
*/

// Webkit settings
// https://webkitgtk.org/reference/webkit2gtk/stable/class.Settings.html 
#define WEBKIT_DEFAULT_SETTINGS \
	"enable-back-forward-navigation-gestures", true, "enable-developer-extras", true, \
	"enable-smooth-scrolling", false, \
    "default-charset", "utf-8"
#define DATA_DIR "/home/nuno/.cache/rosenrot"
#define DATA_MANAGER_OPTS "base-cache-directory", DATA_DIR, "base-data-directory", DATA_DIR

// GTK 
#define GTK_SETTINGS_CONFIG_H "gtk-application-prefer-dark-theme", false, "gtk-enable-animations", false
// https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h
#define KEY(x) GDK_KEY_##x
// https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkenums.h
#define SFT  1 << 0 
#define CTRL 1 << 2
#define ALT  1 << 3

// Shortcuts
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

    prettify,
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

    { CTRL,        KEY(KP_Page_Up),    prev_tab             }, 
    { CTRL,        KEY(KP_Page_Down),  next_tab             }, 
    { CTRL,        KEY(Page_Up),       prev_tab             }, // working hypothesis: Page_UP vs KP_Page_Up might depend on whether the user has a numpad
    { CTRL,        KEY(Page_Down),     next_tab             }, 
    { CTRL,        KEY(t),             new_tab              },
    { CTRL,        KEY(w),             close_tab            },

    { CTRL,        KEY(l),             show_searchbar       },
    { CTRL,        KEY(semicolon),     hide_bar             },
    { CTRL,        KEY(f),             show_finder          },
    { CTRL,        KEY(n),             finder_next          },
    { CTRL,        KEY(N),             finder_prev          },

    { CTRL,        KEY(p),             prettify             }
};

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
