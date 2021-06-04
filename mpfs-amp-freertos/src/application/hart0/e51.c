/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on E51
 *
 * FreeRTOS example project suitable for Asymmetric Multiprocessing (AMP) configuration
 *
 */


#include <stdio.h>
#include "mpfs_hal/mss_hal.h"
#include "drivers/mss_mmuart/mss_uart.h"
#include <inc/application.h>

uint8_t g_message[] =
"\r\n\r\n\r\n **** PolarFire SoC Icicle Kit AMP FreeRTOS example  ****\r\n\r\n\r\n\
This program should be run from an application core (U54) in an AMP build.\r\n\r\n\
For more information refer to the project README\r\n\
\r\n";

uint64_t uart_lock;

/* Created for convenience. Uses Polled method of UART TX */
void uart_tx_with_mutex (mss_uart_instance_t * this_uart,
                                uint64_t mutex_addr,
                                const uint8_t * pbuff)
{
    mss_take_mutex(mutex_addr);
    MSS_UART_polled_tx_string(this_uart, pbuff);
    mss_release_mutex(mutex_addr);
}

/* Main function for the hart0(E51 processor).
 * Application code running on hart0 is placed here. */
void e51(void)
{
    mss_init_mutex((uint64_t)&uart_lock);

    /* Turn on peripheral clocks */
    SYSREG->SOFT_RESET_CR &= ~(SOFT_RESET_CR_MMUART0_MASK |\
            SOFT_RESET_CR_CFM_MASK);

    /* Turn on peripheral clocks */
    SYSREG->SUBBLK_CLOCK_CR |= (SUBBLK_CLOCK_CR_MMUART0_MASK |\
            SUBBLK_CLOCK_CR_CFM_MASK);

    mss_take_mutex(uart_lock);
    MSS_UART_init(&g_mss_uart0_lo,
            MSS_UART_115200_BAUD,
            MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);
    mss_release_mutex(uart_lock);

    uart_tx_with_mutex(&g_mss_uart0_lo, (uint64_t)&uart_lock,
                       g_message);

    /* Wake-up the U54_1 from WIF. U54_1 will read sensor data over I2C and
     * display it on the terminal. Since floating point operations are involved
     * U54_1 will carry these out; Keeping SoftConsole project target settings
     * common.*/
    raise_soft_interrupt(1u);

    while(1u)
    {
        /* No return */
    }
}
