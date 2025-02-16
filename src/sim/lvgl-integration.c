
/*
 *************************************
 *
 *  GTK LVGL Simulator
 *
 *  Copyright (c) 2025 Varanda Labs
 *  MIT License
 *
 *************************************
 */

#include "lvgl.h"
#include "lvgl-integration.h"
#include "logger.h"
#include <time.h>

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define LVGL_BUF_SIZE (LCD_WIDTH * LCD_HEIGHT * BYTES_PER_PIXEL)

static uint8_t * lvgl_buf = NULL;
static uint8_t * pixel_buf = NULL;

cairo_surface_t *lv_int_surface = NULL;
GtkWidget *drawing_area = NULL;

extern void lv_demo_music();

extern void app_init();

lv_display_t * lvgl_display;
static lv_indev_t * indev = NULL;

static int pointer_x = 0;
static int pointer_y = 0;
static int pointer_z = 0;

void lv_int_set_pointer(int x, int y, int z)
{
    pointer_x = x;
    pointer_y = y;
    pointer_z = z;
}

void lv_int_pointer_left()
{
    pointer_z = 0;
}

static void input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    uint16_t x,y;
    uint8_t z;
    static bool touch = false;

    x = pointer_x;
    if (x >= LCD_WIDTH) x = LCD_WIDTH - 1;
    if (x < 0) x = 0;

    y = pointer_y;
    if (y >= LCD_HEIGHT) y = LCD_HEIGHT - 1;
    if (y < 0) y = 0;

    z = pointer_z & 0x0000ff;


    if (z > TOUCH_PRESSURE) {
        if (! touch) {
            touch = true;
            LOG("touch x=%d, y=%d, z=%d\n", x,y,z);
        }
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        if (touch) {
            touch = false;
            LOG("touch release\n");
        }
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static uint64_t get_timestamp_ms()
{
    struct timespec tp;
    static uint64_t t_ini = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &tp)) {
        LOG_E("get_timestamp_ms: clock_gettime error\n");
        return 0;
    }
    uint64_t t = tp.tv_sec * 1000; // sec to milli
    t += tp.tv_nsec / 1000000;        // nano to milli
    if (t_ini == 0) {
        t_ini = t;
        t = 0;
    }
    else t -= t_ini;
    return t;
}


static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    #define PIXEL_SIZE 1
    CHECK_SURFACE();
    if (!drawing_area) { 
        lv_display_flush_ready(display);
        return;
    }
    if (lvgl_buf == NULL ||  pixel_buf == NULL) {
        lv_display_flush_ready(display);
        return;
    }

    uint16_t * color_p = (uint16_t *) px_map;
    int32_t y;
    int32_t x;
    int32_t p;
    for(y = area->y1; y <= area->y2; y++) {
        p = (y * LCD_WIDTH + area->x1) * 3;
        for(x = area->x1; x <= area->x2; x++) {
            // *color_p format (565): rrrrrGGGGGGbbbbb
            pixel_buf[p] = (*color_p >> 8) & 0b11111000;
            pixel_buf[p + 1] = (*color_p >> 3) & 0b11111100;
            pixel_buf[p + 2] = (*color_p << 3) & 0b11111000;

            p += 3;
            color_p++;
        }
    }

    if (drawing_area && lv_int_surface) {

        GdkPixbuf * pixbuf = gdk_pixbuf_new_from_data(  (guchar*)pixel_buf, 
                                            GDK_COLORSPACE_RGB, 
                                            false, 
                                            8, 
                                            LCD_WIDTH, 
                                            LCD_HEIGHT, 
                                            LCD_WIDTH * 3, 
                                            NULL, 
                                            NULL);
        if (!pixbuf) {
            LOG_E("Could not create pixbuf for display\n");
            return;
        }

        cairo_t *cr = cairo_create (lv_int_surface);
        gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
        cairo_paint(cr);
        cairo_destroy (cr);
        g_object_unref(pixbuf);
    }
    gtk_widget_queue_draw_area (drawing_area, 0, 0, LCD_WIDTH, LCD_HEIGHT);

    lv_display_flush_ready(display);

}

void lv_int_run_slice()
{
    static int cnt = 0;
    static bool launched = false; 
    // as we resize the surface we need to postpone the app launch as it is destroyed and recreated:
    if (! launched) {
        if (cnt++ >= 10) {
            launched = true;
#ifdef MUSIC_DEMO
            lv_demo_music();
#else
            app_init();
#endif
        }
    }
    lv_tick_inc(LVGL_PERIOD_TIME);
    lv_timer_handler();
}

void lv_int_init()
{
    lv_init();

    lvgl_buf = (uint8_t *) malloc (LVGL_BUF_SIZE);
    pixel_buf = (uint8_t *) malloc (LCD_WIDTH * LCD_HEIGHT * 3);

    if (lvgl_buf == NULL ||  pixel_buf == NULL) {
        LOG_E("No memo for LCD buffers\n");
    }

    lvgl_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

    lv_display_set_buffers( lvgl_display, 
                            lvgl_buf, 
                            NULL, 
                            LVGL_BUF_SIZE,
                            LV_DISPLAY_RENDER_MODE_FULL);
                            
    lv_display_set_flush_cb(lvgl_display, flush_cb);

    indev = lv_indev_create();        /* Create input device connected to Default Display. */
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   /* Touch pad is a pointer-like device. */
    lv_indev_set_read_cb(indev, input_read_cb);    /* Set driver function. */
}
