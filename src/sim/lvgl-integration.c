
#include "lvgl.h"
#include "lvgl-integration.h"
#include "logger.h"
#include <time.h>

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t lvgl_buf[LCD_WIDTH * LCD_HEIGHT / 10 * BYTES_PER_PIXEL];

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

static void input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    uint16_t x,y;
    uint8_t z;
    static bool touch = false;

    x = pointer_x;
    y = pointer_y;
    z = pointer_z & 0x0000ff;

    // lcd_get_raw_touch(&x, &y, &z);

    if (z > TOUCH_PRESSURE) {
        if (! touch) {
            touch = true;
            LOG("touch x=%d, y=%d, z=%d", x,y,z);
        }
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        if (touch) {
            touch = false;
            LOG("touch release");
        }
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static uint64_t get_timestamp_ms()
{
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp)) {
        LOG_E("get_timestamp_ms: clock_gettime error");
        return 0;
    }
    uint64_t t = tp.tv_sec * 1000; // sec to milli
    t += tp.tv_nsec / 1000000;        // nano to milli
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
    cairo_t *cr = cairo_create (lv_int_surface);

    int x, y, ri, gi, bi;
    double r,g,b;
    uint16_t * color_p = (uint16_t *) px_map;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            // *color_p format (565): rrrrrGGGGGGbbbbb

            r = ((*color_p) >> 11) / (double) 0b00011111;
            g = (((*color_p) >> 5) & 0b00111111) / (double) 0b00111111;
            b = ((*color_p) & 0b00011111) / (double) 0b00011111;

            cairo_set_source_rgba (cr, r, g, b, 1);
            cairo_rectangle (cr, x, y, PIXEL_SIZE, PIXEL_SIZE);
            cairo_fill (cr);
            color_p++;
        }
    }
    gtk_widget_queue_draw_area (drawing_area, 0, 0, LCD_WIDTH, LCD_HEIGHT);

    cairo_destroy (cr);
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

    lv_tick_set_cb((lv_tick_get_cb_t) get_timestamp_ms);
    lvgl_display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);

    lv_display_set_buffers( lvgl_display, 
                            lvgl_buf, 
                            NULL, 
                            sizeof(lvgl_buf),
                            LV_DISPLAY_RENDER_MODE_PARTIAL);
                            
    lv_display_set_flush_cb(lvgl_display, flush_cb);

    indev = lv_indev_create();        /* Create input device connected to Default Display. */
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);   /* Touch pad is a pointer-like device. */
    lv_indev_set_read_cb(indev, input_read_cb);    /* Set driver function. */
    // app_init();

}
