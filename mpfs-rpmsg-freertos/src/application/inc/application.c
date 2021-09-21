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
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "inc/application.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#define UART_APP &g_mss_uart3_lo

const uint8_t g_message1[] =
"\r\n\r\n\r\n **** PolarFire SoC Icicle Kit AMP FreeRTOS example  ****\r\n\r\n\r\n";

uint8_t __attribute__ ((section (".FreeRTOSheap"))) ucHeap[configTOTAL_HEAP_SIZE];

SemaphoreHandle_t xSemaphore = NULL;

void freertos_task_one( void *pvParameters );
void freertos_task_two( void *pvParameters );

void start_application()
{
	BaseType_t rtos_result;

#if (IMAGE_LOADED_BY_BOOTLOADER == 0)

    /*Put this hart into WFI.*/

    do
    {
        __asm("wfi");
    }while(0 == (read_csr(mip) & MIP_MSIP));

    /* The hart is out of WFI, clear the SW interrupt. Hear onwards Application
     * can enable and use any interrupts as required */
    clear_soft_interrupt();
#endif

    (void)mss_config_clk_rst(MSS_PERIPH_MMUART3, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
    (void)mss_config_clk_rst(MSS_PERIPH_GPIO2, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    PLIC_init();

    MSS_GPIO_init(GPIO2_LO);

    MSS_GPIO_config(GPIO2_LO,
                    MSS_GPIO_16,
                    MSS_GPIO_OUTPUT_MODE);

    MSS_UART_init(UART_APP, MSS_UART_115200_BAUD,
                   MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY);

    rtos_result = xTaskCreate( freertos_task_one, "task1", 4000, NULL, uartPRIMARY_PRIORITY, NULL );
    if(1 != rtos_result)
    {
        int ix;
        for(;;)
            ix++;
    }

    rtos_result = xTaskCreate( freertos_task_two, "task2", 4000, NULL, uartPRIMARY_PRIORITY, NULL );
    if(1 != rtos_result)
    {
        int ix;
        for(;;)
            ix++;
    }

    xSemaphore = xSemaphoreCreateMutex();

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

}

volatile uint64_t* mtime = (volatile uint64_t*)0x0200bff8;
volatile uint64_t* timecmp = ((volatile uint64_t*)0x02004000) + MPFS_HAL_FIRST_HART;

void freertos_task_one( void *pvParameters )
{
    int8_t info_string[50];
    vPortSetupTimer();

    if(xSemaphoreTake(xSemaphore, portMAX_DELAY)== pdTRUE)
    {
        MSS_UART_polled_tx(UART_APP, g_message1, sizeof(g_message1));
        xSemaphoreGive( xSemaphore );
    }

    while(1)
    {
        if(xSemaphoreTake(xSemaphore, portMAX_DELAY)== pdTRUE)
        {
            sprintf(info_string,"\r\nRunning FreeRTOS task 1 from hart %d\r\n", read_csr(mhartid));
            MSS_UART_polled_tx(UART_APP, info_string, strlen(info_string));
            xSemaphoreGive( xSemaphore );
        }
        vTaskDelay(300);
    }

}

void freertos_task_two( void *pvParameters )
{
    int8_t info_string[50];
    static volatile uint8_t value = 0u;

    while(1)
    {
        if(xSemaphoreTake(xSemaphore, portMAX_DELAY)== pdTRUE)
        {
            sprintf(info_string,"\r\nRunning FreeRTOS task 2 from hart %d\r\n", read_csr(mhartid));
            MSS_UART_polled_tx(UART_APP, info_string, strlen(info_string));
            xSemaphoreGive( xSemaphore );
        }

        if(0u == value)
        {
            value = 0x01u;
        }
        else
        {
            value = 0x00u;
        }

        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_16, value);

        vTaskDelay(500);
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
