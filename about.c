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
#include "icons.h"

void about_show()
{
	static GtkWidget* about_window;
	GtkWidget* content_area;
	GtkWidget* about_label;

	if (about_window)
	{
		gtk_window_present(GTK_WINDOW(about_window));
		return;
	}

	const char* about_msg =
			"Lock-keys Version 0.3\n" \
			"Copyright (c) 2014\n\n"
			"Author: Fernando Rodriguez\n"
			"frodriguez.developer@outlook.com\n";

	about_window = gtk_dialog_new_with_buttons(
		"About Lock-keys...",
		GTK_WINDOW(NULL),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		("_OK"),
		GTK_RESPONSE_ACCEPT,
		NULL);

	/*gtk_window_set_decorated(GTK_WINDOW(about_window), TRUE);*/
	content_area = gtk_dialog_get_content_area(GTK_DIALOG(about_window));
	gtk_container_set_border_width(GTK_CONTAINER(content_area), 15);

    about_label = gtk_label_new(about_msg);
    gtk_container_add(GTK_CONTAINER(content_area), about_label);
    gtk_widget_show(about_label);

    gtk_dialog_run(GTK_DIALOG(about_window));
    gtk_widget_destroy(GTK_WIDGET(about_window));

    about_window = NULL;

}
