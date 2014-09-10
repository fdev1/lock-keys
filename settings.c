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


#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "settings.h"
#include "overlay.h"

#if !defined(LK_EXEC_PATH)
#define LK_EXEC_PATH	"/usr/local/bin/lock-keys"
#endif


static GtkWidget* settings_window = NULL;

/*
 * in-memory settings structure
 */
LK_SETTINGS lk_settings = {
	/* opacity */ 0.8,
	/* overlay */ TRUE,
	/* timeout */ 2,
	/* autostart */ FALSE
};

void check_overlay_changed(GtkToggleButton *sender, GtkWidget** widgets)
{
	gboolean checked = gtk_toggle_button_get_active(sender);
	gtk_widget_set_sensitive(*widgets++, checked);
	gtk_widget_set_sensitive(*widgets++, checked);
	gtk_widget_set_sensitive(*widgets++, checked);
	gtk_widget_set_sensitive(*widgets++, checked);
}

static void opacity_changed(GtkSpinButton *spinbutton, GtkWidget* opacity)
{
	overlay_opacity_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(opacity)));
	overlay_show();
}

static void timeout_changed(GtkSpinButton *spinbutton, GtkWidget* timeout)
{
	overlay_timeout_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(timeout)));
	overlay_show();
}

/*
 * Display the settings dialog and update the config
 * file and in-memory structure if necessary
 */
void settings_dialog_show()
{

	int row = 0;
	GtkWidget* content_area;
	GtkWidget* grid;
	GtkWidget* check_overlay;
	GtkWidget* lbl_opacity;
	GtkAdjustment* opacity_adjustment;
	GtkWidget* opacity;
	GtkWidget* lbl_timeout;
	GtkAdjustment* timeout_adjustment;
	GtkWidget* timeout;

#if LK_USE_KDE || LK_USE_KDE4
	GtkWidget* check_autostart;
#endif

	if (settings_window)
	{
		gtk_window_present(GTK_WINDOW(settings_window));
		return;
	}

	settings_window = gtk_dialog_new_with_buttons(
		"Lock-keys Settings",
		GTK_WINDOW(NULL),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		("_OK"),
		GTK_RESPONSE_ACCEPT,
		("_Cancel"),
		GTK_RESPONSE_REJECT,
		NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(settings_window));
	gtk_container_set_border_width(GTK_CONTAINER(content_area), 15);

	grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_container_add(GTK_CONTAINER(content_area), grid);
	gtk_widget_show(GTK_WIDGET(grid));

#if LK_USE_KDE || LK_USE_KDE4
	check_autostart = gtk_check_button_new_with_label("Autostart at login.");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_autostart), lk_settings.autostart);
	gtk_grid_attach(GTK_GRID(grid), check_autostart, 0, row++, 2, 1);
	gtk_widget_show(GTK_WIDGET(check_autostart));
#endif

	check_overlay = gtk_check_button_new_with_label("Show on-screen overlay");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_overlay), lk_settings.overlay);
	gtk_grid_attach(GTK_GRID(grid), check_overlay, 0, row++, 2, 1);
	gtk_widget_show(GTK_WIDGET(check_overlay));

	lbl_opacity = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lbl_opacity), 0, row++, 1, 1);
	gtk_widget_show(GTK_WIDGET(lbl_opacity));


	lbl_opacity = gtk_label_new("Opacity:");
	gtk_misc_set_alignment(GTK_MISC(lbl_opacity), 0, 0.5);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lbl_opacity), 0, row, 1, 1);
	gtk_widget_show(GTK_WIDGET(lbl_opacity));
	opacity_adjustment = gtk_adjustment_new(lk_settings.opacity, 0.10, 1.0, 0.01, 0.1, 0.0);
	opacity = gtk_spin_button_new(opacity_adjustment, 0.01, 2);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(opacity), 0.10, 1.0);
	gtk_grid_attach(GTK_GRID(grid), opacity, 1, row++, 1, 1);
	gtk_widget_show(GTK_WIDGET(opacity));
	g_signal_connect(opacity, "value-changed", G_CALLBACK(opacity_changed), opacity);


	lbl_timeout = gtk_label_new("Overlay Duration:");
	gtk_misc_set_alignment(GTK_MISC(lbl_timeout), 0, 0.5);
	gtk_misc_set_alignment(GTK_MISC(lbl_timeout), 0, 0.5);
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lbl_timeout), 0, row, 1, 1);
	gtk_widget_show(GTK_WIDGET(lbl_timeout));
	timeout_adjustment = gtk_adjustment_new((gdouble)lk_settings.timeout, 1.0, 10.0, 1.0, 1.0, 0.0);
	timeout = gtk_spin_button_new(timeout_adjustment, 1.0, 0.0);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(timeout), 1, 10);
	gtk_grid_attach(GTK_GRID(grid), timeout, 1, row++, 1, 1);
	gtk_widget_show(GTK_WIDGET(timeout));
	g_signal_connect(timeout, "value-changed", G_CALLBACK(timeout_changed), timeout);

	{
		GtkWidget* widgets[] = { lbl_opacity, opacity, lbl_timeout, timeout };
		g_signal_connect(check_overlay, "toggled", G_CALLBACK(check_overlay_changed), widgets);
	}

	if (gtk_dialog_run(GTK_DIALOG(settings_window)) == GTK_RESPONSE_ACCEPT)
	{
#if LK_USE_KDE || LK_USE_KDE4
		lk_settings.autostart = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_autostart));
#endif
		lk_settings.overlay = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_overlay));
		lk_settings.opacity = gtk_spin_button_get_value(GTK_SPIN_BUTTON(opacity));
		lk_settings.timeout = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(timeout));
		settings_save();
	}

	gtk_widget_destroy(GTK_WIDGET(settings_window));
	settings_window = NULL;

}

static gboolean readline(FILE* f, char* buffer, uint bufflen)
{
	char c;
	while (bufflen--)
	{
		if ((c = fgetc(f)) == EOF)
		{
			*buffer++ = 0;
			return TRUE;
		}

		if (c == 0 || c == '\n')
		{
			*buffer++ = 0;
			break;
		}
		*buffer++ = c;
	}
	return FALSE;
}

static char* split(char* str, char c, uint len)
{
	char* s = str;
	while (len-- && *str != c && *str != 0)
		str++;

	if (*str == c && str != s && len != 0)
	{
		*str = 0;
		return ++str;
	}
	return 0;
}

/*
 * Save settings to file.
 */
void settings_save()
{
	char home_dir[255];
	char config_file[255];
	struct stat st = {0};
	struct passwd *pw = getpwuid(getuid());

	sprintf(home_dir, "%s/.lock-keys", pw->pw_dir);
	sprintf(config_file, "%s/lock-keys-rc", home_dir);

	if (stat(home_dir, &st) == -1)
		mkdir(home_dir, 0600);

	FILE* f = fopen(config_file, "w");
	if (f)
	{
		fprintf(f, "overlay=%i\n", lk_settings.overlay);
		fprintf(f, "opacity=%.2f\n", lk_settings.opacity);
		fprintf(f, "timeout=%i\n", lk_settings.timeout);
		fclose(f);
	}

#if LK_USE_KDE || LK_USE_KDE4

#if LK_USE_KDE4
	sprintf(config_file, "%s/.kde4/Autostart/lock-keys", pw->pw_dir);
#else
	sprintf(config_file, "%s/.kde4/Autostart/lock-keys", pw->pw_dir);
#endif

	if (lk_settings.autostart)
	{
		if (symlink(LK_EXEC_PATH, config_file))
			lk_settings.autostart = lk_settings.autostart;
	}
	else
	{
		if (stat(config_file, &st) != -1)
			remove(config_file);
	}
#endif

}

/*
 * Parse the config file and load settings.
 */
void settings_load()
{
	#define MAX_LINE_LEN 30
	#define MAX_PATH_LEN 255

	char line[MAX_LINE_LEN];
	char home_dir[MAX_PATH_LEN];
	char config_file[MAX_PATH_LEN];
	struct passwd *pw = getpwuid(getuid());

	/*
	 * since the max path length can be really long or
	 * unbounded we just allocate a reasonable buffer and
	 * if the home dir path exceeds it we throw an error. The
	 * 27 if for the path of kde bellow.
	 */
	if (strlen(pw->pw_dir) > (MAX_PATH_LEN - 27))
	{
		printf("error: home directory path too long!");
		printf("error: could not load settings!");
		return;
	}

	sprintf(home_dir, "%s/.lock-keys", pw->pw_dir);
	sprintf(config_file, "%s/lock-keys-rc", home_dir);

	FILE* f = fopen(config_file, "r");
	if (f)
	{
		gboolean eof = FALSE;
		while (!eof)
		{
			char* val;
			eof = readline(f, line, MAX_LINE_LEN);
			val = split(line, '=', MAX_LINE_LEN);

			if (val)
			{
				if (!strcmp(line, "opacity"))
				{
					lk_settings.opacity = atof(val);
				}
				else if (!strcmp(line, "overlay"))
				{
					lk_settings.overlay = atoi(val);
				}
				else if (!strcmp(line, "timeout"))
				{
					lk_settings.timeout = atoi(val);
				}
			}
		}
		fclose(f);
	}

#if LK_USE_KDE || LK_USE_KDE4
	{
		struct stat st = {0};

#if LK_USE_KDE4
		sprintf(config_file, "%s/.kde4/Autostart/lock-keys", pw->pw_dir);
#else
		sprintf(config_file, "%s/.kde/Autostart/lock-keys", pw->pw_dir);
#endif

		if (stat(config_file, &st) != -1)
		{
			lk_settings.autostart = TRUE;
		}
	}
#endif

	#undef MAX_LINE_LEN
	#undef MAX_PATH_LEN

}
