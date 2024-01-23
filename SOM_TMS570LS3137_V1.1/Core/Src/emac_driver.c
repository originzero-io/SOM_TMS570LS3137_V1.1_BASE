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


volatile uint32_t emacTxNotification_counter = 0, emacRxNotification_counter = 0;

void emacTxNotification(hdkif_t *hdkif)
{
    emacTxNotification_counter++;
    ethernetif_tx_callback(hdkif);
}

void emacRxNotification(hdkif_t *hdkif)
{
    emacRxNotification_counter++;
    ethernetif_rx_callback(hdkif);
}

