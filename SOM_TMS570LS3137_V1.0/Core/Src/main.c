#include "FreeRTOS.h"
#include "os_queue.h"
#include "os_task.h"

/* TI Libraries */
#include "het.h"
#include "gio.h"

#include "results.h"
#include "lwiplib.h"

static results_enum_t ethernet_lwip_init(bool is_async);

/* My Libraries */
#include "user_main.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_DEFAULT_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_USER_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
 to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS         ( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold.  This is 1 as the receive task
 will remove items as they are added, meaning the send task should always find
 the queue empty. */
#define mainQUEUE_LENGTH                    ( 1 )

/*-----------------------------------------------------------*/
/* Values passed to the two tasks just to check the task parameter
 functionality. */
#define mainQUEUE_DEFAULT_PARAMETER            ( 0x1111UL )
#define mainQUEUE_USER_PARAMETER         ( 0x22UL )
/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void defaultTask(void *pvParameters);
static void userTask(void *pvParameters);

/*-----------------------------------------------------------*/

/**
 * main.c
 */
void user_main(void)
{
#if 1
    hetInit();
    gioInit();

    vimInit();
    esmInit();
    _enable_IRQ();
    _enable_interrupt_();
    esmEnableInterrupt(0xffffffff);


    gioSetDirection(hetPORT1, 0XFFFFFFFF);


    xTaskCreate(defaultTask, /* The function that implements the task. */
                "default", /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                configMINIMAL_STACK_SIZE*4, /* The size of the stack to allocate to the task. */
                (void*) mainQUEUE_DEFAULT_PARAMETER, /* The parameter passed to the task - just to check the functionality. */
                mainQUEUE_DEFAULT_TASK_PRIORITY, /* The priority assigned to the task. */
                NULL); /* The task handle is not required, so NULL is passed. */
/*    xTaskCreate(userTask, "user", configMINIMAL_STACK_SIZE,
                (void*) mainQUEUE_USER_PARAMETER,
                mainQUEUE_USER_TASK_PRIORITY,
                NULL);*/

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
     line will never be reached.  If the following line does execute, then
     there was insufficient FreeRTOS heap memory available for the idle and/or
     timer tasks to be created.  See the memory management section on the
     FreeRTOS web site for more details. */
    for (;;)
        ;
#else

    hetInit();
    gioInit();

    vimInit();
    esmInit();
    _enable_IRQ();
    _enable_interrupt_();
    esmEnableInterrupt(0xffffffff);


    gioSetDirection(hetPORT1, 0XFFFFFFFF);

    ethernet_lwip_init(false);


    while(1)
    {

    }
#endif
}

extern uint8 emacAddress[6U];

void user_main2(void)
{

    while (1)
    {

    }
}

static results_enum_t ethernet_lwip_init(bool is_async)
{
    results_enum_t return_value;
    lwip_init();  // Initialize the LwIP stack
    do
    {
        uint8_t ip_address_array[] = { 192, 168, 1, 14 };
        uint8_t net_mask_array[] = { 255, 255, 255, 0 };
        uint8_t gateway_address_array[] = { 192, 168, 1, 253 };

        return_value = lwip_init_sync(0, &emacAddress[0], &ip_address_array[0],
                                      &net_mask_array[0],
                                      &gateway_address_array[0],
                                      IPADDR_USE_DHCP);

    }
    while (return_value != RESULT_SUCCESS);

    return return_value;
}

/*-----------------------------------------------------------*/

static void defaultTask(void *pvParameters)
{
    ethernet_lwip_init(false);

    for (;;)
    {
        vTaskDelay(100);
        gioToggleBit(hetPORT1, 5);

    }
}

/*-----------------------------------------------------------*/

static void userTask(void *pvParameters)
{
    setup();
    for (;;)
    {
        loop();

        vTaskDelay(1);
    }
}
/*-----------------------------------------------------------*/

