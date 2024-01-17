/*
 * lwip.h
 *
 *  Created on: Nov 2, 2021
 *      Author: fatih
 */

#ifndef LWIP_H
#define LWIP_H

#include <stdint.h>

uint8_t lwipInit(void);


void IntMasterIRQEnable(void);
void IntMasterIRQDisable(void);

unsigned int IntMasterStatusGet(void);

#endif /* LWIP_H */
