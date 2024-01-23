/*
 * core.c
 *
 *  Created on: Nov 17, 2023
 *      Author: fatih
 */

#include "core.h"
#include "sys_core.h"

processor_mode_enum_t get_processor_mode(void)
{
    cpsr_register_union_t cpsr;
    cpsr.value = _getCPSRValue_();

    return (processor_mode_enum_t) (cpsr.bits.M);
}

bool is_processor_in_isr(void)
{
    processor_mode_enum_t mode = get_processor_mode();
    return mode == PROCESSOR_MODE_IRQ || mode == PROCESSOR_MODE_FIQ;
}
