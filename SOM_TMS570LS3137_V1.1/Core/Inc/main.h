/*
 * main.h
 *
 *  Created on: Oct 21, 2023
 *      Author: fatih
 */

#ifndef CORE_INC_MAIN_H_
#define CORE_INC_MAIN_H_

#include "core.h"


/* MQTT SETTINGS BEGIN */
#define MQTT_CYCLIC_TIMER_INTERVAL 5

/* MQTT SETTINGS END*/


void user_main(void);

int log_printf(const char *__restrict _format, ...);
#endif /* CORE_INC_MAIN_H_ */
