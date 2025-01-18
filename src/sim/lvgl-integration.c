
#include "lvgl.h"
#include "lvgl-integration.h"
#include "logger.h"

#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
static uint8_t lvgl_buf[LCD_WIDTH * LCD_HEIGHT / 10 * BYTES_PER_PIXEL];

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
    uint64_t t = 0;

    return t;
}

static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    static int t = 0;
    if (++t % 100 == 0) {
        LOG("flush called\n");
    }

    lv_display_flush_ready(display);
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
}

void lv_int_run_slice()
{
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

}
