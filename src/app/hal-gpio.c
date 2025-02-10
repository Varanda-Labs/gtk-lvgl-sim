
/*
 *************************************
 *
 *  GTK LVGL Simulator
 *
 *  Copyright (c) 2025 Varanda Labs
 *  MIT License
 *
 *************************************

   Hardware Abstraction Layer for GPIO's

 */

#include "hal-gpio.h"

#ifdef IS_SIMULATOR
// ******************************
// *
// *      Simulator code
// *
// ******************************
extern void set_led(int index, int state);             // LED in simulator to show GPIO output states
extern void ui_set_led(int led_index, int on_1_off_0); // LED in LVGL screen to show GPIO inputs states
extern void ui_set_bar(int level);

void set_gpio(int index, int level)
{
    set_led( index,  level);
}

#else
// ******************************
// *
// *   Real HAL implementation
// *
// ******************************
void set_gpio(int index, int level)
{
    LOG("Implement me!!!\n");
}
#endif

void handle_gpio_input_state(int index, int level)
{
    ui_set_led( index,  level);
}

void handle_analog_input(int level)
{
    ui_set_bar(level);
}