/*
 * lock-keys - Caps lock indicator fo linux laptops
 * Copyright (C) 2014 Fernando Rodriguez (frodriguez.developer@outlook.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include "settings.h"
#include "overlay.h"

#if !defined(LK_TRAY_ICON_ON)
	#define LK_TRAY_ICON_ON			"tray_on.png"
#endif

#if !defined(LK_TRAY_ICON_OFF)
	#define LK_TRAY_ICON_OFF		"tray_off.png"
#endif

static GtkStatusIcon* tray_icon;
static gboolean caps_lock = FALSE;

/*
 * polls for caps lock changes at a preset interval
 */
static gboolean poll_keys(gpointer data)
{
	gboolean caps =
		gdk_keymap_get_caps_lock_state(gdk_keymap_get_default());

	if (caps_lock != caps)
	{
		caps_lock = caps;
		gtk_status_icon_set_from_file(tray_icon, caps ? LK_TRAY_ICON_ON : LK_TRAY_ICON_OFF);
		overlay_caps_lock_set(caps);
		overlay_show();
	}

	return TRUE;
}

/*
 * configures the overlay window
 */
static inline void configure_overlay()
{
	overlay_state_set(lk_settings.overlay);
	overlay_opacity_set(lk_settings.opacity);
	overlay_timeout_set(lk_settings.timeout);
}

/*
 * handle tray icons activate signals (shows
 * settings dialog and reconfigures overlay window
 */
static void tray_icon_activate(GtkStatusIcon *status_icon, gpointer user_data)
{
	settings_dialog_show();
	configure_overlay();
}

int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	settings_load();
	configure_overlay();
	caps_lock = gdk_keymap_get_caps_lock_state(gdk_keymap_get_default());
	tray_icon = gtk_status_icon_new_from_file(caps_lock ? LK_TRAY_ICON_ON : LK_TRAY_ICON_OFF);
	overlay_caps_lock_set(caps_lock);
	g_signal_connect(tray_icon, "activate", G_CALLBACK(tray_icon_activate), NULL);

	/*
	 * TODO: see if we can get this working without polling
	 */
    g_timeout_add(100, poll_keys, NULL);

    gtk_main();

    return 0;
}
