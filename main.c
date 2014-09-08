#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <glib.h>
#include <Xlib.h>
#include <extensions/XKB.h>
#include <pthread.h>

GtkWidget* label;
GtkWidget* window;
const count_down = 5;

static gboolean autohide_window(gpointer data)
{
	static char visible = 1;
	static char caps_lock = -1;
	static char count = 0;
	char caps;
	
	if (visible)
	{		
		if (++count == count_down)
		{
			gtk_widget_hide((GtkWidget*) data);
		}
	}
	else
	{
		char* txt = "";
		gchar* markup;
		Display* display;
		display = XOpenDisplay(NULL);
		
		if (display) 
		{
			unsigned n;
			XkbGetIndicatorState(display, XkbUseCoreKbd, &n);
			if ((caps = n & 0x01))
			{
				txt = "A";
			}
			else
			{
				txt = "a";
			}
			XCloseDisplay(display);

			if (caps_lock == -1)
			{
				caps_lock = caps;
			}
			else if (caps_lock != caps)
			{
				count = 0;
				caps_lock = caps;
				markup = g_markup_printf_escaped("<span size=\"100000\">%s</span>", txt);
				gtk_label_set_markup((GtkLabel*)label, markup);
				g_free(markup);
				gtk_widget_show((GtkWidget*) data);
			}
		}
	}
	
	visible ^= 1;
	
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
    gtk_widget_set_opacity((GtkWidget*)window, 0.5);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
 
    label = gtk_label_new("A");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_widget_show(label);
	

}

/*
static GdkFilterReturn root_window_filter(GdkXEvent* xevent, GdkEvent* event, gpointer data)
{
	
	g_print("%s", "Event Received");
	
	return GDK_FILTER_CONTINUE;
}
*/

int main( int   argc, char *argv[] )
{

	gtk_init (&argc, &argv);

	create_window();

	/*
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
	*/

    g_timeout_add(200, autohide_window, window);

    gtk_main();

    return 0;
}