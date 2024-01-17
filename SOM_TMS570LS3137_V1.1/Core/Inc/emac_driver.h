/*
 * emac_driver.h
 *
 *  Created on: Oct 22, 2023
 *      Author: fatih
 */

#ifndef EMAC_DRIVER_H_
#define EMAC_DRIVER_H_

#include "emac.h"

extern hdkif_t hdkif_data[MAX_EMAC_INSTANCE];
void hdkif_transmit(struct hdkif *hdkif, struct pbuf *pbuf);

#endif /* EMAC_DRIVER_H_ */
