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

#pragma once

void set_gpio(int index, int level);                    // to set GPIO output state

// The following are called from IRQ or from another periodic reading
// For simulator called from GPIO dialog
void handle_gpio_input_state(int index, int level);     // to process GPIO input state change
void handle_analog_input(int level);                    // to process Analog input value change

