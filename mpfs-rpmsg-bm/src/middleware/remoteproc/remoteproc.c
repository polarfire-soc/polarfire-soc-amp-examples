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
#include "rpmsg_platform.h"

#ifdef USING_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#else
#include "utils.h"
#endif

typedef void (*func_t)(void);
extern const uint64_t __scratchpad_start;
#define SCRATCHPAD_START    (&__scratchpad_start)

#define IHC_APP_X_H0_IRQ  FABRIC_F2H_63_PLIC
#define IHC_APP_X_H1_IRQ  FABRIC_F2H_62_PLIC
#define IHC_APP_X_H2_IRQ  FABRIC_F2H_61_PLIC
#define IHC_APP_X_H3_IRQ  FABRIC_F2H_60_PLIC
#define IHC_APP_X_H4_IRQ  FABRIC_F2H_59_PLIC

enum miv_rp_mbox_messages {
    MIV_RP_MBOX_READY = 0xFFFFFF00,
    MIV_RP_MBOX_PENDING_MSG = 0xFFFFFF01,
    MIV_RP_MBOX_STOP = 0xFFFFFF02,
    MIV_RP_MBOX_END_MSG = 0xFFFFFF03,
};

static uint32_t QUEUE_IHC_MP_ISR_CALLBACK(uint8_t channel,
                           const uint32_t *message,
                           uint32_t message_size, uint32_t * ext_msg_ptr)
{
    uint32_t msg = message[0];
 
     switch(msg) {
        case MIV_RP_MBOX_STOP:
            rproc_stop(channel);
            break;
        default:
            return(0U);
    }

    return 0u;
}

static uint32_t QUEUE_IHC_MC_ISR_CALLBACK(uint8_t channel,
                           const uint32_t *message,
                           uint32_t message_size, uint32_t * ext_msg_ptr)
{
    return 0;
}

void rproc_setup(uint32_t link_id)
{
    uint32_t channel_id = RL_GET_COM_ID_FROM_LINK_ID(link_id);
    IHC_init(channel_id);
    IHC_config_mp_callback_handler(channel_id, QUEUE_IHC_MP_ISR_CALLBACK);
    IHC_config_mc_callback_handler(channel_id, QUEUE_IHC_MC_ISR_CALLBACK);
    IHC_enable_mp_interrupt(channel_id);
    IHC_enable_mc_interrupt(channel_id);

#ifdef USING_FREERTOS
    vTaskDelay(300);
#else
    SpinDelay_MilliSecs(300);
#endif

    PLIC_init();

    switch (channel_id)
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            PLIC_SetPriority(IHC_APP_X_H0_IRQ, 2);
            PLIC_EnableIRQ(IHC_APP_X_H0_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            PLIC_SetPriority(IHC_APP_X_H1_IRQ, 2);
            PLIC_EnableIRQ(IHC_APP_X_H1_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            PLIC_SetPriority(IHC_APP_X_H2_IRQ, 2);
            PLIC_EnableIRQ(IHC_APP_X_H2_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            PLIC_SetPriority(IHC_APP_X_H3_IRQ, 2);
            PLIC_EnableIRQ(IHC_APP_X_H3_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            PLIC_SetPriority(IHC_APP_X_H4_IRQ, 2);
            PLIC_EnableIRQ(IHC_APP_X_H4_IRQ);
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
    }

    __enable_irq();

}

void rproc_stop(uint32_t channel)
{

    switch (channel)
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            PLIC_CompleteIRQ(IHC_APP_X_H0_IRQ);
            PLIC_DisableIRQ(IHC_APP_X_H0_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            PLIC_CompleteIRQ(IHC_APP_X_H1_IRQ);
            PLIC_DisableIRQ(IHC_APP_X_H1_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            PLIC_CompleteIRQ(IHC_APP_X_H2_IRQ);
            PLIC_DisableIRQ(IHC_APP_X_H2_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            PLIC_CompleteIRQ(IHC_APP_X_H3_IRQ);
            PLIC_DisableIRQ(IHC_APP_X_H3_IRQ);
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            PLIC_CompleteIRQ(IHC_APP_X_H4_IRQ);
            PLIC_DisableIRQ(IHC_APP_X_H4_IRQ);
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
    }

    ((func_t)SCRATCHPAD_START)();
}

