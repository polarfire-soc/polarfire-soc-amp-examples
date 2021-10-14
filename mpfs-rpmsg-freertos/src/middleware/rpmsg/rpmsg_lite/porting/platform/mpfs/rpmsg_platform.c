/*
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 * Copyright 2016-2019 NXP
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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#if defined(RL_USE_ENVIRONMENT_CONTEXT) && (RL_USE_ENVIRONMENT_CONTEXT == 1)
#error "This RPMsg-Lite port requires RL_USE_ENVIRONMENT_CONTEXT set to 0"
#endif

static int32_t isr_counter     = 0;
static int32_t disable_counter = 0;
static void *platform_lock;

static xTaskHandle task_handle;
static uint32_t rx_handler(uint32_t remote_hart_id, uint32_t * message, uint32_t message_size, bool is_ack, uint32_t *message_storage_ptr );
static void rpmsg_handler(bool is_ack, uint32_t vring_idx);

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

    /* Register ISR to environment layer */
    env_register_isr(vector_id, isr_data);

    env_lock_mutex(platform_lock);

    RL_ASSERT(0 <= isr_counter);
    if (isr_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MPFS_CONTEXT_A_B_LINK_ID:
                PLIC_init();
#ifdef IHC_CHANNEL_SIDE_A
                PLIC_SetPriority(IHCIA_hart1_INT, 2);
#else
                PLIC_SetPriority(IHCIA_hart4_INT, 2);
#endif
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
    env_lock_mutex(platform_lock);

    RL_ASSERT(0 < isr_counter);
    isr_counter--;
    if (isr_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MPFS_CONTEXT_A_B_LINK_ID:
#ifdef IHC_CHANNEL_SIDE_A
                PLIC_DisableIRQ(IHCIA_hart1_INT);
#else
                PLIC_DisableIRQ(IHCIA_hart4_INT);
#endif
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

void platform_notify(uint32_t vector_id)
{
    uint32_t tx_status;
    uint32_t ihc_tx_message[IHC_MAX_MESSAGE_SIZE];

    ihc_tx_message[0] = (uint32_t)(vector_id << 16);

    task_handle = xTaskGetCurrentTaskHandle();

    switch (RL_GET_LINK_ID(vector_id))
    {
        case RL_PLATFORM_MPFS_CONTEXT_A_B_LINK_ID:

            env_lock_mutex(platform_lock);
#ifdef IHC_CHANNEL_SIDE_A
            (void)IHC_tx_message(IHC_CHANNEL_TO_CONTEXTB, (uint32_t *) &ihc_tx_message);
#else
            (void)IHC_tx_message(IHC_CHANNEL_TO_CONTEXTA, (uint32_t *) &ihc_tx_message);
#endif
            xTaskNotifyWait(0, 0x0001, NULL, portMAX_DELAY);
            env_unlock_mutex(platform_lock);
            break;
        default:
            break;
    }
}

void rpmsg_handler(bool is_ack, uint32_t vring_idx)
{
    if(is_ack)
    {
        (void)xTaskNotifyFromISR(task_handle, 0x0001, eSetBits, NULL);
    }
    else
    {
        env_isr((uint32_t) (vring_idx >> 16));
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

}

/**
 * platform_in_isr
 *
 * Return whether CPU is processing IRQ
 *
 * @return True for IRQ, false otherwise.
 *
 */
int32_t platform_in_isr(void)
{
    int32_t ret = 0;

    volatile uintptr_t mcause = read_csr(mcause);

    if(((mcause & MCAUSE_INT) == MCAUSE_INT) && ((mcause & MCAUSE_CAUSE)  == IRQ_M_EXT))
    {
        ret = 1;
    }

    return ret;
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
    RL_ASSERT(0 < disable_counter);

    platform_global_isr_disable();
    disable_counter--;

    if (disable_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MPFS_CONTEXT_A_B_LINK_ID:
#ifdef IHC_CHANNEL_SIDE_A
                PLIC_EnableIRQ(FABRIC_F2H_62_PLIC);
#else
                PLIC_EnableIRQ(FABRIC_F2H_59_PLIC);
#endif
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
    RL_ASSERT(0 <= disable_counter);

    platform_global_isr_disable();

    if (disable_counter == 0)
    {
        switch (RL_GET_LINK_ID(vector_id))
        {
            case RL_PLATFORM_MPFS_CONTEXT_A_B_LINK_ID:
#ifdef IHC_CHANNEL_SIDE_A
                PLIC_DisableIRQ(IHCIA_hart1_INT);
#else
                PLIC_DisableIRQ(IHCIA_hart4_INT);
#endif
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
uint32_t platform_vatopa(void *addr)
{
    return ((uint32_t)(char *)addr);
}

/**
 * platform_patova
 *
 * Dummy implementation
 *
 */
void *platform_patova(uint32_t addr)
{
    return ((void *)(char *)addr);
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

#ifdef IHC_CHANNEL_SIDE_A
        uint32_t remote_hart_id = CONTEXTB_HARTID;
#else
        uint32_t remote_hart_id = CONTEXTA_HARTID;
#endif
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
