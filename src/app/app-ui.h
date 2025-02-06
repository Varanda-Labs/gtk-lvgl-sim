#pragma once

#include "lvgl.h"

extern uint32_t input_change_event;         // use: lv_obj_send_event(widget, input_change_event, &val_ptr)
extern lv_obj_t * GPIO_in[4];
extern lv_obj_t * analog_input_bar;