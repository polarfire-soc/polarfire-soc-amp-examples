/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code
 *
 * FreeRTOS example project suitable for Asymmetric Multiprocessing (AMP) configuration
 *
 */


#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"
#include "drivers/mss_mmuart/mss_uart.h"
#include <inc/application.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#define UART_DEMO &g_mss_uart3_lo

const uint8_t g_message1[] =
"\r\n\r\n\r\n **** PolarFire SoC Icicle Kit AMP FreeRTOS example  ****\r\n\r\n\r\n";

void freertos_task( void *pvParameters );

void start_application()
{
	BaseType_t rtos_result;

    PLIC_init();

    MSS_UART_init(UART_DEMO, MSS_UART_115200_BAUD,
                   MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY);

    rtos_result = xTaskCreate( freertos_task, "task1", 4000, NULL, uartPRIMARY_PRIORITY, NULL );
    if(1 != rtos_result)
    {
        int ix;
        for(;;)
            ix++;
    }

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

}

volatile uint64_t* mtime = (volatile uint64_t*)0x0200bff8;
volatile uint64_t* timecmp = ((volatile uint64_t*)0x02004000) + MPFS_HAL_FIRST_HART;

void freertos_task( void *pvParameters )
{
    vPortSetupTimer();

    vTaskDelay(1000);

    MSS_UART_polled_tx(UART_DEMO, g_message1, sizeof(g_message1));

    while(1){
        MSS_UART_polled_tx_string(UART_DEMO,
                                  "\r\nRunning FreeRTOS task...\r\n");
        vTaskDelay(300);
    }

}

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
