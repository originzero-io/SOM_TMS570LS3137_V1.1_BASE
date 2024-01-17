/** @file sys_main.c 
 *   @brief Application main file
 *   @date 15.July.2009
 *   @version 1.01.000
 *
 *   This file contains the initialization & control path for the LwIP & EMAC driver
 *   and can be called from system main.
 */

/* (c) Texas Instruments 2011, All rights reserved. */

#include "lwip.h"

#if defined(_TMS570LC43x_) || defined(_RM57Lx_)
#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_emac.h"
#include "HL_mdio.h"
#include "HL_phy_dp83640.h"
#include "HL_sci.h"
#else
#include "sys_common.h"
#include "system.h"
#include "emac.h"
#include "mdio.h"
#include "phy_dp83640.h"
#include "sci.h"
#endif

#include "lwipopts.h"
#include "lwiplib.h"
#include "lwip\inet.h"

#include "lwip/ip_addr.h"

#define TMS570_MDIO_BASE_ADDR 	0xFCF78900u /* Same base address for TMS570 & RM48 devices */
#define TMS570_EMAC_BASE_ADDR	0xFCF78000u /* Same base address for TMS570 & RM48 devices */
#define DPS83640_PHYID			0x20005CE1u	/** PHY ID Register 1 & 2 values for DPS83640 (Same for TMS570 & RM devices */
#define PHY_ADDR				1			/** EVM/Hardware dependent & is same for TMS570 & RM48 HDKs */

void iommUnlock(void);
void iommLock(void);
void iommMuxEnableMdio(void);
void iommMuxEnableRmii(void);
void iommMuxEnableMii(void);
void IntMasterIRQEnable(void);

/** @fn void main(void)
 *   @brief Application main function
 *   @note This function is empty by default.
 *
 *   This function is called after startup.
 *   The user can use this function to implement the application.
 */

/* USER CODE BEGIN (2) */

/*
 ** Interrupt Handler for Core 0 Receive interrupt
 */
volatile int countEMACCore0RxIsr = 0;
#pragma INTERRUPT(EMACCore0RxIsr, IRQ)
void EMACCore0RxIsr(void)
{
    countEMACCore0RxIsr++;
    lwIPRxIntHandler(0);
}

/*
 ** Interrupt Handler for Core 0 Transmit interrupt
 */
volatile int countEMACCore0TxIsr = 0;
#pragma INTERRUPT(EMACCore0TxIsr, IRQ)
void EMACCore0TxIsr(void)
{
    countEMACCore0TxIsr++;
    lwIPTxIntHandler(0);
}

void IntMasterIRQEnable(void)
{
    _enable_IRQ();
    return;
}

void IntMasterIRQDisable(void)
{
    _disable_IRQ();
    return;
}

unsigned int IntMasterStatusGet(void)
{
    return (0xC0 & _get_CPSR());
}
