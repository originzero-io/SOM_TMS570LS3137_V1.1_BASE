/*
 * core.c
 *
 *  Created on: Nov 17, 2023
 *      Author: fatih
 */

#include "core.h"

processor_mode_enum_t get_processor_mode(void)
{
    cpsr_register_union_t cpsr;
    cpsr.value = _get_CPSR();

    if (cpsr.bits.T)
    {
        return PROCESSOR_MODE_THREAD;
    }
    else
    {
        return PROCESSOR_MODE_HANDLER;
    }
}
