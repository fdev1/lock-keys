#include "settings.h"
#include "overlay.h"

static GtkWidget* settings_window = NULL;

/*
 * settings struct
 */
LK_SETTINGS lk_settings = {
	/* opacity */ 0.8,
	/* overlay */ TRUE,
	/* timeout */ 2,
	/* autostart */ TRUE
};

static void opacity_changed(
	GtkSpinButton *spinbutton,
	GtkWidget* opacity)
{
	overlay_opacity_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(opacity)));
	overlay_show();
}

static void timeout_changed(
	GtkSpinButton *spinbutton,
	GtkWidget* timeout)
{
	overlay_timeout_set(gtk_spin_button_get_value(GTK_SPIN_BUTTON(timeout)));
	overlay_show();
}

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
	}

	gtk_widget_destroy(GTK_WIDGET(settings_window));
	settings_window = NULL;

}
