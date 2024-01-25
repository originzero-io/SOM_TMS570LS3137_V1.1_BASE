#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include "ethernetif.h"
#include <string.h>
#include "cmsis_os.h"
#include "lwip/tcpip.h"
#include "emac_driver.h"

#define TIME_WAITING_FOR_INPUT ( portMAX_DELAY )

QueueHandle_t rx_queue;

/* Network interface name */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define ETH_MAX_PAYLOAD MAX_TRANSFER_UNIT
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE ( 350 * 2 )

#define ETHERNETIF_RX_QUEUE_LEN 10

typedef struct ethernetif_rx_quiue
{
    uint8_t payload[ETH_MAX_PAYLOAD];
    uint16_t len;
    uint8_t is_filled;
} ethernetif_rx_quiue_t;

ethernetif_rx_quiue_t ethernetif_rx_quiue[ETHERNETIF_RX_QUEUE_LEN];
uint8_t ethernetif_rx_quiue_counter = 0;

static uint8_t mac_address[ETH_HWADDR_LEN];

osSemaphoreId RxPktSemaphore = NULL; /* Semaphore to signal incoming packets */
osSemaphoreId TxPktSemaphore = NULL; /* Semaphore to signal transmit packet complete */

static err_t low_level_init(struct netif *netif);
static struct pbuf* low_level_input(struct netif *netif);
static void ethernetif_input(void const *argument);
static err_t low_level_output(struct netif *netif, struct pbuf *p);

pbuf_t* convert_pbuf_to_pbuf_t(const struct pbuf *pbuf_ptr);
void free_pbuf_t(pbuf_t *pbuf_t_ptr);

static struct pbuf* rxch_to_pbuf(rxch_t *rxch);

/**
 * @brief Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
#else
  /* The user should write its own code in low_level_output_arp_off function */
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    return low_level_init(netif);

}
uint32_t EMAC_to_pbuf_counter;
/**
 * Converts received EMAC data to LwIP pbuf.
 *
 * @param hdkif interface structure
 * @return struct pbuf* Pointer to the pbuf containing received data, or NULL in case of error.
 */
struct pbuf* EMAC_to_pbuf(hdkif_t *hdkif)
{
    struct pbuf *p = NULL;

    rxch_t *rxch_int = &(hdkif->rxchptr);
    volatile emac_rx_bd_t *curr_bd = rxch_int->active_head;

    while ((curr_bd->flags_pktlen & EMAC_BUF_DESC_OWNER) != EMAC_BUF_DESC_OWNER)
    {
        // Copy data from buffer descriptor to pbuf
        ethernetif_rx_quiue[ethernetif_rx_quiue_counter].len =
                (curr_bd->bufoff_len & 0xFFFF);

        memcpy(&ethernetif_rx_quiue[ethernetif_rx_quiue_counter].payload[0],
               (void*) curr_bd->bufptr,
               ethernetif_rx_quiue[ethernetif_rx_quiue_counter].len);

        ethernetif_rx_quiue[ethernetif_rx_quiue_counter].is_filled = 1;

        BaseType_t xHigherPriorityTaskWoken = pdTRUE;
        BaseType_t xStatus = xQueueSendFromISR(rx_queue,
                                               &ethernetif_rx_quiue_counter,
                                               &xHigherPriorityTaskWoken);

        if (ethernetif_rx_quiue_counter >= (ETHERNETIF_RX_QUEUE_LEN - 1))
        {
            ethernetif_rx_quiue_counter = 0;
        }
        else
        {
            ethernetif_rx_quiue_counter++;
        }

        curr_bd = curr_bd->next;
    }

    return p;
}

void ethernetif_rx_callback(hdkif_t *hdkif)
{

    EMAC_to_pbuf(hdkif);
    osSemaphoreRelease(RxPktSemaphore);
}

void ethernetif_tx_callback(hdkif_t *hdkif)
{
    osSemaphoreRelease(TxPktSemaphore);
}

void ethernetif_set_mac_address(uint8_t *mac)
{
    if (NULL_PTR != mac)
    {
        for (uint8_t i = 0; i < ETH_HWADDR_LEN; i++)
        {
            mac_address[i] = mac[i];
        }
    }
}

/*******************************************************************************
 LL Driver Interface ( LwIP stack --> ETH)
 *******************************************************************************/
/**
 * @brief In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static err_t low_level_init(struct netif *netif)
{
    err_t return_value = ERR_OK;
    /* Start ETH Init */

#if LWIP_ARP || LWIP_ETHERNET

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    for (uint8_t i = 0; i < ETH_HWADDR_LEN; i++)
    {
        netif->hwaddr[i] = mac_address[i];
    }

    /* maximum transfer unit */
    netif->mtu = ETH_MAX_PAYLOAD;

    /* Accept broadcast address and ARP traffic */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
  #endif /* LWIP_ARP */

#endif /* LWIP_ARP || LWIP_ETHERNET */

    /* create a binary semaphore used for informing ethernetif of frame reception */
    RxPktSemaphore = xSemaphoreCreateBinary();
    if (NULL == RxPktSemaphore)
    {
        return_value = ERR_IF;
    }
    /* create a binary semaphore used for informing ethernetif of frame transmission */
    TxPktSemaphore = xSemaphoreCreateBinary();
    if (NULL == TxPktSemaphore)
    {
        return_value = ERR_IF;
    }
    /* create the task that handles the ETH_MAC */

    osThreadDef(EthIf, ethernetif_input, osPriorityRealtime, 0,
                INTERFACE_THREAD_STACK_SIZE);
    if (NULL == osThreadCreate(osThread(EthIf), netif))
    {
        return_value = ERR_IF;
    }

    rx_queue = xQueueCreate(ETHERNETIF_RX_QUEUE_LEN, sizeof(uint8_t));
    if (NULL == rx_queue)
    {
        return_value = ERR_IF;
    }

    if (EMAC_ERR_OK != EMACHWInit(&mac_address[0]))
    {
        return_value = ERR_IF;
    }

    EMACTxPrioritySelect(hdkif_data[0].emac_base, 2);

    return return_value;
}

/**
 * @brief Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf* low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL_PTR;
    ethernetif_rx_quiue_t *quiue_item = NULL;
    uint8_t index;
    if (xQueueReceive(rx_queue, &index, 10))
    {
        quiue_item = &ethernetif_rx_quiue[index];

        p = pbuf_alloc(PBUF_RAW, quiue_item->len, PBUF_POOL);
        if (NULL != p)
        {
            pbuf_take(p, (void*) &quiue_item->payload[0], quiue_item->len);
        }

        quiue_item->is_filled = 0;
    }

    return p;
}

/**
 * @brief This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void ethernetif_input(void const *argument)
{
    struct pbuf *p = NULL;
    struct netif *netif = (struct netif*) argument;

    for (;;)
    {
        if (osSemaphoreWait(RxPktSemaphore, TIME_WAITING_FOR_INPUT) == osOK)
        {
            do
            {
                p = low_level_input(netif);
                if (p != NULL)
                {
                    if (netif->input(p, netif) != ERR_OK)
                    {
                        pbuf_free(p);
                    }

                }
            }
            while (p != NULL);
        }
    }
}

/**
 * @brief This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
osThreadId thread_id;

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{

    if (p->tot_len < MIN_PKT_LEN)
    {
        p->tot_len = MIN_PKT_LEN;
        p->len = MIN_PKT_LEN;
    }

    pbuf_ref(p);

    pbuf_t *emac_pbuf = convert_pbuf_to_pbuf_t(p);
    if (NULL == emac_pbuf)
    {
        pbuf_free(p);
    }
    else
    {
        // Reference the pbuf to prevent it from being freed prematurely

        if (false == EMACTransmit(&hdkif_data[0], emac_pbuf))
        {
            pbuf_free(p);
        }
        else
        {

            while (osSemaphoreWait(TxPktSemaphore, TIME_WAITING_FOR_INPUT)
                    != osOK)
            {

            }

            free_pbuf_t(emac_pbuf);
        }
    }
    return ERR_OK;
}

pbuf_t* convert_pbuf_to_pbuf_t(const struct pbuf *pbuf_ptr)
{
    if (pbuf_ptr == NULL)
    {
        return NULL;
    }

    // Allocate memory for the new pbuf_t
    pbuf_t *pbuf_t_ptr = (pbuf_t*) mem_malloc(sizeof(pbuf_t));
    if (pbuf_t_ptr == NULL)
    {
        // Handle memory allocation failure
        return NULL;
    }

    // Copy data from pbuf to pbuf_t
    pbuf_t_ptr->next = convert_pbuf_to_pbuf_t(pbuf_ptr->next); // Recursive call for linked list
    pbuf_t_ptr->payload = pbuf_ptr->payload;

    pbuf_t_ptr->tot_len = (uint16) pbuf_ptr->tot_len;
    pbuf_t_ptr->len = (uint16) pbuf_ptr->len;

    return pbuf_t_ptr;
}

void free_pbuf_t(pbuf_t *pbuf_t_ptr)
{
    while (pbuf_t_ptr != NULL)
    {
        pbuf_t *next = pbuf_t_ptr->next;
        mem_free(pbuf_t_ptr);
        pbuf_t_ptr = next;
    }
}

static struct pbuf* rxch_to_pbuf(rxch_t *rxch)
{
    if (rxch == NULL || rxch->active_head == NULL)
    {
        return NULL;  // Return NULL if rxch is NULL or has no data
    }

    emac_rx_bd_t *active = (emac_rx_bd_t*) rxch->active_head;

    // Extract the packet length and flags from flags_pktlen
    uint16_t packet_length = active->flags_pktlen & 0xFFFF;
    uint16_t flags = (active->flags_pktlen >> 16) & 0xFFFF;

    // Allocate a pbuf using lwIP function
    struct pbuf *p = pbuf_alloc(PBUF_RAW, packet_length, PBUF_RAM);
    if (p == NULL)
    {
        // Handle allocation failure
        return NULL;
    }

    // Set payload and length for the first pbuf
    p->payload = (void*) active->bufptr;
    p->len = p->tot_len = packet_length;

    // Iterate through the chain to add additional pbufs
    while (active->next != NULL)
    {
        active = active->next;
        // Extract the packet length and flags for the next segment
        packet_length = active->flags_pktlen & 0xFFFF;
        flags = (active->flags_pktlen >> 16) & 0xFFFF;

        // Allocate a new pbuf for the next segment
        struct pbuf *q = pbuf_alloc(PBUF_RAW, packet_length, PBUF_RAM);
        if (q == NULL)
        {
            // Handle allocation failure
            pbuf_free(p); // Free the already allocated pbufs
            return NULL;
        }
        // Set payload and length for the new pbuf
        q->payload = (void*) active->bufptr;
        q->len = q->tot_len = packet_length;
        // Chain the new pbuf to the existing ones
        pbuf_chain(p, q);
    }

    return p;
}

