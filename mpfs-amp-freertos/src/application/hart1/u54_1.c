/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_1
 *
 * FreeRTOS example project suitable for Asymmetric Multiprocessing (AMP) configuration
 *
 */


#include <stdio.h>
#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include <inc/application.h>

volatile uint32_t count_sw_ints_h1 = 0;


/* Main function for the hart2(U54_2 processor).
 * Application code running on hart4 is placed here
 */
void u54_1(void)
{
    volatile uint32_t icount = 0U;
    uint64_t hartid = read_csr(mhartid);

    /* Clear pending software interrupt in case there was any.
       Enable only the software interrupt so that the E51 core can bring this
       core out of WFI by raising a software interrupt. */
    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

    if(MPFS_HAL_FIRST_HART == hartid)
    {
        start_application();
        /* Never return */
    }

    /* Put this hart in WFI. */
    do
    {
        __asm("wfi");
    }while(0 == (read_csr(mip) & MIP_MSIP));

    /* The hart is now out of WFI, clear the SW interrupt. Here onwards the
     * application can enable and use any interrupts as required */
    clear_soft_interrupt();

     while (1u)
    {
        icount++;
        if (0x100000u == icount)
        {
            icount = 0u;
        }
    }

  /* Never return */
}

/* hart2 software interrupt handler */
void Software_h1_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
    count_sw_ints_h1++;
}
