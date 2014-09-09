#include <gtk/gtk.h>
#include <gdk/gdk.h>

typedef struct
{
	gdouble opacity;
	gboolean overlay;
	gint timeout;
	gboolean autostart;
}
LK_SETTINGS;

extern LK_SETTINGS lk_settings;
void settings_dialog_show();
