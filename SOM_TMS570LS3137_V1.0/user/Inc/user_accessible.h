/*
 * user_accessible.h
 *
 *  Created on: Oct 21, 2023
 *      Author: fatih
 */

#ifndef INC_USER_ACCESSIBLE_H_
#define INC_USER_ACCESSIBLE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum leds_enum
{
    LED1 = 22,
    LED2 = 25,
    LED3 = 5
}leds_enum_t;


void led_set(leds_enum_t led, bool state);
void led_toggle(leds_enum_t led);
bool led_get(leds_enum_t led);


void delay_ms(uint32_t time);


#endif /* INC_USER_ACCESSIBLE_H_ */
