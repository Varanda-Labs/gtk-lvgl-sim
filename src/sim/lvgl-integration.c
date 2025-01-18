
#include "lvgl.h"
#include "lvgl-integration.h"
#include "logger.h"
#include <time.h>

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t lvgl_buf[LCD_WIDTH * LCD_HEIGHT / 10 * BYTES_PER_PIXEL];

cairo_surface_t *lv_int_surface = NULL;
GtkWidget *drawing_area = NULL;

extern void app_init();

lv_display_t * lvgl_display;
static lv_indev_t * indev = NULL;

static void input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    uint16_t x,y;
    uint8_t z;
    static bool touch = false;

    // lcd_get_raw_touch(&x, &y, &z);

    // if (z > TOUCH_PRESSURE) {
    //     uint16_t cal_x, cal_y;        
    //     get_calibrated_touch(x, y, &cal_x, &cal_y);
    //     if (! touch) {
    //         touch = true;
    //         LOG("touch x=%d, y=%d, z=%d", cal_x,cal_y,z);
    //     }
    //     data->point.x = cal_x;
    //     data->point.y = cal_y;
    //     data->state = LV_INDEV_STATE_PRESSED;
    // }
    // else {
    //     if (touch) {
    //         touch = false;
    //         LOG("touch release");
    //     }
    //     data->state = LV_INDEV_STATE_RELEASED;
    // }
}

static uint64_t get_timestamp_ms()
{
    struct timespec tp;
    if (clock_gettime(CLOCK_MONOTONIC, &tp)) {
        LOG_E("get_timestamp_ms: clock_gettime error");
        return 0;
    }
    //        time_t   tv_sec;        /* seconds */
    //        long     tv_nsec;       /* nanoseconds */

    uint64_t t = tp.tv_sec * 1000; // sec to milli
    t += tp.tv_nsec / 1000000;        // nano to milli

    return t;
}

static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    static int t = 0;
    if ((++t % 100) == 0) {
        LOG("flush called\n");
    }

//    lv_display_flush_ready(display);
#if 0
    int x,y;
    lcd_q_item_t item;
    uint16_t * p = (uint16_t *) LCD_GetFramebuffer();
    uint16_t * color_p = (uint16_t *) px_map; //pixel_output = 0;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            *(p + (x + y * LV_HOR_RES_MAX)) = *color_p;
            color_p++;
        }
    }

    LOG("flush: x1=%d, y1=%d   x2=%d, y2=%d", 
            area->x1, area->y1, area->x2, area->y2);

    item.x1 = area->x1;
    item.y1 = area->y1;
    item.x2 = area->x2;
    item.y2 = area->y2;

    if( mos_queue_put(lcd_q, &item, MOS_WAIT_FOREVER) != MOS_PASS) {
        LOG_W("flush: lcd_q full");
    }

    lv_display_flush_ready(display);
#endif


    #define BRUSH_SIZE 10
    //CHECK_SURFACE();
    cairo_t *cr = cairo_create (lv_int_surface);

    int x, y, ri, gi, bi;
    double r,g,b;
    uint16_t * color_p = (uint16_t *) px_map; //pixel_output = 0;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //*(p + (x + y * LV_HOR_RES_MAX)) = *color_p;
            // r = ((*color_p) >> 11) / 0b00011111;  // rrrrrGGGGGGbbbbb
            // g = (((*color_p) >> 5) & 0b00111111) / 0b00111111;
            // b = ((*color_p) & 0b00011111) / 0b00011111;

            ri = ((*color_p) >> 11);
            r = ri / ((double) 0b00011111);  // rrrrrGGGGGGbbbbb
            gi = ((*color_p) >> 5);
            gi &= 0b00111111;
            g = gi / ((double) 0b00111111);
            bi = ((*color_p) & 0b00011111);
            b = bi / ((double) 0b00011111);
            cairo_set_source_rgba (cr, r, g, b, 1); // color
            cairo_rectangle (cr, x, y, BRUSH_SIZE, BRUSH_SIZE);
            //cairo_set_source_rgba (cr, r, g, 0, 1); // color
            cairo_fill (cr);
            color_p++;
        }
    }

    // cairo_rectangle (cr, x, y, BRUSH_SIZE, BRUSH_SIZE);
    // cairo_fill (cr);


    //gtk_widget_queue_draw_area ((GtkWidget *) gtk_widget_get_window (lv_int_surface), 0, 0, LCD_WIDTH, LCD_HEIGHT);
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
            app_init();
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
