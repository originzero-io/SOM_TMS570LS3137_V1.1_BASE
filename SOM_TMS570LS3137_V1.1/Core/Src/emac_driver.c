/*
 * emac_driver.c
 *
 *  Created on: Oct 22, 2023
 *      Author: fatih
 */

#include "emac_driver.h"
#include <ethernetif.h>
#include "sys_vim.h"
#include "lwiplib.h"
#include "emac.h"
#include "core.h"

volatile uint32_t EMACCore0TxIsr_counter = 0, EMACCore0TxIsr_counter2 = 0;
volatile uint32_t EMACCore0RxIsr_counter[4];

extern struct netif gnetif;

#pragma CODE_STATE(emac_tx_int_isr, 32)
#pragma INTERRUPT(emac_tx_int_isr, IRQ)
void emac_tx_int_isr(void)
{


    EMACCore0TxIsr_counter++;

    EMACTxIntHandler(&hdkif_data[0U]);
    EMACCoreIntAck(hdkif_data[0].emac_base, (uint32) EMAC_INT_CORE0_TX);
    EMACCoreIntAck(hdkif_data[0U].emac_base, (uint32) EMAC_INT_CORE0_RX);


    ethernetif_tx_callback();


    EMACCore0TxIsr_counter2++;
}

#pragma CODE_STATE(emac_rx_int_isr, 32)
#pragma INTERRUPT(emac_rx_int_isr, IRQ)
void emac_rx_int_isr(void)
{

    EMACCore0RxIsr_counter [0]++;

    ethernetif_rx_callback();
    EMACCore0RxIsr_counter [1]++;

    //hdkif_rx_inthandler(&gnetif);

    EMACReceive(&hdkif_data[0U]);
    EMACCore0RxIsr_counter [2]++;
    EMACCoreIntAck(hdkif_data[0U].emac_base, (uint32) EMAC_INT_CORE0_RX);
    EMACCoreIntAck(hdkif_data[0].emac_base, (uint32) EMAC_INT_CORE0_TX);
    EMACCore0RxIsr_counter [3]++;



}

