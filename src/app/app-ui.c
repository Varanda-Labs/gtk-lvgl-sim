
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
//#include "lv_slider.h"

//#define SIMPLE_APP

#ifndef SIMPLE_APP
static void switch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("State: %s\n", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off");
    }
}

static void create_switches(void)
{
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_ROW_WRAP); //LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lv_scr_act(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * sw;

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_state(sw, LV_STATE_DISABLED);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);

    sw = lv_switch_create(lv_scr_act());
    lv_obj_add_state(sw, LV_STATE_CHECKED | LV_STATE_DISABLED);
    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, NULL);
}

#define COL_SIZE 70
#define ROW_SIZE 40

static lv_obj_t * GPIO_in[4];
static lv_obj_t * GPIO_out[4];

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
    }

    // label Analog Input
    label = lv_label_create(cont);
    lv_label_set_text(label, " Analog Input:");
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_CENTER, 0, 1,
                        LV_GRID_ALIGN_START, 5, 1);

    // Bar
    lv_obj_t * bar1 = lv_bar_create(cont);
    lv_obj_set_size(bar1, 240, 20);
    lv_obj_center(bar1);
    lv_bar_set_value(bar1, 50, LV_ANIM_OFF);
    lv_obj_set_grid_cell(bar1, LV_GRID_ALIGN_START, 2, 1,
                            LV_GRID_ALIGN_START, 5, 1);

    // bar value
    label = lv_label_create(cont);
    lv_label_set_text(label, "      100%");
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 1, 1, // col
                         LV_GRID_ALIGN_START, 5, 1);     // row
}
#endif

void app_init() // mandatory function. Called by the simulator
{
#ifdef SIMPLE_APP
    lv_obj_clean(lv_screen_active()); 
    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    //lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);
#else
    //create_switches();
    create_widgets();
#endif
}