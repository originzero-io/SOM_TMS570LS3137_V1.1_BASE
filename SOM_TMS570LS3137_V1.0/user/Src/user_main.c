/*
 * user_main.c
 *
 *  Created on: Oct 21, 2023
 *      Author: fatih
 */
#include "user_main.h"

uint32_t led_blink_period;

void setup(void)
{
    led_blink_period = 200;
}


void loop(void)
{
    led_toggle(LED1);
    led_toggle(LED2);
    delay_ms(led_blink_period);
}

