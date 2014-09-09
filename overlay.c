#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "overlay.h"

static guint timer_id = 0;
static GtkWidget* overlay_label = NULL;
static GtkWidget* overlay_window = NULL;
static guint timeout_secs = 2;

static gboolean autohide_window(gpointer data)
{
	gtk_widget_hide(GTK_WIDGET(overlay_window));
	timer_id = 0;
	return FALSE;
}

static void create_window()
{
	gint width, height;
    gint screen_width, screen_height;
    GdkScreen* screen;

    width = 150;
    height = 150;

    overlay_window = gtk_window_new (GTK_WINDOW_POPUP);
    screen = gtk_window_get_screen((GtkWindow*)overlay_window);
    screen_width = gdk_screen_get_width(screen);
    screen_height = gdk_screen_get_height(screen);

    gtk_window_set_keep_above((GtkWindow*)overlay_window, TRUE);
    gtk_window_resize((GtkWindow*)overlay_window, width, height);
    gtk_window_move((GtkWindow*)overlay_window, (screen_width / 2) - (width / 2), (screen_height) - (height) - 100);
    gtk_container_set_border_width (GTK_CONTAINER(overlay_window), 10);

    overlay_label = gtk_label_new("A");
    gtk_container_add(GTK_CONTAINER(overlay_window), overlay_label);
    gtk_widget_show(overlay_label);

}

void overlay_state_set(gboolean enabled)
{
	if (enabled)
	{
		if (overlay_window == NULL)
			create_window();
	}
	else
	{
		gtk_widget_destroy(overlay_label);
		gtk_widget_destroy(overlay_window);
		overlay_label = NULL;
		overlay_window = NULL;
	}
}

void overlay_caps_lock_set(gboolean caps_lock)
{
	/*
	gchar* markup = g_markup_printf_escaped(
		"<span size=\"100000\">%s</span>", (caps_lock ? "A" : "a"));

	gtk_label_set_markup(GTK_LABEL(label), markup);
	g_free(markup);
	*/
	if (overlay_window != NULL)
	{
		if (caps_lock)
		{
			gtk_label_set_markup(
				GTK_LABEL(overlay_label), "<span size=\"100000\">A</span>");
		}
		else
		{
			gtk_label_set_markup(
				GTK_LABEL(overlay_label), "<span size=\"100000\">a</span>");
		}
	}
}

void overlay_show()
{

	if (overlay_window ==  NULL)
		return;

	gtk_widget_show(GTK_WIDGET(overlay_window));

	if (timer_id)
		g_source_remove(timer_id);

    timer_id = g_timeout_add_seconds(timeout_secs, autohide_window, NULL);


}

void overlay_opacity_set(gdouble opacity)
{
	if (overlay_window != NULL)
		gtk_widget_set_opacity(GTK_WIDGET(overlay_window), opacity);
}

void overlay_timeout_set(guint seconds)
{
	timeout_secs = seconds;
}
