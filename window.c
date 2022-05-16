#include "window.h"

#define LENGTH(x) ((int) (sizeof(x) / sizeof(x[0])))

struct _RoseWindow {
	GtkApplicationWindow parent_instance;
	guint xid;

	GtkWindow *window;
	GtkApplication *app;

	WebKitWebView *webview[9];
	WebKitWebInspector *inspector;
	WebKitFindOptions *findopts;

	GHashTable *action_groups;
	GHashTable *action_labels;
};

enum {
	PROP_ACTIVE_CHILD
};

static float zoom = 1.0;
static unsigned tabid = 0;

G_DEFINE_TYPE(RoseWindow, rose_window, GTK_TYPE_APPLICATION_WINDOW)

static void read_clipboard(GObject *object,
                           GAsyncResult *res,
                           gpointer webview)
{
	GdkClipboard *clipboard = GDK_CLIPBOARD(object);
	webkit_web_view_load_uri(
		WEBKIT_WEB_VIEW(webview),
		gdk_clipboard_read_text_finish(clipboard, res, NULL)
	);
}

static void new_tab(RoseWindow *window, unsigned id)
{
	window->webview[id] = WEBKIT_WEB_VIEW(webkit_web_view_new());

	if (!options[HOMEPAGE])
		options[HOMEPAGE] = "https://duckduckgo.com";

	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(window->webview[id]),
													 options[HOMEPAGE]);

	/* gtk_widget_hide(GTK_WIDGET(window->webview[id])); */

	gtk_window_set_child(
		GTK_WINDOW(
			gtk_widget_get_root(
				GTK_WIDGET(
					window->webview[tabid++]
					)
				)
			),
		GTK_WIDGET(window->webview[id])
	);

	gtk_widget_show(GTK_WIDGET(window->webview[tabid]));
}

static void goto_tab(RoseWindow *window, unsigned id)
{
	if (!window->webview[id]) {
		new_tab(window, id);
		return;
	}

	gtk_window_set_child(
		GTK_WINDOW(
			gtk_widget_get_root(
				GTK_WIDGET(
					window->webview[tabid]
					)
				)
			),
		GTK_WIDGET(window->webview[id])
	);

	/* gtk_widget_hide(GTK_WIDGET(window->webview[tabid])); */
	gtk_widget_show(GTK_WIDGET(window->webview[id]));
}

static gboolean key_press_callback(RoseWindow *window,
                                   guint keyval,
                                   guint keycode,
                                   GdkModifierType state)
{
	(void) keycode;

	for (int i = 0; i < LENGTH(keys); i++) {
		if (keys[i].modkey == state
				&& keys[i].keycod == keyval) {
			switch (keys[i].funcid) {
				case goback:
					webkit_web_view_go_back(window->webview[tabid]);
					break;

				case goforward:
					webkit_web_view_go_forward(window->webview[tabid]);
					break;

				case copy_url: {
					GdkDisplay *dpy = gdk_display_get_default();
					gdk_clipboard_set_text(
							gdk_display_get_clipboard(dpy),
							webkit_web_view_get_uri(window->webview[tabid])
					);
				} break;

				case paste_url: {
					GdkDisplay *dpy = gdk_display_get_default();
					GdkClipboard *clipboard = gdk_display_get_clipboard(dpy);
					gdk_clipboard_read_text_async(clipboard, NULL, read_clipboard, window->webview[tabid]);
				} break;

				case fullscreen:
					if (gtk_window_is_fullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview[tabid])))))
						gtk_window_unfullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview[tabid]))));
					else
						gtk_window_fullscreen(GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(window->webview[tabid]))));
					break;

				case search: {
					int id = fork();
					if (id == 0) {
						if (glob_dpy)
							close(ConnectionNumber(glob_dpy));
						setsid();
						char* argument_list[] = { "/bin/sh", "-c", "dmenu_rose", NULL};
						execvp("/bin/sh", argument_list);
						perror(" failed");
						exit(1);
					} else {
						wait(&id);
						char *uri;
						if (strcmp((uri = (char*)getatom(AtomGo)), ""))
							rose_webview_load_url(window->webview[tabid], uri);
					}
				} break;

				case find: {
					int id = fork();
					if (id == 0) {
						if (glob_dpy)
							close(ConnectionNumber(glob_dpy));
						setsid();
						char* argument_list[] = { "/bin/sh", "-c", "dmenu_rose\tfind", NULL};
						execvp("/bin/sh", argument_list);
						perror(" failed");
						exit(1);
					} else {
						wait(&id);
						WebKitFindController *finder = webkit_web_view_get_find_controller(window->webview[tabid]);
						webkit_find_controller_search(
							finder,
							getatom(AtomFind),
							WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE |
							WEBKIT_FIND_OPTIONS_WRAP_AROUND, G_MAXUINT);
					}
				} break;

				case findnext: {
					WebKitFindController *finder = webkit_web_view_get_find_controller(window->webview[tabid]);
					webkit_find_controller_search_next(finder);
				} break;

				case findprev: {
					WebKitFindController *finder = webkit_web_view_get_find_controller(window->webview[tabid]);
					webkit_find_controller_search_previous(finder);
				} break;

				case zoomin:
					zoom += 0.1;
					webkit_web_view_set_zoom_level(window->webview[tabid], zoom);
					break;

				case zoomout:
					zoom -= 0.1;
					webkit_web_view_set_zoom_level(window->webview[tabid], zoom);
					break;

				case zoomreset:
					zoom = 1;
					webkit_web_view_set_zoom_level(window->webview[tabid], zoom);
					break;

				case inspector:
					window->inspector = webkit_web_view_get_inspector(window->webview[tabid]);
					if (webkit_web_inspector_is_attached(window->inspector))
						webkit_web_inspector_close(window->inspector);
					else
						webkit_web_inspector_show(window->inspector);
					return GDK_EVENT_STOP;
					break;

				case up:
					webkit_web_view_run_javascript(window->webview[tabid], "window.scrollBy(0,-100);", NULL, NULL, NULL);
					break;

				case down:
					webkit_web_view_run_javascript(window->webview[tabid], "window.scrollBy(0,100);", NULL, NULL, NULL);
					break;

				case reload:
					webkit_web_view_reload(window->webview[tabid]);
					break;

				case reloadforce:
					webkit_web_view_reload_bypass_cache(window->webview[tabid]);
					break;

				case history: {
					int id = fork();
					if (id == 0) {
						setsid();
						char* argument_list[] = { "/bin/sh", "-c", "dmenu_rose\thistory", NULL};
						execvp("/bin/sh", argument_list);
						perror(" failed");
						exit(1);
					} else {
						wait(&id);
						char *uri;
						if (strcmp((uri = (char*)getatom(AtomGo)), ""))
							rose_webview_load_url(window->webview[tabid], uri);
					}
				} break;

				case gotop: {
					webkit_web_view_run_javascript(window->webview[tabid], "window.scrollTo(0,0);", NULL, NULL, NULL);
				} break;

				case gobottom: {
					webkit_web_view_run_javascript(
						window->webview[tabid],
						"window.scrollTo(0, document.body.scrollHeight);",
						NULL, NULL, NULL);
				} break;

				case tabnext: {
					if (tabid + 1 > 9)
						return GDK_EVENT_PROPAGATE;

					goto_tab(window, tabid + 1);
					printf("%i\n", tabid);
				} break;

				case tabprev: {
					if (!tabid)
						return GDK_EVENT_PROPAGATE;

					goto_tab(window, tabid - 1);
					printf("%i\n", tabid);
				} break;
			}
		}
	}


	return GDK_EVENT_PROPAGATE;
}

static void rose_window_constructed(GObject *object)
{
	G_OBJECT_CLASS(rose_window_parent_class)->constructed(object);
}

static void rose_window_init(RoseWindow *window)
{
	(void) window;
}

static void destroy()
{
	exit(0);
}

guint rose_window_show(GtkApplication *app, RoseWindow *window, const char *url)
{
	window->app = app;
	window->window = GTK_WINDOW(gtk_application_window_new(window->app));

	/* gtk_application_set_menubar(window->app, FALSE); */
	window->webview[tabid] = WEBKIT_WEB_VIEW(rose_webview_new());

	g_signal_connect(G_OBJECT(window->window), "destroy",
									 G_CALLBACK(destroy), NULL);

	g_signal_connect(G_OBJECT(window->webview[tabid]), "web-process-terminated",
			 G_CALLBACK(destroy), NULL);

	g_signal_connect(G_OBJECT(window->webview[tabid]), "load-changed",
			 G_CALLBACK(rose_load_changed_callback), window->webview[tabid]);

	if (url) {
		rose_webview_load_url(WEBKIT_WEB_VIEW(window->webview[tabid]), url);
		setatom(AtomUri, url);
	}

	if (!(appearance[WIDTH] && appearance[HEIGHT])) {
		appearance[WIDTH]  = 1280;
		appearance[HEIGHT] = 720;
	}

	gtk_window_set_default_size(GTK_WINDOW(window->window), appearance[WIDTH], appearance[HEIGHT]);

	gtk_window_set_child(GTK_WINDOW(window->window), GTK_WIDGET(window->webview[tabid]));

	GtkEventController *controller;
	controller = gtk_event_controller_key_new();
	g_signal_connect_swapped(controller, "key-pressed", G_CALLBACK(key_press_callback), window);
	gtk_widget_add_controller(GTK_WIDGET(window->window), controller);

	gtk_widget_show(GTK_WIDGET(window->window));
	gtk_widget_show(GTK_WIDGET(window->webview[tabid]));

	return 12;
}

void rose_window_set_webview(RoseWindow *window, GtkWidget *webview)
{
	window->webview[tabid] = WEBKIT_WEB_VIEW(webview);
}

static void rose_window_class_init(RoseWindowClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(class);

	object_class->constructed = rose_window_constructed;
}

RoseWindow* rose_window_new(GtkApplication *app)
{
		return g_object_new(
			GTK_TYPE_APPLICATION_WINDOW,
			"application", GTK_APPLICATION(app),
			NULL
		);
}
