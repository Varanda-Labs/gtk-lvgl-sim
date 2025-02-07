
#include "hal-gpio.h"

#ifdef IS_SIMULATOR
// ******************************
// *
// *      Simulator code
// *
// ******************************
extern void set_led(int index, int state);

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