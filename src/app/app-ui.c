
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
#include "logger.h"

#define GPIO_APP   // comment this line to compile a very simple example (single button)

#ifdef GPIO_APP
// ******************************
// *
// *       GPIO Test App
// *
// ******************************
#include "app-ui.h"

#define COL_SIZE 70
#define ROW_SIZE 40
#define LED_BRIGHTNESS_ON   255
#define LED_BRIGHTNESS_OFF  50

#define ANALOG_BAR_TEXT_FORMAT "      %d%%"

static lv_obj_t * GPIO_in[4];
static lv_obj_t * analog_input_bar;
static lv_obj_t * bar_label;
static lv_obj_t * GPIO_out[4];

static void switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    int idx = lv_event_get_user_data(e);
    int state = lv_obj_has_state(obj, LV_STATE_CHECKED);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LOG("idx: %d, State: %s\n", idx, state ? "On" : "Off");
    }
    set_gpio(idx, state);
}

void ui_set_led(int led_index, int on_1_off_0)
{
    if (on_1_off_0) {
        lv_led_set_brightness(GPIO_in[led_index], LED_BRIGHTNESS_ON);
    }
    else {
        lv_led_set_brightness(GPIO_in[led_index], LED_BRIGHTNESS_OFF);
    }    
}

void ui_set_bar(int level)
{
    char txt[32];
    snprintf(txt, sizeof(txt), ANALOG_BAR_TEXT_FORMAT, level);
    lv_label_set_text(bar_label, txt);
        lv_bar_set_value(analog_input_bar, level, LV_ANIM_OFF);
}

static void create_widgets(void)
{
    int i;
    char txt[32];
    static int32_t col_dsc[] = {    COL_SIZE,   // 5 coluns
                                    COL_SIZE,
                                    COL_SIZE,
                                    COL_SIZE,
                                    COL_SIZE,
                                    LV_GRID_TEMPLATE_LAST};

    static int32_t row_dsc[] = {    ROW_SIZE,   // 6 rows
                                    ROW_SIZE,
                                    ROW_SIZE,
                                    ROW_SIZE,
                                    ROW_SIZE,
                                    ROW_SIZE,
                                    LV_GRID_TEMPLATE_LAST};


    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_size(cont, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_center(cont);

    lv_obj_t * label;
    lv_obj_t * obj;


    label = lv_label_create(cont);
    lv_label_set_text(label, "GPIO Test Screen");
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, // col
                         LV_GRID_ALIGN_START, 0, 5);     // row

    for (i=0; i<4; i++) {
        // label "Output n"
        label = lv_label_create(cont);
        snprintf(txt, sizeof(txt), "Output %d", i + 1);
        lv_label_set_text(label, txt);
        lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 1,
                            LV_GRID_ALIGN_START, 1 + i, 1);

        // switch
        GPIO_out[i] = lv_switch_create(cont);
        lv_obj_set_grid_cell(GPIO_out[i], LV_GRID_ALIGN_END, 1, 1,
                            LV_GRID_ALIGN_START, 1 + i, 1);
        lv_obj_add_event_cb(GPIO_out[i], switch_event_handler, LV_EVENT_VALUE_CHANGED, (void *) i);

        // label "input 1"
        label = lv_label_create(cont);
        snprintf(txt, sizeof(txt), "Input %d", i + 1);
        lv_label_set_text(label, txt);
        lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 3, 1,
                            LV_GRID_ALIGN_START, 1 + i, 1);

        // LED
        GPIO_in[i] = lv_led_create(cont);
        lv_obj_set_grid_cell(GPIO_in[i], LV_GRID_ALIGN_END, 4, 1,
                            LV_GRID_ALIGN_START, 1 + i, 1);
        lv_led_set_color(GPIO_in[i], lv_palette_main(LV_PALETTE_RED));
        lv_led_set_brightness(GPIO_in[i], LED_BRIGHTNESS_OFF);

    }

    // label Analog Input
    label = lv_label_create(cont);
    lv_label_set_text(label, " Analog Input:");
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 1,
                        LV_GRID_ALIGN_START, 5, 1);

    // Bar
    analog_input_bar = lv_bar_create(cont);
    lv_obj_set_size(analog_input_bar, 240, 20);
    lv_obj_center(analog_input_bar);
    lv_bar_set_value(analog_input_bar, 0, LV_ANIM_OFF);
    lv_obj_set_grid_cell(analog_input_bar, LV_GRID_ALIGN_START, 2, 1,
                            LV_GRID_ALIGN_START, 5, 1);

    // bar value
    snprintf(txt, sizeof(txt), ANALOG_BAR_TEXT_FORMAT, 0);
    bar_label = lv_label_create(cont);
    lv_label_set_text(bar_label, txt);
    lv_obj_set_size(bar_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(bar_label, LV_GRID_ALIGN_START, 1, 1, // col
                         LV_GRID_ALIGN_START, 5, 1);     // row

}


void app_init() // mandatory function.
{
    create_widgets();
}

#else
// ******************************
// *
// *   Very simple App (1 button)
// *
// ******************************
#define LABEL_FMT "Button %d"
static int cnt = 0;
static lv_obj_t * label;
static void btn_event_cb(lv_event_t * e)
{
    char txt[32];
    snprintf(txt, sizeof(txt), LABEL_FMT, ++cnt);
    lv_label_set_text(label, txt);
}

void ui_set_led(int led_index, int on_1_off_0){} // stub
void ui_set_bar(int level){}

void app_init() // mandatory function.
{
    lv_obj_clean(lv_screen_active()); 
    lv_obj_t * btn = lv_button_create(lv_screen_active());  // Add a button the current screen
    lv_obj_set_pos(btn, 10, 10);                            // Set its position
    lv_obj_set_size(btn, 120, 50);                          // Set its size
    lv_obj_add_event_cb(btn, btn_event_cb, 
                        LV_EVENT_CLICKED, NULL);            // Assign a callback to the button

    label = lv_label_create(btn);                           // Add a label to the button
    lv_obj_center(label);
    btn_event_cb(NULL);

}
#endif