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

/*******************************************************************************
 * Include files
 */

#include <stdio.h>
#include <string.h>
#include "inc/demo_main.h"

#define RPMSG_RX_MAX_BUFF_SIZE    256U

/*******************************************************************************
 * Macros
 */
#define RPMSG_SHARED_MEMORY_BASE  ((void*)0x1075FB0000)
#define RPMSG_SHARED_MEMORY_SIZE (0x50000)

/*******************************************************************************
 * Instance definition
 */
static void* rpmsg_lite_base = RPMSG_SHARED_MEMORY_BASE;

/*******************************************************************************
 * Function prototypes
 */
static void rpmsg_setup(rpmsg_comm_stack_handle_t handle);
void clear_rpmsg_buffer(rpmsg_comm_stack_handle_t handle);

#ifdef RPMSG_MASTER
static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data);
#endif

/*******************************************************************************
 * Functions
 *******************************************************************************/
void start_demo()
{
    uint8_t rx_buff[1];
    uint8_t rx_size = 0;

    rpmsg_comm_stack_t my_rpmsg_instance;

#ifdef RPMSG_MASTER
    const uint8_t g_message[] =
    "\r\n\r\n\r\n **** PolarFire SoC Icicle Kit AMP RPMsg Master Bare Metal Example ****\r\n\r\n\r\n";
#else
    const uint8_t g_message[] =
    "\r\n\r\n\r\n **** PolarFire SoC Icicle Kit AMP RPMsg Remote Bare Metal Example ****\r\n\r\n\r\n";
#endif

    const uint8_t g_menu[] =
    "\r\n\
    Press 0 to show this menu\r\n\
    Press 1 to run ping pong demo\r\n\
    Press 2 to run console demo\r\n\
    Press 3 to run sample echo demo\r\n";

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

#ifdef RPMSG_MASTER
    (void)mss_config_clk_rst(MSS_PERIPH_MMUART1, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
#else
    (void)mss_config_clk_rst(MSS_PERIPH_MMUART3, (uint8_t) MPFS_HAL_FIRST_HART, PERIPHERAL_ON);
#endif

    PLIC_init();

    MSS_UART_init(UART_APP, MSS_UART_115200_BAUD,
                   MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY);

    MSS_UART_polled_tx(UART_APP, g_message, sizeof(g_message));

	rpmsg_setup(&my_rpmsg_instance);

    MSS_UART_polled_tx(UART_APP, g_menu, sizeof(g_menu));

    while(1)
    {
        rx_size = MSS_UART_get_rx(UART_APP, rx_buff, sizeof(rx_buff));
        if (rx_size > 0)
        {
#ifndef RPMSG_MASTER
            clear_rpmsg_buffer(&my_rpmsg_instance);
#endif
            switch(rx_buff[0])
            {
                case '0':
                    MSS_UART_polled_tx(UART_APP, g_menu, sizeof(g_menu));
                    break;
                case '1':
                    rpmsg_pingpong_demo(&my_rpmsg_instance);
                    break;
                case '2':
                    rpmsg_console_demo(&my_rpmsg_instance);
                    break;
                case '3':
                    rpmsg_echo_demo(&my_rpmsg_instance);
                    break;
                default:
                    break;
            }
        }
    }

    rpmsg_lite_destroy_ept(my_rpmsg_instance.my_rpmsg, my_rpmsg_instance.ctrl_ept);
    rpmsg_queue_destroy(my_rpmsg_instance.my_rpmsg, my_rpmsg_instance.ctrl_q);
    rpmsg_lite_deinit(my_rpmsg_instance.my_rpmsg);

}

void clear_rpmsg_buffer(rpmsg_comm_stack_handle_t handle)
{
    uint32_t remote_addr;
    uint32_t len;
    char buff[RPMSG_RX_MAX_BUFF_SIZE];

    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    while(rpmsg_queue_get_current_size(rpmsgHandle->ctrl_q) > 0)
    {
        /* free the rpmsg buffer */
        rpmsg_queue_recv(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_q, 
                (uint32_t *)&remote_addr, (char *)&buff, sizeof(buff), &len, RL_BLOCK);
    }
}

void rpmsg_setup(rpmsg_comm_stack_handle_t handle)
{
    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    rpmsgHandle->remote_addr = 0xFFFFFFFF;

#ifdef RPMSG_MASTER

    /* RPMsg Master Mode */
    rpmsgHandle->my_rpmsg = rpmsg_lite_master_init(rpmsg_lite_base, RPMSG_SHARED_MEMORY_SIZE, RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID, RL_NO_FLAGS);
    rpmsgHandle->ctrl_q = rpmsg_queue_create(rpmsgHandle->my_rpmsg);
    rpmsgHandle->ns_handle = rpmsg_ns_bind(rpmsgHandle->my_rpmsg, app_nameservice_isr_cb, (void *)&(rpmsgHandle->remote_addr));
    MSS_UART_polled_tx_string(UART_APP, "\r\nWaiting for remote to send name service announcement..\r\n");

    /* Wait until the second context issues the nameservice isr and the remote endpoint address is known. */
    while (0xFFFFFFFF == rpmsgHandle->remote_addr);
#else
    /* RPMsg Remote Mode */
    MSS_UART_polled_tx_string(UART_APP, "\r\nWaiting for master to get ready...\r\n");

    rpmsgHandle->my_rpmsg = rpmsg_lite_remote_init(rpmsg_lite_base, RL_PLATFORM_MIV_IHC_CONTEXT_A_B_LINK_ID, RL_NO_FLAGS);
    rpmsgHandle->ctrl_q = rpmsg_queue_create(rpmsgHandle->my_rpmsg);

    while(!rpmsg_lite_is_link_up(rpmsgHandle->my_rpmsg));

    MSS_UART_polled_tx_string(UART_APP, "\r\nMaster is ready\r\n");
#endif
}

#ifdef RPMSG_MASTER
static void app_nameservice_isr_cb(uint32_t new_ept, const char *new_ept_name, uint32_t flags, void *user_data)
{
    uint32_t *data = (uint32_t *)user_data;
    *data = new_ept;
}
#endif


