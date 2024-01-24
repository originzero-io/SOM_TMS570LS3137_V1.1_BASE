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
static results_enum_t ethernet_lwip_init(bool is_async);

/*
 * The tasks as described in the comments at the top of this file.
 */
static void defaultTask(void *pvParameters);
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
/*-----------------------------------------------------------*/

static err_t tcp_connect_cb(void *arg, struct altcp_pcb *tpcb, err_t err)
{
    return ERR_OK;
}

uint8_t lock_mqtt_connect = 0;
volatile uint8_t dhcp_status = 0;
static void defaultTask(void *pvParameters)
{
    log_printf("program started\n");

    ethernet_lwip_init(false);

    for (;;)
    {
        dhcp_status = dhcp_supplied_address(&gnetif);

        if (1 == dhcp_status && 0 == lock_mqtt_connect)
        {
            mqtt_example_init();
            lock_mqtt_connect = 1;
        }

        task_counter++;
        vTaskDelay(1000);
    }
}

/*-----------------------------------------------------------*/

int log_printf(const char *__restrict _format, ...)
{
    /*
     char log_buffer[3000];

     va_list va;
     va_start(va, _format);
     int size = vsprintf(log_buffer, _format, va);

     if (size > 0)
     {
     sciSend(scilinREG, size, (uint8_t*) &log_buffer[0]);
     }

     va_end(va);

     return size;*/
    return 0;
}

