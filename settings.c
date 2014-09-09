#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "settings.h"
#include "overlay.h"


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

	GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(settings_window));
	gtk_container_set_border_width(GTK_CONTAINER(content_area), 5);

	GtkWidget* grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(content_area), grid);
	gtk_widget_show(GTK_WIDGET(grid));

	GtkWidget* check_autostart = gtk_check_button_new_with_label("Autostart at login.");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_autostart), lk_settings.autostart);
	gtk_grid_attach(GTK_GRID(grid), check_autostart, 0, 0, 2, 1);
	gtk_widget_show(GTK_WIDGET(check_autostart));

	GtkWidget* check_overlay = gtk_check_button_new_with_label("Show on-screen overlay");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_overlay), lk_settings.overlay);
	gtk_grid_attach(GTK_GRID(grid), check_overlay, 0, 1, 2, 1);
	gtk_widget_show(GTK_WIDGET(check_overlay));

	GtkWidget* label = gtk_label_new("Opacity:");
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(label), 0, 2, 1, 1);
	gtk_widget_show(GTK_WIDGET(label));
	GtkAdjustment* opacity_adjustment = gtk_adjustment_new(lk_settings.opacity, 0.10, 1.0, 0.01, 0.1, 0.0);
	GtkWidget* opacity = gtk_spin_button_new(opacity_adjustment, 0.01, 2);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(opacity), 0.10, 1.0);
	gtk_grid_attach(GTK_GRID(grid), opacity, 1, 2, 1, 1);
	gtk_widget_show(GTK_WIDGET(opacity));
	g_signal_connect(opacity, "value-changed", G_CALLBACK(opacity_changed), opacity);


	GtkWidget* lbl_timeout = gtk_label_new("Overlay Duration:");
	gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(lbl_timeout), 0, 3, 1, 1);
	gtk_widget_show(GTK_WIDGET(lbl_timeout));
	GtkAdjustment* timeout_adjustment = gtk_adjustment_new((gdouble)lk_settings.timeout, 1.0, 10.0, 1.0, 1.0, 0.0);
	GtkWidget* timeout = gtk_spin_button_new(timeout_adjustment, 1.0, 0.0);
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(timeout), 1, 10);
	gtk_grid_attach(GTK_GRID(grid), timeout, 1, 3, 1, 1);
	gtk_widget_show(GTK_WIDGET(timeout));
	g_signal_connect(timeout, "value-changed", G_CALLBACK(timeout_changed), timeout);


	if (gtk_dialog_run(GTK_DIALOG(settings_window)) == GTK_RESPONSE_ACCEPT)
	{
		lk_settings.autostart = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_autostart));
		lk_settings.overlay = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_overlay));
		lk_settings.opacity = gtk_spin_button_get_value(GTK_SPIN_BUTTON(opacity));
		lk_settings.timeout = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(timeout));
		settings_save();
	}

	gtk_widget_destroy(GTK_WIDGET(settings_window));
	settings_window = NULL;

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
		mkdir(home_dir, 0700);

	FILE* f = fopen(config_file, "w");
	if (f)
	{
		fprintf(f, "overlay=%i\n", lk_settings.overlay);
		fprintf(f, "opacity=%.2f\n", lk_settings.opacity);
		fprintf(f, "timeout=%i\n", lk_settings.timeout);
		fclose(f);
	}

}

static gboolean readline(FILE* f, char* buffer, uint bufflen)
{
	uint i;

	for (i = 0; i < bufflen; i++)
	{
		char c = fgetc(f);

		if (c == EOF)
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
//	len++;
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
 * Parse the config file and load settings.
 */
void settings_load()
{
	char line[30];
	char home_dir[255];
	char config_file[255];
	struct passwd *pw = getpwuid(getuid());

	sprintf(home_dir, "%s/.lock-keys", pw->pw_dir);
	sprintf(config_file, "%s/lock-keys-rc", home_dir);

	FILE* f = fopen(config_file, "r");
	if (f)
	{
		gboolean eof = FALSE;
		while (!eof)
		{
			char* val;
			eof = readline(f, line, 30);
			val = split(line, '=', 30);

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
}
