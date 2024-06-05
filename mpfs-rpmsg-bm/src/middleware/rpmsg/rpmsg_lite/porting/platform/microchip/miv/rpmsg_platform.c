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

#ifndef IHC_COM_ID_MASTER
#define IHC_COM_ID_MASTER RL_PLATFORM_MIV_IHC_CH8_ID
#endif

#ifndef IHC_COM_ID_REMOTE
#define IHC_COM_ID_REMOTE RL_PLATFORM_MIV_IHC_CH21_ID
#endif

#define IHC_APP_X_H0_IRQ  FABRIC_F2H_63_PLIC
#define IHC_APP_X_H1_IRQ  FABRIC_F2H_62_PLIC
#define IHC_APP_X_H2_IRQ  FABRIC_F2H_61_PLIC
#define IHC_APP_X_H3_IRQ  FABRIC_F2H_60_PLIC
#define IHC_APP_X_H4_IRQ  FABRIC_F2H_59_PLIC

static int32_t isr_counter0 = 0;
static int32_t isr_counter1 = 0;
static int32_t isr_counter2 = 0;
static int32_t isr_counter3 = 0;
static int32_t isr_counter4 = 0;

static int32_t disable_counter0 = 0;
static int32_t disable_counter1 = 0;
static int32_t disable_counter2 = 0;
static int32_t disable_counter3 = 0;
static int32_t disable_counter4 = 0;

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

static uint32_t QUEUE_IHC_MP_ISR_CALLBACK(uint8_t channel, const uint32_t *message, uint32_t message_size, uint32_t * ext_msg_ptr);
static uint32_t QUEUE_IHC_MC_ISR_CALLBACK(uint8_t channel, const uint32_t *message, uint32_t message_size, uint32_t * ext_msg_ptr);

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

    PLIC_init();

    switch (RL_GET_COM_ID(vector_id))
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            RL_ASSERT(0 <= isr_counter0);
            if (isr_counter0 == 0)
            {
                PLIC_SetPriority(IHC_APP_X_H0_IRQ,2);
            }
            isr_counter0++;
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            RL_ASSERT(0 <= isr_counter1);
            if (isr_counter1 == 0)
            {
               PLIC_SetPriority(IHC_APP_X_H1_IRQ,2);
            }
            isr_counter1++;
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            RL_ASSERT(0 <= isr_counter2);
            if (isr_counter2 == 0)
            {
                PLIC_SetPriority(IHC_APP_X_H2_IRQ,2);
            }
            isr_counter2++;
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            RL_ASSERT(0 <= isr_counter3);
            if (isr_counter3 == 0)
            {
               PLIC_SetPriority(IHC_APP_X_H3_IRQ,2);
            }
            isr_counter3++;
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            RL_ASSERT(0 <= isr_counter4);
            if (isr_counter4 == 0)
            {
               PLIC_SetPriority(IHC_APP_X_H4_IRQ,2);
            }
            isr_counter4++;
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
    }

    env_unlock_mutex(platform_lock);

    return 0;
}

int32_t platform_deinit_interrupt(uint32_t vector_id)
{
    env_lock_mutex(platform_lock);

    switch (RL_GET_COM_ID(vector_id))
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            RL_ASSERT(0 <= isr_counter0);
            isr_counter0--;
            if (isr_counter0 == 0)
            {
                PLIC_DisableIRQ(IHC_APP_X_H0_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            RL_ASSERT(0 <= isr_counter1);
            isr_counter1--;
            if (isr_counter1 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H1_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            RL_ASSERT(0 <= isr_counter2);
            isr_counter2--;
            if (isr_counter2 == 0)
            {
                PLIC_DisableIRQ(IHC_APP_X_H2_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            RL_ASSERT(0 <= isr_counter3);
            isr_counter3--;
            if (isr_counter3 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H3_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            RL_ASSERT(0 <= isr_counter4);
            isr_counter4--;
            if (isr_counter4 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H4_IRQ);
            }
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
    }

    /* Unregister ISR from environment layer */
    env_unregister_isr(vector_id);

    env_unlock_mutex(platform_lock);

    return 0;
}

#ifdef REMOTEPROC
void platform_ready(uint32_t link_id)
{
    uint32_t msg = MIV_RP_MBOX_READY;

#ifdef USING_FREERTOS
    task_handle = xTaskGetCurrentTaskHandle();
#else
    ack_notify = 1;
#endif

(void)IHC_tx_message(RL_GET_COM_ID_FROM_LINK_ID(link_id), (uint32_t *) &msg, sizeof(msg));

#ifdef USING_FREERTOS
            xTaskNotifyWait(0, 0x0001, NULL, portMAX_DELAY);
/* #else
            while(ack_notify ==1); */
#endif
}
#endif

void platform_notify(uint32_t vector_id)
{
    /* Only vring id and queue id is needed in msg */
    uint32_t msg = RL_GEN_MU_MSG(vector_id);

#ifdef USING_FREERTOS
    task_handle = xTaskGetCurrentTaskHandle();
#endif

    env_lock_mutex(platform_lock);

#ifndef USING_FREERTOS
            ack_notify = 1;
#endif

    IHC_tx_message(RL_GET_COM_ID(vector_id), (uint32_t *) &msg, sizeof(msg));

#ifdef USING_FREERTOS
            xTaskNotifyWait(0, 0x0001, NULL, portMAX_DELAY);
#else 
            while(ack_notify ==1);
#endif

    env_unlock_mutex(platform_lock);
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
    platform_global_isr_disable();

    switch (RL_GET_COM_ID(vector_id))
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            RL_ASSERT(0 <= disable_counter0);
            disable_counter0--;
            if (disable_counter0 == 0)
            {
                PLIC_EnableIRQ(IHC_APP_X_H0_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            RL_ASSERT(0 <= disable_counter1);
            disable_counter1--;
            if (disable_counter1 == 0)
            {
               PLIC_EnableIRQ(IHC_APP_X_H1_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            RL_ASSERT(0 <= disable_counter2);
            disable_counter2--;
            if (disable_counter2 == 0)
            {
                PLIC_EnableIRQ(IHC_APP_X_H2_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            RL_ASSERT(0 <= disable_counter3);
            disable_counter3--;
            if (disable_counter3 == 0)
            {
               PLIC_EnableIRQ(IHC_APP_X_H3_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            RL_ASSERT(0 <= disable_counter4);
            disable_counter4--;
            if (disable_counter4 == 0)
            {
               PLIC_EnableIRQ(IHC_APP_X_H4_IRQ);
            }
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
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
    platform_global_isr_disable();

    switch (RL_GET_COM_ID(vector_id))
    {
        case RL_PLATFORM_MIV_IHC_CH0_ID:
        case RL_PLATFORM_MIV_IHC_CH1_ID:
        case RL_PLATFORM_MIV_IHC_CH2_ID:
        case RL_PLATFORM_MIV_IHC_CH3_ID:
        case RL_PLATFORM_MIV_IHC_CH4_ID:
            RL_ASSERT(0 <= disable_counter0);
            disable_counter0++;
            if (disable_counter0 == 0)
            {
                PLIC_DisableIRQ(IHC_APP_X_H0_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH5_ID:
        case RL_PLATFORM_MIV_IHC_CH6_ID:
        case RL_PLATFORM_MIV_IHC_CH7_ID:
        case RL_PLATFORM_MIV_IHC_CH8_ID:
        case RL_PLATFORM_MIV_IHC_CH9_ID:
            RL_ASSERT(0 <= disable_counter1);
            disable_counter1++;
            if (disable_counter1 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H1_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH10_ID:
        case RL_PLATFORM_MIV_IHC_CH11_ID:
        case RL_PLATFORM_MIV_IHC_CH12_ID:
        case RL_PLATFORM_MIV_IHC_CH13_ID:
        case RL_PLATFORM_MIV_IHC_CH14_ID:
            RL_ASSERT(0 <= disable_counter2);
            disable_counter2++;
            if (disable_counter2 == 0)
            {
                PLIC_DisableIRQ(IHC_APP_X_H2_IRQ);
            }
        case RL_PLATFORM_MIV_IHC_CH15_ID:
        case RL_PLATFORM_MIV_IHC_CH16_ID:
        case RL_PLATFORM_MIV_IHC_CH17_ID:
        case RL_PLATFORM_MIV_IHC_CH18_ID:
        case RL_PLATFORM_MIV_IHC_CH19_ID:
            RL_ASSERT(0 <= disable_counter3);
            disable_counter3++;
            if (disable_counter3 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H3_IRQ);
            }
            break;
        case RL_PLATFORM_MIV_IHC_CH20_ID:
        case RL_PLATFORM_MIV_IHC_CH21_ID:
        case RL_PLATFORM_MIV_IHC_CH22_ID:
        case RL_PLATFORM_MIV_IHC_CH23_ID:
        case RL_PLATFORM_MIV_IHC_CH24_ID:
            RL_ASSERT(0 <= disable_counter4);
            disable_counter4++;
            if (disable_counter4 == 0)
            {
               PLIC_DisableIRQ(IHC_APP_X_H4_IRQ);
            }
            break;
        default:
            /* All the cases have been listed above, the default clause should not be reached. */
            break;
    }

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
#ifdef RPMSG_MASTER
    IHC_init(IHC_COM_ID_MASTER);
    IHC_config_mp_callback_handler(IHC_COM_ID_MASTER, QUEUE_IHC_MP_ISR_CALLBACK);
    IHC_config_mc_callback_handler(IHC_COM_ID_MASTER, QUEUE_IHC_MC_ISR_CALLBACK);
    IHC_enable_mp_interrupt(IHC_COM_ID_MASTER);
    IHC_enable_mc_interrupt(IHC_COM_ID_MASTER);
#else
    IHC_init(IHC_COM_ID_REMOTE);
    IHC_config_mp_callback_handler(IHC_COM_ID_REMOTE, QUEUE_IHC_MP_ISR_CALLBACK);
    IHC_config_mc_callback_handler(IHC_COM_ID_REMOTE, QUEUE_IHC_MC_ISR_CALLBACK);
    IHC_enable_mp_interrupt(IHC_COM_ID_REMOTE);
    IHC_enable_mc_interrupt(IHC_COM_ID_REMOTE); //ojo
#endif 

    SpinDelay_MilliSecs(100);

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

static uint32_t QUEUE_IHC_MP_ISR_CALLBACK(uint8_t channel,
                           const uint32_t *message,
                           uint32_t message_size, uint32_t * ext_msg_ptr)
{
    uint32_t msg = message[0];

    switch(msg) {
         case MIV_RP_MBOX_STOP:
#ifdef REMOTEPROC
            rproc_stop(channel);
#endif
            break;
        default:
            if (msg >= MIV_RP_MBOX_READY && msg < MIV_RP_MBOX_END_MSG)
                return 0;
            env_isr((uint32_t)((msg) | (channel << 3)));
    }

    return 0;
}


static uint32_t QUEUE_IHC_MC_ISR_CALLBACK(uint8_t channel,
                           const uint32_t *message,
                           uint32_t message_size, uint32_t * ext_msg_ptr)
{

#ifdef USING_FREERTOS
        (void)xTaskNotifyFromISR(task_handle, 0x0001, eSetBits, NULL);
#else
        ack_notify = 0;
#endif
        return 0;
}

