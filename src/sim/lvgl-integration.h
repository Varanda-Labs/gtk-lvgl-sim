#pragma once

#include <gtk/gtk.h>

extern cairo_surface_t *lv_int_surface;
extern GtkWidget *drawing_area;

#define LVGL_PERIOD_TIME    20
void lv_int_init();
void lv_int_run_slice();
