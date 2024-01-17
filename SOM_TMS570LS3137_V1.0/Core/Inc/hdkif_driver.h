/*
 * hdkif_driver.h
 *
 *  Created on: Oct 22, 2023
 *      Author: fatih
 */

#ifndef HDKIF_DRIVER_H_
#define HDKIF_DRIVER_H_

#include "results.h"
#include "lwip/netif.h"

#define LINK_SPEED (10000000u) /* Link speed in bits per second */

results_enum_t hdkif_init(struct netif *netif);
void hdkif_driver_set_mac_addr(uint8_t inst_num, const uint8_t *mac_addr);
void hdkif_driver_rx_inthandler(struct netif *netif);
void  hdkif_driver_tx_inthandler(struct netif *netif);

#endif /* HDKIF_DRIVER_H_ */
