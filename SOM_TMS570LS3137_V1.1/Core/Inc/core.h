/*
 * core.h
 *
 *  Created on: Nov 17, 2023
 *      Author: fatih
 */

#ifndef INC_CORE_H_
#define INC_CORE_H_

#include <stdint.h>

typedef enum processor_mode_enum
{
    PROCESSOR_MODE_HANDLER,
    PROCESSOR_MODE_THREAD
}processor_mode_enum_t;

// Define the CPSR register structure
typedef union
{
    struct
    {
        uint8_t N :1; /* Stores bit 31 of the result of the instruction. In other words stores the sign of the number */
        uint8_t Z :1; /* Is set to 1 if the result of the operation is zero else stays 0 */
        uint8_t C :1; /* Stores the value of the carry bit if it occurred in an addition or the borrow bit in a subtraction. In a shift stores the last bit shifted out. */
        uint8_t V :1; /* Set to 1 if an overflow occurred */
        uint8_t Q :1; /* Indicates whether an overflow or a saturation occurred in the enhanced DSP instructions */
        uint8_t IT_1_0 :2; /* IT state bits. */
        uint8_t J :1; /* Java State Bit. */
        uint8_t Reserved :4; /* Reserved. */
        uint8_t GE :4; /* Greater than or equal bits */
        uint8_t IT_7_2 :6; /* IT state bits */
        uint8_t E :1; /* If set, data memory is interpreted as big-endian. If cleared data memory is interpreted as little-endian. */
        uint8_t A :1; /* If set, any asynchronous abort is held pending until this bit is cleared. */
        uint8_t I :1; /* If set, IRQ is disabled. If cleared IRQ is allowed */
        uint8_t F :1; /* If set, FIQ is disabled. If cleared FIQ is allowed */
        uint8_t T :1; /* If set ARM is in Thumb mode */
        uint8_t M :5; /* Mode of ARM */

    } bits;
    uint32_t value;
} cpsr_register_union_t;

processor_mode_enum_t get_processor_mode(void);



#endif /* INC_CORE_H_ */
