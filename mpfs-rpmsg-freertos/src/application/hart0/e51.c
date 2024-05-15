/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on E51
 *
 * FreeRTOS RPMsg example project suitable for Asymmetric Multiprocessing (AMP) configuration
 *
 */

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "inc/demo_main.h"

volatile uint32_t count_sw_ints_h0 = 0U;

const uint8_t g_info_string[] =
"\r\n\r\n\r\n **** PolarFire SoC AMP FreeRTOS example  ****\r\n\r\n\r\n\
This program should be run from an application core (U54) in an AMP build.\r\n\r\n\
For more information refer to the project's README\r\n\
\r\n";

/* Main function for the hart0(E51 processor).
 * Application code running on hart0 is placed here. */
void e51(void)
{
    volatile uint32_t icount = 0U;

    (void)mss_config_clk_rst(MSS_PERIPH_MMUART0, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);

    MSS_UART_init( &g_mss_uart0_lo,
            MSS_UART_115200_BAUD,
            MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

    MSS_UART_polled_tx_string(&g_mss_uart0_lo, g_info_string);

#if (IMAGE_LOADED_BY_BOOTLOADER == 0)
    /* Clear pending software interrupt in case there was any. */
    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

    /* Raise software interrupt to wake hart 1 */
    raise_soft_interrupt(1U);

    __enable_irq();
#endif

    while (1U)
    {
        icount++;

        if (0x100000U == icount)
        {
            icount = 0U;
        }
    }
    /* never return */
}

/* hart0 software interrupt handler */
void Software_h0_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
    count_sw_ints_h0++;
}

