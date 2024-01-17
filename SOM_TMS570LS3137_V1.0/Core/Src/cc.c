/*
 * cc.c
 *
 *  Created on: Oct 24, 2023
 *      Author: fatih
 */

#include "arch/cc.h"
#include <stdlib.h>



unsigned int lwip_port_rand(void)
{
    return (unsigned int)rand(); // Or any other platform-specific random number generation function
}


