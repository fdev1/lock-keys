#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#if 0
#include <gdk/gdkx.h>
#include <Xlib.h>
#include <XKBlib.h>
#include <extensions/XKB.h>
#include <pthread.h>
#endif
#include "settings.h"

#if !defined(LK_TRAY_ICON_ON)
	#define LK_TRAY_ICON_ON			"tray_on.png"
#endif

#if !defined(LK_TRAY_ICON_OFF)
	#define LK_TRAY_ICON_OFF		"tray_off.png"
#endif

GtkWidget* label;
GtkWidget* window;
GtkStatusIcon* tray_icon;
const int count_down = 15;

static gboolean autohide_window(gpointer data)
{

	/*
	 * We're using a polling workaround for now until we
	 * can figure out what the problem with XGrabKey is.
	 */

	static char visible = 0;
	static char caps_lock = -1;
	static char count = 0;
	char caps;
	
	gchar* markup;

#if 0
	Display* display;
	display = XOpenDisplay(NULL);

	if (display)
	{
		unsigned n;
		XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
		caps = n & 0x01;
		XCloseDisplay(display);
#else
		caps = gdk_keymap_get_caps_lock_state(gdk_keymap_get_default());
#endif

		if (caps_lock == -1)
		{
			caps_lock = caps;
		}
		else if (caps_lock != caps)
		{
			count = -1;
			caps_lock = caps;
			if (caps)
			{
				gtk_status_icon_set_from_file(tray_icon, LK_TRAY_ICON_ON);
				markup = g_markup_printf_escaped("<span size=\"100000\">A</span>");
			}
			else
			{
				gtk_status_icon_set_from_file(tray_icon, LK_TRAY_ICON_OFF);
				markup = g_markup_printf_escaped("<span size=\"100000\">a</span>");
			}
			gtk_label_set_markup((GtkLabel*)label, markup);
			g_free(markup);

			if (!visible)
				gtk_widget_show(GTK_WIDGET(window));

			visible = 1;
		}

		if (visible)
		{
			if (++count == count_down)
			{
				gtk_widget_hide((GtkWidget*) data);
				visible = 0;
			}
		}
#if 0
	}
	else
	{
		/*
		 * TODO: log an error somewhere
		 */
		if (visible)
		{
			gtk_widget_hide(GTK_WIDGET(window));
			visible = 0;
		}
	}
#endif
    return TRUE;
}

static void create_window()
{
	gint width, height;
    gint screen_width, screen_height;
    GdkScreen* screen;

    width = 150;
    height = 150;

    window = gtk_window_new (GTK_WINDOW_POPUP);
    screen = gtk_window_get_screen((GtkWindow*)window);
    screen_width = gdk_screen_get_width(screen);
    screen_height = gdk_screen_get_height(screen);
    
    gtk_window_set_keep_above((GtkWindow*)window, TRUE);
    gtk_window_resize((GtkWindow*)window, width, height);
    gtk_window_move((GtkWindow*)window, (screen_width / 2) - (width / 2), (screen_height) - (height) - 100);
    gtk_widget_set_opacity((GtkWidget*)window, lk_settings.opacity);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
 
    label = gtk_label_new("A");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_widget_show(label);
	

}

#if 0
static GdkFilterReturn root_window_filter(GdkXEvent* xevent, GdkEvent* event, gpointer data)
{
	
	g_print("%s", "Event Received");
	
	return GDK_FILTER_CONTINUE;
}
#endif

void tray_icon_activate(GtkStatusIcon *status_icon, gpointer user_data)
{
	settings_dialog_show();
    gtk_widget_set_opacity(GTK_WIDGET(window), lk_settings.opacity);
}

int main( int   argc, char *argv[] )
{

	gtk_init (&argc, &argv);

	create_window();

	/*
	 * create tray icon
	 */
	if (gdk_keymap_get_caps_lock_state(gdk_keymap_get_default()))
		tray_icon = gtk_status_icon_new_from_file(LK_TRAY_ICON_ON);
	else
		tray_icon = gtk_status_icon_new_from_file(LK_TRAY_ICON_OFF);
	g_signal_connect(tray_icon, "activate", G_CALLBACK(tray_icon_activate), NULL);


#if 0
	gdk_window_add_filter(
		gdk_get_default_root_window(),
		root_window_filter,
		NULL);

	XGrabKey(
		gdk_x11_get_default_xdisplay(),
		AnyKey,
		AnyModifier,
		gdk_x11_get_default_root_xwindow(),
		FALSE,
		GrabModeAsync,
		GrabModeAsync);
#endif

    g_timeout_add(100, autohide_window, window);

    gtk_main();

    return 0;
}
