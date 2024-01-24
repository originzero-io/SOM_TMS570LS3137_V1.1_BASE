/**
 *  \file lwiplib.c
 *
 *  \brief lwip related initializations
 */
#include <ethernetif.h>
#include "lwiplib.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/autoip.h"
#include "netif/ethernet.h"
#include "emac_driver.h"
#include "LwIP/prot/dhcp.h"

/******************************************************************************
 **                       INTERNAL VARIABLE DEFINITIONS
 ******************************************************************************/
/*
 ** The lwIP network interface structure for the HDK Ethernet MAC.
 */
struct netif gnetif;
static uint32_t timer_counter;
/******************************************************************************
 **                          FUNCTION DEFINITIONS
 ******************************************************************************/

/**
 *
 * \brief Initializes the lwIP TCP/IP stack.
 *
 * \param instNum  The instance index of the EMAC module
 * \param macArray Pointer to the MAC Address
 * \param ipAddr   The IP address to be used 
 * \param netMask  The network mask to be used 
 * \param gwAddr   The Gateway address to be used 
 * \param ipMode   The IP Address Mode.
 *        ipMode can take the following values\n
 *             IPADDR_USE_STATIC - force static IP addressing to be used \n
 *             IPADDR_USE_DHCP - force DHCP with fallback to Link Local \n
 *             IPADDR_USE_AUTOIP - force  Link Local only
 *
 * This function performs initialization of the lwIP TCP/IP stack for the
 * HDK EMAC, including DHCP and/or AutoIP, as configured.
 *
 * \return IP Address.
 */
results_enum_t lwip_init_async(uint8_t instance_number, uint8_t *mac_address, uint8_t *ip_address_array, uint8_t *net_mask_array, uint8_t *gateway_address_array, uint8_t ip_mode, bool is_reconncet)
{
    results_enum_t return_value = RESULT_IN_THE_PROCESS;

    static uint8_t state = 0;

    if (true == is_reconncet && 0 == state)
    {
        state = 5;
    }

    static ip4_addr_t ip_address = { 0 };
    static ip4_addr_t net_mask = { 0 };
    static ip4_addr_t gateway_address = { 0 };

    static uint8_t dhcp_state = 0;
    static uint8_t tried_count = 0;

    switch (state)
    {
    case 0:
    {
        lwip_init();

        state++;
    }
        break;
    case 1:
    {
        if (ip_mode == IPADDR_USE_STATIC)
        {
            IP4_ADDR(&ip_address, ip_address_array[0], ip_address_array[1], ip_address_array[2], ip_address_array[3]);
            IP4_ADDR(&net_mask, net_mask_array[0], net_mask_array[1], net_mask_array[2], net_mask_array[3]);
            IP4_ADDR(&gateway_address, gateway_address_array[0], gateway_address_array[1], gateway_address_array[2], gateway_address_array[3]);
        }

        state++;
    }
        break;
    case 2:
    {
        ethernetif_set_mac_address(mac_address);

        state++;
    }
        break;
    case 3:
    {
        if (NULL == netif_add(&gnetif, &ip_address, &net_mask, &gateway_address, &instance_number, ethernetif_init, ethernet_input))
        {
            return_value = RESULT_ERR_NETIF;
        }
        else
        {
            netif_set_default(&gnetif);
            netif_set_up(&gnetif);
            netif_set_link_up(&gnetif);
        }

        state++;
    }
        break;
    case 4:
    {
        netif_set_default(&gnetif);

        if (IPADDR_USE_DHCP == ip_mode)
        {
#if LWIP_DHCP
            state += 1;
#endif
        }
        else if (IPADDR_USE_AUTOIP == ip_mode)
        {
#if LWIP_AUTOIP
            state += 2;
#endif
        }
        else
        {
            state += 3;
        }
    }
        break;
    case 5: /* LWIP_DHCP */
    {

        if (0 == dhcp_state)
        {
            err_t err = dhcp_start(&gnetif);

            if (ERR_OK != err)
            {
                return_value = RESULT_ERR_HEAP_MEM;
            }
            return RESULT_SUCCESS;
            timer_counter = 0;

            dhcp_state = 1;
        }
        else if (1 == dhcp_state)
        {
            osDelay(1);
            if (dhcp_supplied_address(&gnetif))
            {
                state = 7;
            }
            else if (2 < timer_counter)
            {
                if (tried_count < 5)
                {
                    dhcp_state = 0;
                }
                else
                {
                    return_value = RESULT_ERR_TIMEOUT;
                }

                tried_count++;
            }
        }

    }
        break;
    case 6: /* LWIP_AUTOIP */
    {
#if LWIP_AUTOIP
         autoip_start(&gnetif);
 #endif
    }
        break;
    case 7: /* END */
    {
        if (IPADDR_USE_STATIC == ip_mode || IPADDR_USE_AUTOIP == ip_mode)
        {
            /* Bring the interface up */
            netif_set_up(&gnetif);
        }

        return_value = RESULT_SUCCESS;
    }
        break;
    default:
        break;
    }

    if (RESULT_IN_THE_PROCESS != return_value)
    {
        state = 0;
        dhcp_state = 0;
        tried_count = 0;
    }

    return return_value;
}

struct netif* lwip_get_hdkNetIF_p(void)
{
    return &gnetif;
}

/***************************** End Of File ***********************************/

