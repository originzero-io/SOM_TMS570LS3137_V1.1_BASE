/*
 * core.h
 *
 *  Created on: Nov 17, 2023
 *      Author: fatih
 */

#ifndef INC_CORE_H_
#define INC_CORE_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum processor_mode_enum
{
    PROCESSOR_MODE_USER = 0X10,
    PROCESSOR_MODE_FIQ = 0X11,
    PROCESSOR_MODE_IRQ = 0X12,
    PROCESSOR_MODE_SUPERVISOR = 0X13,
    PROCESSOR_MODE_MONITOR = 0X16,
    PROCESSOR_MODE_ABORT = 0X17,
    PROCESSOR_MODE_HYP = 0X1A,
    PROCESSOR_MODE_UNDEFINED = 0X1B,
    PROCESSOR_MODE_SYSTEM = 0X1F,
}processor_mode_enum_t;


// Define the CPSR register structure
typedef union {
    struct {
        uint8_t N :1;       // Negative flag
        uint8_t Z :1;       // Zero flag
        uint8_t C :1;       // Carry flag
        uint8_t V :1;       // Overflow flag
        uint8_t Q :1;       // Saturation flag
        uint8_t IT_1_0 :2;  // IT state bits
        uint8_t J :1;       // Java State Bit
        uint8_t Reserved :4;// Reserved bits
        uint8_t GE :4;      // Greater than or Equal flags
        uint8_t IT_7_2 :6;  // IT state bits
        uint8_t E :1;       // Endianness
        uint8_t A :1;       // Asynchronous abort
        uint8_t I :1;       // IRQ disable
        uint8_t F :1;       // FIQ disable
        uint8_t T :1;       // Thumb state
        uint8_t M :5;       // Mode field
    } bits;
    uint32_t value;        // Entire register value
} cpsr_register_union_t;


processor_mode_enum_t get_processor_mode(void);
bool is_processor_in_isr(void);



#endif /* INC_CORE_H_ */
