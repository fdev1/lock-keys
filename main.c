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
#include "overlay.h"

#if !defined(LK_TRAY_ICON_ON)
	#define LK_TRAY_ICON_ON			"tray_on.png"
#endif

#if !defined(LK_TRAY_ICON_OFF)
	#define LK_TRAY_ICON_OFF		"tray_off.png"
#endif

GtkWidget* label;
GtkWidget* window;
GtkStatusIcon* tray_icon;

static gboolean autohide_window(gpointer data)
{

	/*
	 * We're using a polling workaround for now until we
	 * can figure out what the problem with XGrabKey is.
	 */

	static char caps_lock = -1;
	char caps;

	caps = gdk_keymap_get_caps_lock_state(gdk_keymap_get_default());

	if (caps_lock == -1)
	{
		caps_lock = caps;
	}
	else if (caps_lock != caps)
	{
		caps_lock = caps;
		if (caps)
		{
			gtk_status_icon_set_from_file(tray_icon, LK_TRAY_ICON_ON);
			overlay_caps_lock_set(caps);
			overlay_show();
		}
		else
		{
			gtk_status_icon_set_from_file(tray_icon, LK_TRAY_ICON_OFF);
			overlay_caps_lock_set(caps);
			overlay_show();
		}
	}

	return TRUE;
}

void tray_icon_activate(GtkStatusIcon *status_icon, gpointer user_data)
{
	settings_dialog_show();
	overlay_state_set(lk_settings.overlay);
	overlay_opacity_set(lk_settings.opacity);
	overlay_timeout_set(lk_settings.timeout);
}

int main( int   argc, char *argv[] )
{

	gtk_init (&argc, &argv);

	settings_load();
	overlay_state_set(lk_settings.overlay);
	overlay_opacity_set(lk_settings.opacity);
	overlay_timeout_set(lk_settings.timeout);

	/*
	 * create tray icon
	 */
	if (gdk_keymap_get_caps_lock_state(gdk_keymap_get_default()))
	{
		tray_icon = gtk_status_icon_new_from_file(LK_TRAY_ICON_ON);
		overlay_caps_lock_set(TRUE);
	}
	else
	{
		tray_icon = gtk_status_icon_new_from_file(LK_TRAY_ICON_OFF);
		overlay_caps_lock_set(FALSE);
	}
	g_signal_connect(tray_icon, "activate", G_CALLBACK(tray_icon_activate), NULL);


    g_timeout_add(100, autohide_window, window);

    gtk_main();

    return 0;
}
