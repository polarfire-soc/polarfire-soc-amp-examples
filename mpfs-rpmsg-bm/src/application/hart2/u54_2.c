/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_2
 *
 * FreeRTOS RPMsg example project suitable for Asymmetric Multiprocessing (AMP) configuration
 *
 */

#include "mpfs_hal/mss_hal.h"
#include "inc/demo_main.h"

volatile uint32_t count_sw_ints_h2 = 0U;

/* Main function for the hart2(U54 processor).
 * Application code running on hart2 is placed here.
 */
void u54_2(void)
{
    volatile uint32_t icount = 0U;
    uint64_t hartid = read_csr(mhartid);
    
    /* Clear pending software interrupt in case there was any.
     * Enable only the software interrupt so that the E51 core can bring this
     * core out of WFI by raising a software interrupt In case of external,
     * bootloader not present
     */

    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

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

    if(MPFS_HAL_FIRST_HART == hartid)
    {
        start_demo();
        /* Never return */
    }

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

/* hart2 Software interrupt handler */

void Software_h2_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
    count_sw_ints_h2++;
}