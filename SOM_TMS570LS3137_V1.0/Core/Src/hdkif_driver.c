/*
 * hdkif_driver.c
 *
 *  Created on: Oct 22, 2023
 *      Author: fatih
 */

#include "hdkif_driver.h"

#include "emac.h"
#include "mdio.h"
#include "phy_dp83640.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#include "netif/etharp.h"
#include "lwip/snmp.h"
#include "lwip/sys.h"

#include <stdint.h>
#include <string.h>

/* Constants for configuration */
#define DELAY_MAX                0xfffU
#define PHY_ID_READ_MAX          0xFFFFU
#define CHANNEL_MAX              8U
#define MAX_DESCRIPTOR_COUNT     ((SIZE_EMAC_CTRL_RAM >> 1) / sizeof(emac_tx_bdp))

/* Define those to better describe the network interface. */
#define IFNAME0                  'e'
#define IFNAME1                  'n'

/* EMAC TX Buffer descriptor data structure */
struct emac_tx_bdp
{
    volatile struct emac_tx_bdp *next;
    volatile uint32 bufptr;
    volatile uint32 bufoff_len;
    volatile uint32 flags_pktlen;

    /* helper to know which pbuf this tx bd corresponds to */
    volatile struct pbuf *pbuf;
} emac_tx_bdp;

/* EMAC RX Buffer descriptor data structure */
struct emac_rx_bdp
{
    volatile struct emac_rx_bdp *next;
    volatile uint32 bufptr;
    volatile uint32 bufoff_len;
    volatile uint32 flags_pktlen;

    /* helper to know which pbuf this rx bd corresponds to */
    volatile struct pbuf *pbuf;
} emac_rx_bdp;

/**
 * Helper struct to hold the data used to operate on a particular
 * receive channel
 */
struct rxch
{
    volatile struct emac_rx_bdp *free_head;
    volatile struct emac_rx_bdp *active_head;
    volatile struct emac_rx_bdp *active_tail;
    uint32 freed_pbuf_len;
} rxch;

/**
 * Helper struct to hold the data used to operate on a particular
 * transmit channel
 */
struct txch
{
    volatile struct emac_tx_bdp *free_head;
    volatile struct emac_tx_bdp *active_tail;
    volatile struct emac_tx_bdp *next_bd_to_process;
} txch;

/**
 * Helper struct to hold private data used to operate the ethernet interface.
 */
struct hdkif
{
    /* emac instance number */
    uint32 inst_num;

    uint8 mac_addr[6];

    /* emac base address */
    uint32 emac_base;

    /* emac controller base address */
    volatile uint32 emac_ctrl_base;
    volatile uint32 emac_ctrl_ram;

    /* mdio base address */
    volatile uint32 mdio_base;

    /* phy parameters for this instance - for future use */
    uint32 phy_addr;
    boolean (*phy_autoneg)(uint32, uint32, uint16);
    boolean (*phy_partnerability)(uint32, uint32, uint16*);

    /* The tx/rx channels for the interface */
    struct txch txch;
    struct rxch rxch;
} hdkif;

static struct hdkif hdkif_data[MAX_EMAC_INSTANCE];

static void hdkif_driver_rx_inthandler_process_packet(struct pbuf *q,
                                                      struct netif *netif);
static void hdkif_driver_rx_inthandler_update_descriptor(
        volatile struct emac_rx_bdp *curr_bd, struct pbuf *q, struct rxch *rxch);
static void hdkif_driver_tx_inthandler_free_transmitted_packets(
        volatile struct emac_tx_bdp *curr_bd, struct txch *txch,
        struct hdkif *hdkif);
static results_enum_t hdkif_driver_inst_config(struct hdkif *hdkif);
static results_enum_t hdkif_driver_hw_init(struct netif *netif);
static void hdkif_driver_set_mac_address(struct netif *netif,
                                         const struct hdkif *hdkif);
static results_enum_t hdkif_driver_initialize_phy(struct hdkif *hdkif);
static results_enum_t hdkif_driver_link_setup(struct hdkif *hdkif);
static results_enum_t hdkif_driver_link_setup(struct hdkif *hdkif);
static void hdkif_driver_initialize_tx_descriptors(struct hdkif *hdkif);
static results_enum_t hdkif_driver_initialize_rx_descriptors(
        struct hdkif *hdkif);

static err_t hdkif_driver_netif_output(struct netif *netif, struct pbuf *p,
                                       const ip4_addr_t *ipaddr);
static err_t hdkif_driver_netif_linkoutput(struct netif *netif, struct pbuf *p);

static results_enum_t hdkif_driver_transmit(struct hdkif *hdkif,
                                            struct pbuf *pbuf);
static uint32 hdkif_driver_swizzle_data(uint32 word);
static struct emac_tx_bdp* hdkif_driver_swizzle_txp(
        volatile struct emac_tx_bdp *p);
static struct emac_rx_bdp* hdkif_driver_swizzle_rxp(
        volatile struct emac_rx_bdp *p);

/**
 * @brief Initializes the HDK network interface.
 *
 * This function performs the necessary initialization for the HDK network interface,
 * setting up the link output, output function pointers, and SNMP variables, as well
 * as configuring the hardware through a call to `hdkif_hw_init`. The function assumes
 * that the netif structure is already allocated and that the instance number is stored
 * in the `state` field of the `netif` structure.
 *
 * @note This function should be called during the system initialization phase or when
 * the network interface needs to be re-initialized. Ensure that the netif structure is
 * properly allocated and the instance number is correctly set in the `state` field.
 *
 * @param netif Pointer to the netif structure associated with the HDK interface. This
 * structure contains information about the network interface and is used throughout the
 * initialization process. It must not be NULL.
 *
 * @return RESULT_SUCCESS if the initialization completes successfully.
 *         Appropriate error code from the results_enum_t enumeration otherwise, indicating
 *         the nature of the error (e.g., RESULT_ERR_INIT if there is a hardware initialization
 *         failure, RESULT_ERR_NULL if the provided netif pointer is NULL, etc.).
 */
err_t hdkif_init(struct netif *netif)
{
    results_enum_t result = RESULT_SUCCESS;


    /* Validate pointers to avoid dereferencing NULL */
    if (netif == NULL)
    {
        /* Handle error, set an appropriate error code */
        result = RESULT_ERR_NULL;
    }
    else
    {

        uint8_t inst_num;
        (void) memcpy(&inst_num, &(netif->state), sizeof(inst_num));

        struct hdkif *hdkif = &hdkif_data[inst_num];
        netif->state = hdkif;

#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        netif->hostname = "OriginZero";
#endif /* LWIP_NETIF_HOSTNAME */

        NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED);

        hdkif->inst_num = inst_num;

        /* Assumed that IFNAME0 and IFNAME1 are 'char' type */
        netif->name[0] = IFNAME0;
        netif->name[1] = IFNAME1;

        netif->num = (uint8_t) inst_num; /* Casting for safety */

        netif->output = hdkif_driver_netif_output;
        netif->linkoutput = hdkif_driver_netif_linkoutput;

        /* Initialize the hardware */
        if (hdkif_driver_inst_config(hdkif) != RESULT_SUCCESS)
        {
            /* Handle hardware initialization failure */
            result = RESULT_ERR_INIT;
        }

        if (hdkif_driver_hw_init(netif) != RESULT_SUCCESS)
        {
            /* Handle hardware initialization failure */
            result = RESULT_ERR_INIT;
        }
    }

    if (RESULT_IS_ERROR(result))
    {
        return ERR_CONN;
    }
    else
    {
        return ERR_OK;
    }
}

/**
 * @brief Sets the MAC address for a specific interface instance.
 *
 * This function assigns a MAC address to an Ethernet interface specified by its instance
 * number. It reverses the byte order of the provided MAC address before setting it.
 *
 * @note The function assumes the MAC address array is of length 'ETHARP_HWADDR_LEN'.
 *
 * @param inst_num Instance number of the Ethernet interface. It should be a valid index
 *                 within the 'hdkif_data' array bounds.
 * @param mac_addr Pointer to an array containing the MAC address (6 bytes). This array
 *                 must be at least 'ETHARP_HWADDR_LEN' bytes long. The function does not
 *                 check for NULL pointers; the caller must ensure the pointer is valid.
 *
 * @return None.
 */
void hdkif_driver_set_mac_addr(uint8_t inst_num, const uint8_t *mac_addr)
{
    /* Local variables declaration */
    uint32_t temp;
    struct hdkif *hdkif;

    if ((inst_num < MAX_EMAC_INSTANCE) && (mac_addr != NULL)) /* Validate inputs as per MISRA 22.1 and 18.1 */
    {
        hdkif = &hdkif_data[inst_num];

        /* Set MAC hardware address with byte order reversal */
        for (temp = 0U; temp < (uint32_t) ETHARP_HWADDR_LEN; ++temp)
        {
            hdkif->mac_addr[temp] = mac_addr[(uint32_t) ETHARP_HWADDR_LEN - 1U
                    - temp];
        }
    }
    /* Could add else branch to handle error, if required */
}

/* Forward declarations of static functions */
static void hdkif_driver_rx_inthandler_process_packet(struct pbuf *q,
                                                      struct netif *netif);
static void hdkif_driver_rx_inthandler_update_descriptor(
        volatile struct emac_rx_bdp *curr_bd, struct pbuf *q, struct rxch *rxch);

/**
 * @brief Handler for Receive interrupt. Packet processing is done in this
 * interrupt handler itself.
 *
 * @param netif The lwip network interface structure for this ethernetif.
 * @return None.
 */
void hdkif_driver_rx_inthandler(struct netif *netif)
{
    struct hdkif *hdkif;
    struct rxch *rxch;
    volatile struct emac_rx_bdp *curr_bd;
    volatile struct pbuf *q;

    if (netif == NULL)
    {
        /* Handle error or add error logging */
        return;
    }

    hdkif = netif->state;
    rxch = &(hdkif->rxch);
    curr_bd = rxch->active_head;

    /* Process the descriptors as long as data is available */
    while ((hdkif_driver_swizzle_data(curr_bd->flags_pktlen) & EMAC_BUF_DESC_SOP)
            != 0U)
    {
        /* Start processing once the packet is loaded */
        if ((hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                & EMAC_BUF_DESC_OWNER) == 0U)
        {
            /* Get the total length of the packet. curr_bd points to the start of the packet. */
            uint16_t tot_len = hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                    & 0xFFFFU;

            /* Get the start of the pbuf queue */
            q = curr_bd->pbuf;

            /* Process packet and handle descriptors */
            hdkif_driver_rx_inthandler_process_packet(q, netif);
            hdkif_driver_rx_inthandler_update_descriptor(curr_bd, q, rxch);

            /* Acknowledge that this packet is processed */
            EMACRxCPWrite(hdkif->emac_base, 0, (uintptr_t) curr_bd); // Cast to uintptr_t for MISRA compliance

            rxch->active_head = curr_bd;
        }
    }

    /* Acknowledge the processed interrupts */
    EMACCoreIntAck(hdkif->emac_base, EMAC_INT_CORE0_RX);
    EMACCoreIntAck(hdkif->emac_base, EMAC_INT_CORE0_TX);
}

/**
 * @brief Process the packet and update statistics.
 *
 * @param q The pbuf pointer pointing to the packet buffer.
 * @param netif The lwip network interface structure for this ethernetif.
 * @return None.
 */
static void hdkif_driver_rx_inthandler_process_packet(struct pbuf *q,
                                                      struct netif *netif)
{
    /* Adjust the link statistics */
    LINK_STATS_INC(link.recv);

    /* Process the packet */
    if (ethernet_input(q, netif) != ERR_OK)
    {
        /* Adjust the link statistics in case of error */
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }
}

/**
 * @brief Update the descriptor with the new packet buffer information.
 *
 * @param curr_bd The current buffer descriptor being processed.
 * @param q The new packet buffer.
 * @param rxch The receive channel information.
 * @return None.
 */
static void hdkif_driver_rx_inthandler_update_descriptor(
        volatile struct emac_rx_bdp *curr_bd, struct pbuf *q, struct rxch *rxch)
{
    struct pbuf *new_pbuf;
    volatile struct emac_rx_bdp *last_bd = rxch->active_tail; // Preserve the last buffer descriptor

    /* Allocate a new pbuf */
    new_pbuf = pbuf_alloc(PBUF_RAW, (u16_t) (rxch->freed_pbuf_len), PBUF_POOL);
    if (new_pbuf == NULL)
    {
        return;
    }

    /* Write the descriptors with the pbuf info till either of them expires */
    for (; (q != NULL) && (curr_bd != rxch->active_head); q = q->next)
    {
        curr_bd->bufptr = hdkif_driver_swizzle_data((uint32_t) q->payload);
        curr_bd->bufoff_len = hdkif_driver_swizzle_data((uint16_t) q->len);
        curr_bd->flags_pktlen = hdkif_driver_swizzle_data(EMAC_BUF_DESC_OWNER);
        rxch->freed_pbuf_len -= q->len;

        /* Save the pbuf */
        curr_bd->pbuf = q;
        last_bd = curr_bd;
        curr_bd = hdkif_driver_swizzle_rxp(curr_bd->next);
    }

    /* If there are no enough rx bds to allocate all pbufs in the chain, free the rest of the pbuf */
    if (q != NULL)
    {
        pbuf_free(q);
    }

    /* Update the receive channel's active tail to the last buffer descriptor processed */
    rxch->active_tail = last_bd;
}

/**
 * Handler for EMAC Transmit interrupt
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return none
 */
void hdkif_driver_tx_inthandler(struct netif *netif)
{
    struct txch *txch;
    struct hdkif *hdkif;
    volatile struct emac_tx_bdp *curr_bd, *next_bd_to_process;

    if (netif == NULL)
    {
        /* Handle error or add error logging */
        return;
    }

    hdkif = netif->state;
    txch = &(hdkif->txch);
    next_bd_to_process = txch->next_bd_to_process;
    curr_bd = next_bd_to_process;

    /* Processing the buffer descriptors linked list */
    while (hdkif_driver_swizzle_data(curr_bd->flags_pktlen) & EMAC_BUF_DESC_SOP)
    {
        /* Wait for the EMAC to release the buffer descriptor */
        while ((hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                & EMAC_BUF_DESC_OWNER) == EMAC_BUF_DESC_OWNER)
            ;

        /* Find the end of the packet */
        while ((hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                & EMAC_BUF_DESC_EOP) != EMAC_BUF_DESC_EOP)
        {
            curr_bd = hdkif_driver_swizzle_txp(curr_bd->next);
        }

        /* Clear the SOP and EOP flags */
        next_bd_to_process->flags_pktlen &= hdkif_driver_swizzle_data(
                ~(EMAC_BUF_DESC_SOP));
        curr_bd->flags_pktlen &= hdkif_driver_swizzle_data(
                ~(EMAC_BUF_DESC_EOP));

        /* If no more data is transmitted, prepare for the next interrupt */
        if (curr_bd->next == NULL)
        {
            txch->next_bd_to_process = txch->free_head;
        }
        else
        {
            txch->next_bd_to_process = hdkif_driver_swizzle_txp(curr_bd->next);
        }

        /* Free the buffers of the transmitted packets */
        hdkif_driver_tx_inthandler_free_transmitted_packets(curr_bd, txch,
                                                            hdkif);

        next_bd_to_process = txch->next_bd_to_process;
        curr_bd = next_bd_to_process;
    }

    /* Acknowledge the processed interrupts */
    EMACCoreIntAck(hdkif->emac_base, EMAC_INT_CORE0_RX);
    EMACCoreIntAck(hdkif->emac_base, EMAC_INT_CORE0_TX);
}

/**
 * Free the transmitted packets and update the statistics.
 *
 * @param curr_bd The current buffer descriptor being processed.
 * @param txch The transmit channel information.
 * @param hdkif The hardware kit interface information.
 * @return None.
 */
static void hdkif_driver_tx_inthandler_free_transmitted_packets(
        volatile struct emac_tx_bdp *curr_bd, struct txch *txch,
        struct hdkif *hdkif)
{
    /* Acknowledge the EMAC that packet has been sent */
    EMACTxCPWrite(hdkif->emac_base, 0, (uint32_t) curr_bd); // Cast to uint32_t for consistency

    /* Free the packet buffer */
    pbuf_free((struct pbuf*) curr_bd->pbuf);

    /* Increment the link transmit statistics */
    LINK_STATS_INC(link.xmit);
}

/**
 * @brief Sets up the instance parameters for the specified network interface.
 *
 * This function configures the base addresses and other parameters for a specific
 * instance of the network interface. The configuration is based on the instance
 * number specified in the hdkif structure.
 *
 * @param hdkif Pointer to the network interface structure to configure.
 *              This parameter must not be NULL.
 *
 * @return Returns RESULT_OK if the configuration is successful.
 *         Returns RESULT_ERR_NULL if the hdkif parameter is NULL.
 *         Returns RESULT_ERR_BAD_DATA if the instance number is not supported.
 */
static results_enum_t hdkif_driver_inst_config(struct hdkif *hdkif)
{
    if (NULL == hdkif) // Changed from NULL_PTR to NULL for standard C compatibility
    {
        return RESULT_ERR_NULL;
    }

    // Consider using a switch case if there are more instances
    switch (hdkif->inst_num)
    {
    case 0:
        hdkif->emac_base = EMAC_0_BASE;
        hdkif->emac_ctrl_base = EMAC_CTRL_0_BASE;
        hdkif->emac_ctrl_ram = EMAC_CTRL_RAM_0_BASE;
        hdkif->mdio_base = MDIO_0_BASE;
        hdkif->phy_addr = EMAC_PHYADDRESS;
        hdkif->phy_autoneg = Dp83640AutoNegotiate; // Assuming this is a function pointer or macro
        hdkif->phy_partnerability = Dp83640PartnerAbilityGet; // Assuming this is a function pointer or macro
        return RESULT_SUCCESS;
        // case 1: // Uncomment and set values for other instances if applicable
        //     // Set appropriate values
        //     return RESULT_OK;
    default:
        return RESULT_ERR_INST_NUM; // Unsupported instance number
    }

    // Unreachable code
    // return RESULT_OK;
}

/**
 * @brief Initializes the hardware for the network interface.
 *
 * This function is designed to prepare the network interface hardware for operation. This includes setting
 * the MAC address, initializing the PHY, setting up descriptors, and configuring interrupts.
 *
 * @param netif The lwIP network interface structure for this ethernetif.
 * @return ERR_OK if successful, ERR_MEM if there was a memory allocation error, or another err_t error code if there was an error.
 */
static results_enum_t hdkif_driver_hw_init(struct netif *netif)
{
    struct hdkif *hdkif;
    results_enum_t result;

    if (netif == NULL)
    {
        return RESULT_ERR_NULL;
    }

    hdkif = netif->state;

    /* Set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* Set MAC hardware address */
    hdkif_driver_set_mac_address(netif, hdkif);

    /* Maximum transfer unit */
    netif->mtu = MAX_TRANSFER_UNIT;

    /* Device capabilities */
    netif->flags =
    NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    EMACInit(hdkif->emac_ctrl_base, hdkif->emac_base);

    MDIOInit(hdkif->mdio_base, MDIO_FREQ_INPUT, MDIO_FREQ_OUTPUT);

    for (volatile uint32_t delay = DELAY_MAX; delay > 0U; --delay)
    {
        /* Intentionally empty loop body: simple delay loop */
    }

    EMACRxBroadCastEnable(hdkif->emac_base, 0);

    /* Set the MAC Addresses in EMAC hardware */
    EMACMACSrcAddrSet(hdkif->emac_base, hdkif->mac_addr);

    for (uint32_t channel = 0U; channel < CHANNEL_MAX; ++channel)
    {
        EMACMACAddrSet(hdkif->emac_base, channel, hdkif->mac_addr,
        EMAC_MACADDR_MATCH);
    }

    result = hdkif_driver_initialize_phy(hdkif);
    if (result != RESULT_SUCCESS)
    {
        return result;
    }

    /* Initialize the TX descriptors */
    hdkif_driver_initialize_tx_descriptors(hdkif);

    /* Initialize the RX descriptors and check for errors */
    result = hdkif_driver_initialize_rx_descriptors(hdkif);
    if (result != RESULT_SUCCESS)
    {
        return result;
    }

    /* Remaining initializations */
    EMACRxUnicastSet(hdkif->emac_base, 0);
    EMACNumFreeBufSet(hdkif->emac_base, 0, 10);
    EMACTxEnable(hdkif->emac_base);
    EMACRxEnable(hdkif->emac_base);

    /* Write the RX HDP for channel 0 */
    EMACRxHdrDescPtrWrite(hdkif->emac_base, (uint32_t) hdkif->rxch.active_head,
                          0);

    EMACMIIEnable(hdkif->emac_base);

    /* Enable transmission and reception; enable interrupts for channel 0 and control core 0 */
    EMACTxIntPulseEnable(hdkif->emac_base, hdkif->emac_ctrl_base, 0, 0);
    EMACRxIntPulseEnable(hdkif->emac_base, hdkif->emac_ctrl_base, 0, 0);

    return result;
}

/**
 * @brief Sets the MAC address for the network interface.
 *
 * This function programs the hardware with the correct MAC address from the netif structure.
 *
 * @param netif Pointer to the lwIP network interface structure for this ethernetif.
 * @param hdkif Pointer to the hdkif device structure.
 */
static void hdkif_driver_set_mac_address(struct netif *netif,
                                         const struct hdkif *hdkif)
{
    /* set MAC hardware address */
    for (uint32_t temp = 0U; temp < ETHARP_HWADDR_LEN; ++temp)
    {
        netif->hwaddr[temp] = hdkif->mac_addr[(ETHARP_HWADDR_LEN - 1U) - temp];
    }
}

/**
 * @brief Initializes the PHY for the network interface.
 *
 * This function is responsible for initializing the PHY. It will read the PHY ID, ensure the PHY is alive,
 * check the link status, and perform any necessary setup for the PHY to operate correctly.
 *
 * @param hdkif Pointer to the hdkif device structure.
 * @return RESULT_SUCCESS if successful, or an appropriate error code if not.
 */
static results_enum_t hdkif_driver_initialize_phy(struct hdkif *hdkif)
{
    volatile uint32_t phy_id = 0U;
    uint32_t phy_id_read_count = PHY_ID_READ_MAX;

    while ((phy_id == 0U) && (phy_id_read_count > 0U))
    {
        phy_id = Dp83640IDGet(hdkif->mdio_base, hdkif->phy_addr);
        --phy_id_read_count;
    }

    if (!((MDIOPhyAliveStatusGet(hdkif->mdio_base) >> hdkif->phy_addr) & 0x01U))
    {
        return RESULT_ERR_CONNECTION;
    }

    if (!Dp83640LinkStatusGet(hdkif->mdio_base, hdkif->phy_addr, 0xFFFFU))
    {
        return RESULT_ERR_LINK;
    }

    if (hdkif_driver_link_setup(hdkif) != RESULT_SUCCESS)
    {
        return RESULT_ERR_INIT;
    }

    return RESULT_SUCCESS;
}

/**
 * @brief Set up the link by auto-negotiating with the PHY for link setup and configure EMAC accordingly.
 *
 * This function checks the instance number and, if it's 0, starts the auto-negotiation process.
 * If the auto-negotiation is successful, it checks the partner's abilities and sets the duplex
 * for the EMAC. The function blocks while a "delay" loop runs, presumably to wait for some hardware
 * state to settle. The result of the function indicates the success or failure of the link setup.
 *
 * @param hdkif The handle of the HDK interface. Must not be NULL.
 *
 * @return ERR_OK if the link setup was successful.
 *         ERR_CONN if the link setup failed.
 */
static results_enum_t hdkif_driver_link_setup(struct hdkif *hdkif)
{
    results_enum_t linkstat = RESULT_ERR_CONNECTION; /* Initialized to failure state */
    uint16 partnr_ablty;
    uint32 phyduplex = EMAC_DUPLEX_HALF; /* Assume half-duplex by default */
    const unsigned int delay_max = 0xFFFFFu; /* Upper-case U for MISRA compliance */
    volatile unsigned int delay = delay_max; /* Volatile as it may be used for timing, preventing unwanted optimization */

    if (hdkif == NULL) /* Check for NULL pointer */
    {
        return RESULT_ERR_PARAMATER; /* Can create a new error code ERR_INVALID_PARAM for MISRA Rule 15.7 compliance */
    }

    if (hdkif->inst_num == 0u) /* Literal unsigned value for MISRA compliance */
    {
        if (Dp83640AutoNegotiate(
                hdkif->mdio_base,
                hdkif->phy_addr,
                (DP83640_100BTX | DP83640_100BTX_FD | DP83640_10BT
                        | DP83640_10BT_FD)) != 0u) /* Assuming TRUE is defined as 1 */
        {

            /* Presumably, this function returns void. If it can fail, you should check the return value */
            if (0
                    != Dp83640PartnerAbilityGet(hdkif->mdio_base,
                                                hdkif->phy_addr, &partnr_ablty))
            {

                linkstat = RESULT_SUCCESS;

                if ((partnr_ablty & DP83640_100BTX_FD) != 0u)
                {
                    phyduplex = EMAC_DUPLEX_FULL;
                }
            }
        }
    }

    if (linkstat == RESULT_SUCCESS)
    {
        /* Presumably, this function returns void. If it can fail, you should check the return value */
        EMACDuplexSet(hdkif->emac_base, phyduplex);
    }

    while (delay > 0u) /* Comparison with 0 for MISRA compliance */
    {
        delay--;
    }

    return linkstat;
}

/**
 * @brief Initializes the TX descriptors for the network interface.
 *
 * This function prepares the TX descriptors for use. It sets up the linked list of TX buffer descriptors
 * and initializes each to a known state.
 *
 * @param hdkif Pointer to the hdkif device structure.
 */
static void hdkif_driver_initialize_tx_descriptors(struct hdkif *hdkif)
{
    struct txch *txch = &(hdkif->txch);
    volatile struct emac_tx_bdp *curr_txbd;
    volatile struct emac_tx_bdp *last_txbd;
    uint32_t num_bd = MAX_DESCRIPTOR_COUNT;

    txch->free_head = (volatile struct emac_tx_bdp*) (hdkif->emac_ctrl_ram);
    txch->next_bd_to_process = txch->free_head;
    txch->active_tail = NULL;

    curr_txbd = txch->free_head;

    /* Initialize all the TX buffer Descriptors */
    while (num_bd > 0U)
    {
        curr_txbd->next = hdkif_driver_swizzle_txp(curr_txbd + 1);
        curr_txbd->flags_pktlen = 0U;
        last_txbd = curr_txbd;
        curr_txbd = hdkif_driver_swizzle_txp(curr_txbd->next);
        --num_bd;
    }

    last_txbd->next = hdkif_driver_swizzle_txp(txch->free_head);
}

/**
 * @brief Initializes the RX descriptors for the network interface.
 *
 * This function prepares the RX descriptors for use. It sets up the linked list of RX buffer descriptors
 * and initializes each to a known state.
 *
 * @param hdkif Pointer to the hdkif device structure.
 * @return RESULT_SUCCESS if successful, or an appropriate error code if not.
 */
static results_enum_t hdkif_driver_initialize_rx_descriptors(
        struct hdkif *hdkif)
{
    struct rxch *rxch = &(hdkif->rxch);

    rxch->active_head = (volatile struct emac_rx_bdp*) (hdkif->emac_ctrl_ram
            + (SIZE_EMAC_CTRL_RAM >> 1));

    /* Remaining initializations */
    /* ... */

    return RESULT_SUCCESS;
}

/**
 * @brief Transmits an IPv4 packet on the network interface.
 *
 * This function is intended to be assigned to the netif's output
 * function pointer to transmit IPv4 packets. The function prepares
 * the packet to be sent by the lower layers and initiates the
 * transmission process.
 *
 * @param netif   Pointer to the network interface structure for which the
 *                packet is to be sent.
 * @param p       Pointer to the pbuf (packet buffer) structure containing
 *                the packet to be sent.
 * @param ipaddr  Pointer to the destination IPv4 address structure.
 *
 * @return ERR_OK if the packet was transmitted successfully.
 *         Another err_t code otherwise, indicating the nature of the error.
 */
static err_t hdkif_driver_netif_output(struct netif *netif, struct pbuf *p,
                                       const ip4_addr_t *ipaddr)
{
    return etharp_output(netif, p, ipaddr);
}

/**
 * @brief Transmits a packet on the network interface (link layer).
 *
 * This function is intended to be assigned to the netif's linkoutput
 * function pointer to transmit packets at the link layer. The function
 * is responsible for the actual transmission of the packet on the hardware.
 *
 * @param netif  Pointer to the network interface structure for which the
 *               packet is to be sent.
 * @param p      Pointer to the pbuf (packet buffer) structure containing
 *               the packet to be sent.
 *
 * @return ERR_OK if the packet was transmitted successfully.
 *         Another err_t code otherwise, indicating the nature of the error.
 */
static err_t hdkif_driver_netif_linkoutput(struct netif *netif, struct pbuf *p)
{
    err_t result = ERR_OK;
    SYS_ARCH_DECL_PROTECT(lev);

    /* Protect against concurrent access */
    SYS_ARCH_PROTECT(lev);

    /* If the packet is too short, pad it */
    if (p->tot_len < MIN_PKT_LEN)
    {
        /* This is a simplistic way to pad. Depending on your setup,
           you might need a more complex padding mechanism.
           This assumes there's enough space at the end of the buffer
           to add padding. */
        int padding = MIN_PKT_LEN - p->tot_len;
        memset((char *)p->payload + p->tot_len, 0, padding); // Add zero-padding
        p->tot_len = MIN_PKT_LEN;
        p->len = MIN_PKT_LEN;  // This assumes pbuf is not a chain. If pbufs can be chains, adjust this.
    }

    /* Increment the reference count */
    pbuf_ref(p);

    /* Transmit the packet. Assuming hdkif_driver_transmit returns an err_t.
       If not, you'll need to translate its return value to an err_t. */
    result = hdkif_driver_transmit(netif->state, p);

    /* If the transmission failed, decrement the reference count here.
       Otherwise, it should be decremented after successful transmission. */
    if (result != ERR_OK)
    {
        pbuf_free(p); // This decrements the reference count and frees if it reaches zero.
    }

    /* Release the protection */
    SYS_ARCH_UNPROTECT(lev);

    return result;
}


static results_enum_t hdkif_driver_transmit(struct hdkif *hdkif,
                                            struct pbuf *pbuf)
{
    if (hdkif == NULL || pbuf == NULL)
    {
        return RESULT_ERR_NULL;
    }

    struct pbuf *q;
    struct txch *txch;
    volatile struct emac_tx_bdp *curr_bd, *active_head, *bd_end;

    txch = &(hdkif->txch);

    // Get the buffer descriptor which is free to transmit
    curr_bd = txch->free_head;
    if (curr_bd == NULL)
    {
        return RESULT_ERR_NULL;
    }

    active_head = curr_bd;

    // Update the total packet length and set relevant flags
    uint32 flags_pktlen = pbuf->tot_len
            | (EMAC_BUF_DESC_SOP | EMAC_BUF_DESC_OWNER);
    curr_bd->flags_pktlen = hdkif_driver_swizzle_data(flags_pktlen);

    // Copy pbuf information into TX buffer descriptors
    for (q = pbuf; q != NULL; q = q->next)
    {
        // Initialize the buffer pointer and length
        curr_bd->bufptr = hdkif_driver_swizzle_data((uint32) (q->payload));
        curr_bd->bufoff_len = hdkif_driver_swizzle_data(q->len & 0xFFFF);
        bd_end = curr_bd;
        curr_bd->pbuf = pbuf;
        curr_bd = hdkif_driver_swizzle_txp(curr_bd->next);

        // Check if we've reached the end of the buffer descriptors
        if (curr_bd == NULL)
        {
            bd_end->next = NULL; // close the current descriptor list

            return RESULT_ERR_NULL;
        }
    }

    // Indicate the end of the packet
    bd_end->next = NULL;
    bd_end->flags_pktlen |= hdkif_driver_swizzle_data(EMAC_BUF_DESC_EOP);

    txch->free_head = curr_bd;

    // If this is the first packet, initiate transmission
    if (txch->active_tail == NULL)
    {
        EMACTxHdrDescPtrWrite(hdkif->emac_base, (unsigned int) active_head, 0);
    }
    else
    {
        // If the DMA engine reached the end of the chain, re-initiate transmission
        curr_bd = txch->active_tail;

        // Replace busy wait with an efficient method
        while (EMAC_BUF_DESC_EOQ
                != (hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                        & EMAC_BUF_DESC_EOQ))
        {
            // Consider replacing this with an interrupt-driven approach or adding delay
        }

        // Wait until it's safe to write to TXHDP0
        while (*((uint32*) 0xFCF78600) != 0)
        {
            // Consider replacing this with an interrupt-driven approach or adding delay
        }

        curr_bd->next = hdkif_driver_swizzle_txp(active_head);
        if (EMAC_BUF_DESC_EOQ
                == (hdkif_driver_swizzle_data(curr_bd->flags_pktlen)
                        & EMAC_BUF_DESC_EOQ))
        {
            EMACTxHdrDescPtrWrite(hdkif->emac_base, (unsigned int) active_head,
                                  0);
        }
    }

    txch->active_tail = bd_end;

    return RESULT_SUCCESS; // transmission initiated successfully
}

static uint32 hdkif_driver_swizzle_data(uint32 word)
{
#if defined(_TMS570LC43x_)
    return
        (((word << 24) & 0xFF000000) |
        ((word <<  8) & 0x00FF0000)  |
        ((word >>  8) & 0x0000FF00)  |
        ((word >> 24) & 0x000000FF));
#else
    return word;
#endif
}

static struct emac_tx_bdp* hdkif_driver_swizzle_txp(
        volatile struct emac_tx_bdp *p)
{
    return (struct emac_tx_bdp*) hdkif_driver_swizzle_data((uint32) p);
}

static struct emac_rx_bdp* hdkif_driver_swizzle_rxp(
        volatile struct emac_rx_bdp *p)
{
    return (struct emac_rx_bdp*) hdkif_driver_swizzle_data((uint32) p);
}
