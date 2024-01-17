/*
 * emac_driver.c
 *
 *  Created on: Oct 22, 2023
 *      Author: fatih
 */

#include "sys_vim.h"
#include "hdkif_driver.h"
#include "lwiplib.h"

#pragma INTERRUPT(EMACCore0TxIsr, IRQ)
void EMACCore0TxIsr(void)
{
    hdkif_driver_tx_inthandler(lwip_get_hdkNetIF_p(0));
}

#pragma INTERRUPT(EMACCore0RxIsr, IRQ)
void EMACCore0RxIsr(void)
{
    hdkif_driver_rx_inthandler(lwip_get_hdkNetIF_p(0));
}
