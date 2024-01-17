/*
 * user_accessible.c
 *
 *  Created on: Oct 21, 2023
 *      Author: fatih
 */

#include "user_accessible.h"
#include "gio.h"
#include "het.h"
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"

void led_set(leds_enum_t led, bool state)
{
    gioSetBit(hetPORT1, (uint32_t) led, (uint32_t) state);
}
void led_toggle(leds_enum_t led)
{
    gioToggleBit(hetPORT1, (uint32_t) led);
}
bool led_get(leds_enum_t led)
{
    return (bool) gioGetBit(hetPORT1, (uint32_t) led);
}

void delay_ms(uint32_t time)
{
    vTaskDelay(time);
}
