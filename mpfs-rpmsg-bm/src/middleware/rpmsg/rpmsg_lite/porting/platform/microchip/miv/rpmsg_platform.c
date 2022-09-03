/*
 * Copyright 2016-2019 NXP
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "rpmsg_platform.h"
#include "rpmsg_env.h"
#include "mpfs_hal/mss_hal.h"
#include "miv_ihc.h"

#ifdef USING_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#else
#include "utils.h"
#endif

#ifdef REMOTEPROC
#include "remoteproc.h"
#endif

#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif

#if !defined(LIBERO_SETTING_CONTEXT_A_HART_EN) || !defined(LIBERO_SETTING_CONTEXT_B_HART_EN)
#error "LIBERO_SETTING_CONTEXT_A_HART_EN and LIBERO_SETTING_CONTEXT_B_HART_EN macros required for rpmsg communication"
#elif LIBERO_SETTING_CONTEXT_A_HART_EN == 0 || LIBERO_SETTING_CONTEXT_B_HART_EN == 0
#error "LIBERO_SETTING_CONTEXT_A_HART_EN and LIBERO_SETTING_CONTEXT_B_HART_EN macros are not setup"
#endif

static int32_t isr_counter     = 0;
static int32_t disable_counter = 0;
static void *platform_lock;

enum miv_rp_mbox_messages {
	MIV_RP_MBOX_READY = 0xFFFFFF00,
	MIV_RP_MBOX_PENDING_MSG = 0xFFFFFF01,
    MIV_RP_MBOX_STOP = 0xFFFFFF02,
    MIV_RP_MBOX_END_MSG = 0xFFFFFF03,
};

#ifdef USING_FREERTOS
static xTaskHandle task_handle;
#else
static uint8_t ack_notify = 0;
#endif

static uint32_t rx_handler(uint32_t remote_hart_id, uint32_t * message, uint32_t message_size, bool is_ack, uint32_t *message_storage_ptr );
static void rpmsg_handler(bool is_ack, uint32_t msg);

static void platform_global_isr_disable(void)
{
    __disable_irq();
}

static void platform_global_isr_enable(void)
{
    __enable_irq();
}

int32_t platform_init_interrupt(uint32_t vector_id, void *isr_data)
{
    uint64_t my_hart_id = read_csr(mhartid);

    uint32_t context_hart_id = 0u;
    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    env_lock_mutex(platform_lock);

    RL_ASSERT(0 <= isr_counter);
    if (isr_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID:
                PLIC_init();
                context_hart_id = IHC_context_to_context_hart_id(my_hart_id);
                switch(context_hart_id) {
                    case 1:
                        PLIC_SetPriority(IHCIA_hart1_INT, 2);
                        break;
                    case 2:
                        PLIC_SetPriority(IHCIA_hart2_INT, 2);
                        break;
                    case 3:
                        PLIC_SetPriority(IHCIA_hart3_INT, 2);
                        break;
                    case 4:
                        PLIC_SetPriority(IHCIA_hart4_INT, 2);
                        break;
                    default:
                        /*  Unsupported configuration value*/
                        break;
                }
                break;
            default:
                break;
        }
    }

    isr_counter++;

    env_unlock_mutex(platform_lock);

    return 0;
}

int32_t platform_deinit_interrupt(uint32_t vector_id)
{
	uint32_t context_hart_id = 0u;
    uint64_t my_hart_id = read_csr(mhartid);

    env_lock_mutex(platform_lock);

    RL_ASSERT(0 < isr_counter);
    isr_counter--;
    if (isr_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID:

                context_hart_id = IHC_context_to_context_hart_id(my_hart_id);

                switch(context_hart_id) {
                    case 1:
                        PLIC_DisableIRQ(IHCIA_hart1_INT);
                        break;
                    case 2:
                        PLIC_DisableIRQ(IHCIA_hart2_INT);
                        break;
                    case 3:
                        PLIC_DisableIRQ(IHCIA_hart3_INT);
                        break;
                    case 4:
                        PLIC_DisableIRQ(IHCIA_hart4_INT);
                        break;
                    default:
                        /*  Unsupported configuration value*/
                        break;
                }
                break;
            default:
                break;
        }
    }

    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    env_unlock_mutex(platform_lock);

    return 0;
}

#ifdef REMOTEPROC
void platform_ready(void)
{
    uint32_t ihc_tx_message[IHC_MAX_MESSAGE_SIZE];
    ihc_tx_message[0] = MIV_RP_MBOX_READY;

#ifdef USING_FREERTOS
    task_handle = xTaskGetCurrentTaskHandle();
#else
    ack_notify = 1;
#endif

#ifdef IHC_CHANNEL_SIDE_A
            (void)IHC_tx_message_from_context(IHC_CHANNEL_TO_CONTEXTB, (uint32_t *) &ihc_tx_message);
#else
            (void)IHC_tx_message_from_context(IHC_CHANNEL_TO_CONTEXTA, (uint32_t *) &ihc_tx_message);
#endif

#ifdef USING_FREERTOS
            xTaskNotifyWait(0, 0x0001, NULL, portMAX_DELAY);
#else
            while(ack_notify ==1);
#endif
}
#endif

void platform_notify(uint32_t vector_id)
{
    uint32_t tx_status;
    uint32_t ihc_tx_message[IHC_MAX_MESSAGE_SIZE];

    ihc_tx_message[0] = (uint32_t)(vector_id << 16);

#ifdef USING_FREERTOS
    task_handle = xTaskGetCurrentTaskHandle();
#endif

    switch (RL_GET_LINK_ID(vector_id))
    {
        case RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID:

            env_lock_mutex(platform_lock);
#ifndef USING_FREERTOS
            ack_notify = 1;
#endif
#ifdef IHC_CHANNEL_SIDE_A
            (void)IHC_tx_message_from_context(IHC_CHANNEL_TO_CONTEXTB,
                                              (uint32_t *) &ihc_tx_message);
#else
            (void)IHC_tx_message_from_context(IHC_CHANNEL_TO_CONTEXTA,
                                              (uint32_t *) &ihc_tx_message);
#endif
#ifdef USING_FREERTOS
            xTaskNotifyWait(0, 0x0001, NULL, portMAX_DELAY);
#else
            while(ack_notify ==1);
#endif
            env_unlock_mutex(platform_lock);
            break;
        default:
            break;
    }
}

void rpmsg_handler(bool is_ack, uint32_t msg)
{
    if(is_ack)
    {
#ifdef USING_FREERTOS
        (void)xTaskNotifyFromISR(task_handle, 0x0001, eSetBits, NULL);
#else
        ack_notify = 0;
#endif
        return;
    }

    switch(msg) {
        case MIV_RP_MBOX_STOP:
#ifdef REMOTEPROC
            rproc_stop();
#endif
            break;
        default:
            /* silently handle all other valid messages */
            if (msg >= MIV_RP_MBOX_READY && msg < MIV_RP_MBOX_END_MSG)
                return;
            env_isr((uint32_t) (msg >> 16));
    }
}

/**
 * platform_time_delay
 *
 * @param num_msec Delay time in ms.
 *
 * This is not an accurate delay, it ensures at least num_msec passed when return.
 */
void platform_time_delay(uint32_t num_msec)
{
#ifndef USING_FREERTOS
    SpinDelay_MilliSecs(num_msec);
#endif
}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */

extern int32_t in_ext_isr;
int32_t platform_in_isr(void)
{
    return in_ext_isr;

}

/**
 * platform_interrupt_enable
 *
 * Enable peripheral-related interrupt
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_enable(uint32_t vector_id)
{
	uint32_t context_hart_id = 0u;
    uint64_t my_hart_id = read_csr(mhartid);

    RL_ASSERT(0 < disable_counter);

    platform_global_isr_disable();
    disable_counter--;

    if (disable_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID:

                context_hart_id = IHC_context_to_context_hart_id(my_hart_id);

                switch(context_hart_id) {
                    case 1:
                        PLIC_EnableIRQ(IHCIA_hart1_INT);
                        break;
                    case 2:
                        PLIC_EnableIRQ(IHCIA_hart2_INT);
                        break;
                    case 3:
                        PLIC_EnableIRQ(IHCIA_hart3_INT);
                        break;
                    case 4:
                        PLIC_EnableIRQ(IHCIA_hart4_INT);
                        break;
                    default:
                        /*  Unsupported configuration value*/
                        break;
                }
                break;
            default:
                break;
        }
    }
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_interrupt_disable
 *
 * Disable peripheral-related interrupt.
 *
 * @param vector_id Virtual vector ID that needs to be converted to IRQ number
 *
 * @return vector_id Return value is never checked.
 *
 */
int32_t platform_interrupt_disable(uint32_t vector_id)
{
	uint32_t context_hart_id = 0u;
    uint64_t my_hart_id = read_csr(mhartid);

    RL_ASSERT(0 <= disable_counter);

    platform_global_isr_disable();

    if (disable_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID:
                context_hart_id = IHC_context_to_context_hart_id(my_hart_id);
                switch(context_hart_id) {
                    case 1:
                        PLIC_DisableIRQ(IHCIA_hart1_INT);
                        break;
                    case 2:
                        PLIC_DisableIRQ(IHCIA_hart2_INT);
                        break;
                    case 3:
                        PLIC_DisableIRQ(IHCIA_hart3_INT);
                        break;
                    case 4:
                        PLIC_DisableIRQ(IHCIA_hart4_INT);
                        break;
                    default:
                        /*  Unsupported configuration value*/
                        break;
                }
                break;
            default:
                break;
        }
    }
    disable_counter++;
    platform_global_isr_enable();
    return ((int32_t)vector_id);
}

/**
 * platform_map_mem_region
 *
 * Dummy implementation
 *
 */
void platform_map_mem_region(uint32_t vrt_addr, uint32_t phy_addr, uint32_t size, uint32_t flags)
{
}

/**
 * platform_cache_all_flush_invalidate
 *
 * Dummy implementation
 *
 */
void platform_cache_all_flush_invalidate(void)
{
}

/**
 * platform_cache_disable
 *
 * Dummy implementation
 *
 */
void platform_cache_disable(void)
{
}

/**
 * platform_vatopa
 *
 * Dummy implementation
 *
 */
uint64_t platform_vatopa(void *addr)
{
    return ((uint64_t *)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(uint64_t addr)
{
    return ((void *)addr);
}

/**
 * platform_init
 *
 * platform/environment init
 */
int32_t platform_init(void)
{
    uint64_t hartid = read_csr(mhartid);

    IHC_local_context_init((uint32_t)hartid);


    uint32_t remote_hart_id = IHC_partner_context_hart_id(hartid);

    IHC_local_remote_config((uint32_t)hartid, remote_hart_id, rx_handler, true, true);
    
    /* Create lock used in multi-instanced RPMsg */
    if (0 != env_create_mutex(&platform_lock, 1))
    {
        return -1;
    }

    return 0;
}

/**
 * platform_deinit
 *
 * platform/environment deinit process
 */
int32_t platform_deinit(void)
{
    /* Delete lock used in multi-instanced RPMsg */
    env_delete_mutex(platform_lock);
    platform_lock = ((void *)0);
    return 0;
}

/**
 *
 * CoreIHC Rx handler
 */
static uint32_t rx_handler( uint32_t remote_hart_id, uint32_t * message, uint32_t message_size, bool is_ack, uint32_t *message_storage_ptr )
{
    (void)remote_hart_id; /* message coming from here */

    if( is_ack == true )
    {
        rpmsg_handler(true, 0);
    }
    else
    {
        rpmsg_handler(false, (uint32_t) *message);
    }

    return(0U);
}
