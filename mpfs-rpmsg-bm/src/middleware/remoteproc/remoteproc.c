/*******************************************************************************
 * Copyright 2019-2022 Microchip FPGA Embedded Systems Solutions..
 *
 * SPDX-License-Identifier: MIT
 *
 * MPFS Embedded Software
 *
 * This file provides helper functions to enable the ihc communication channel.
 * This allows to receive control messages (start, stop, etc) from Linux without
 * the need of having rpmsg enabled.
 */

#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"
#include "miv_ihc.h"
#include "remoteproc.h"

#ifdef USING_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#endif

typedef void (*func_t)(void);
extern const uint64_t __scratchpad_start;
#define SCRATCHPAD_START    (&__scratchpad_start)

static uint32_t rx_handler(uint32_t remote_hart_id, uint32_t * message, uint32_t message_size, bool is_ack, uint32_t *message_storage_ptr );

enum miv_rp_mbox_messages {
    MIV_RP_MBOX_READY = 0xFFFFFF00,
    MIV_RP_MBOX_PENDING_MSG = 0xFFFFFF01,
    MIV_RP_MBOX_STOP = 0xFFFFFF02,
    MIV_RP_MBOX_END_MSG = 0xFFFFFF03,
};

void rproc_setup(void)
{
    uint64_t hartid = read_csr(mhartid);

    uint32_t context_hart_id = 0u;

    IHC_local_context_init((uint32_t)hartid);

    uint32_t remote_hart_id = IHC_partner_context_hart_id(hartid);

    IHC_local_remote_config((uint32_t)hartid, remote_hart_id, rx_handler, true, true);

    PLIC_init();
    context_hart_id = IHC_context_to_context_hart_id(hartid);

    switch(context_hart_id) {
        case 1:
            PLIC_SetPriority(IHCIA_hart1_INT, 2);
            PLIC_EnableIRQ(IHCIA_hart1_INT);
            break;
        case 2:
            PLIC_SetPriority(IHCIA_hart2_INT, 2);
            PLIC_EnableIRQ(IHCIA_hart2_INT);
            break;
        case 3:
            PLIC_SetPriority(IHCIA_hart3_INT, 2);
            PLIC_EnableIRQ(IHCIA_hart3_INT);
            break;
        case 4:
            PLIC_SetPriority(IHCIA_hart4_INT, 2);
            PLIC_EnableIRQ(IHCIA_hart4_INT);
            break;
        default:
            /*  Unsupported configuration value*/
            break;
    }

    __enable_irq();

}

void rproc_stop()
{
    uint64_t my_hart_id = read_csr(mhartid);
    uint32_t context_hart_id = 0u;

    context_hart_id = IHC_context_to_context_hart_id(my_hart_id);
    switch(context_hart_id) {
        case 1:
            PLIC_CompleteIRQ(IHCIA_hart1_INT);
            PLIC_DisableIRQ(IHCIA_hart1_INT);
            break;
        case 2:
            PLIC_CompleteIRQ(IHCIA_hart2_INT);
            PLIC_DisableIRQ(IHCIA_hart2_INT);
            break;
        case 3:
            PLIC_CompleteIRQ(IHCIA_hart3_INT);
            PLIC_DisableIRQ(IHCIA_hart3_INT);
            break;
        case 4:
            PLIC_CompleteIRQ(IHCIA_hart4_INT);
            PLIC_DisableIRQ(IHCIA_hart4_INT);
            break;
        default:
            /*  Unsupported configuration value*/
            return;
    }

    ((func_t)SCRATCHPAD_START)();
}

uint32_t rx_handler( uint32_t remote_hart_id, uint32_t * message, uint32_t message_size, bool is_ack, uint32_t *message_storage_ptr )
{
    (void)remote_hart_id;

    if( is_ack == true )
    {
        return 0;
    }

    switch(*message) {
        case MIV_RP_MBOX_STOP:
            rproc_stop();
            break;
        default:
            /* silently handle all other valid messages */
            return(0U);
    }

    return(0U);
}
