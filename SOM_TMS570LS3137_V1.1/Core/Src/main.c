#include "main.h"
#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"

/* TI Libraries */
#include "het.h"
#include "gio.h"
#include "esm.h"
#include "sys_core.h"
#include "sys_vim.h"

/* standart libraries */
#include <stdio.h>
#include <stdint.h>

#include "results.h"
#include "lwiplib.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"

#include "mqtt_example.h"
#include "lwip/altcp.h"

#include "sci.h"

static results_enum_t ethernet_lwip_init(bool is_async);

/*
 * The tasks as described in the comments at the top of this file.
 */
static void defaultTask(void *pvParameters);

static void echo_server_thread(void *arg);
static void start_echo_server(void);

static void userTask(void *pvParameters);

extern uint8 emacAddress[6U];
void user_main(void)
{

#if 1

    vimInit();
    esmInit();
    _enable_IRQ();
    _enable_FIQ();
    _enable_interrupt_();
    esmEnableInterrupt(0xffffffff);
    sciInit();

    sys_thread_new("default", defaultTask, NULL, configMINIMAL_STACK_SIZE * 4,
                   osPriorityIdle);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
     line will never be reached.  If the following line does execute, then
     there was insufficient FreeRTOS heap memory available for the idle and/or
     timer tasks to be created.  See the memory management section on the
     FreeRTOS web site for more details. */
    for (;;)
        ;
#else

    vimInit();
    esmInit();
    _enable_IRQ();
    _enable_interrupt_();
    esmEnableInterrupt(0xffffffff);


  //  gioSetDirection(hetPORT1, 0XFFFFFFFF);

    ethernet_lwip_init(false);


    while(1)
    {

    }
#endif
}

static results_enum_t ethernet_lwip_init(bool is_async)
{
    results_enum_t return_value;

#if NO_SYS
  lwip_init();
#else
    tcpip_init(NULL, NULL);
#endif

    do
    {
        uint8_t ip_address_array[] = { 192, 168, 1, 14 };
        uint8_t net_mask_array[] = { 255, 255, 255, 0 };
        uint8_t gateway_address_array[] = { 192, 168, 1, 253 };

        do
        {
            return_value = lwip_init_async((uint8_t) 0, &emacAddress[0],
                                           &ip_address_array[0],
                                           &net_mask_array[0],
                                           &gateway_address_array[0],
                                           IPADDR_USE_DHCP,
                                           false);
        }
        while (RESULT_IN_THE_PROCESS == return_value && is_async == false);

        if (RESULT_SUCCESS == return_value)
        {

        }
        else
        {
        }
    }
    while (return_value != RESULT_SUCCESS);

    return return_value;
}
int task_counter = 0;
uint8_t dhcp_status;
uint8_t lock_mqtt_connect;
/*-----------------------------------------------------------*/

static void defaultTask(void *pvParameters)
{
    log_printf("program started\n");

    ethernet_lwip_init(false);

    for (;;)
    {
        dhcp_status = dhcp_supplied_address(&gnetif);

        if (1 == dhcp_status && 0 == lock_mqtt_connect)

        {
            start_echo_server();
            //   mqtt_example_init();
            lock_mqtt_connect = 1;

        }

        vTaskDelay(10);
    }
}
#include "lwip/api.h"

#define ECHO_SERVER_PORT 7  // Standard echo server TCP port

static void echo_server_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err, accept_err;

    // Create a new connection identifier.
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL)
    {
        // Bind connection to the defined port number.
        err = netconn_bind(conn, NULL, ECHO_SERVER_PORT);

        if (err == ERR_OK)
        {
            // Start listening for incoming connections.
            netconn_listen(conn);

            while (1)
            {
                // Accept any incoming connection.
                accept_err = netconn_accept(conn, &newconn);
                if (accept_err == ERR_OK)
                {
                    struct netbuf *buf;
                    void *data;
                    u16_t len;

                    while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
                    {
                        do
                        {
                            netbuf_data(buf, &data, &len);
                            netconn_write(newconn, data, len, NETCONN_COPY);
                        }
                        while (netbuf_next(buf) >= 0);

                        netbuf_delete(buf);
                    }

                    // Close connection and clean up.
                    netconn_close(newconn);
                    netconn_delete(newconn);
                }
            }
        }
        else
        {
            // Error occurred during bind, handle it.
            netconn_delete(newconn);
        }
    }
    else
    {
        // Error occurred during connection creation, handle it.
    }
}

static void start_echo_server(void)
{
    sys_thread_new("echo_server_tsk", echo_server_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

/*-----------------------------------------------------------*/

int log_printf(const char *__restrict _format, ...)
{

     char log_buffer[3000];

     va_list va;
     va_start(va, _format);
     int size = vsprintf(log_buffer, _format, va);

     if (size > 0)
     {
     sciSend(scilinREG, size, (uint8_t*) &log_buffer[0]);
     }

     va_end(va);

     return size;
}

