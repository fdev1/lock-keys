#include <glib.h>

void overlay_show();
void overlay_state_set(gboolean enabled);
void overlay_caps_lock_set(gboolean caps_lock);
void overlay_opacity_set(gdouble opacity);
void overlay_timeout_set(guint seconds);
