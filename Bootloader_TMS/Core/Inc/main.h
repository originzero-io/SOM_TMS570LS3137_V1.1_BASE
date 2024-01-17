/*
 * main.h
 *
 *  Created on: Jul 20, 2022
 *      Author: M.Fatih YAZMAN
 */

#ifndef CORE_INC_MAIN_H_
#define CORE_INC_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "communication_config.h"
#include "results.h"
#include <stdint.h>
#include <stdbool.h>
#include "macro.h"


void user_main(uint8_t value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CORE_INC_MAIN_H_ */
