#pragma once

#include <gtk/gtk.h>

#define TOUCH_PRESSURE 10

#define CHECK_SURFACE() do { if (! lv_int_surface) {LOG("invalid lv_int_surface");  return;} } while(0)
#define CHECK_SURFACE_RET() do { if (! lv_int_surface) {LOG("invalid lv_int_surface");  return FALSE;} } while(0)

extern cairo_surface_t *lv_int_surface;
extern GtkWidget *drawing_area;

#define LVGL_PERIOD_TIME    20
void lv_int_init();
void lv_int_run_slice();
void lv_int_set_pointer(int x, int y, int z);
