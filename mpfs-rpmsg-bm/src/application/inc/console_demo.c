/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * RPMsg console demo
 * 
 * Example application demonstrating how to communicate with another software
 * context over the RPMsg bus.
 * 
 * The software context configured as RPMsg master receives strings from the
 * UART1 console and sends them to the software context configured as RPMsg
 * remote using the RPMsg framework.
 * 
 * The software context configured as RPMsg remote receives the messages 
 * and prints them on the UART3 console.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "inc/demo_main.h"

#ifdef RPMSG_MASTER
#define RPMSG_CONSOLE_EPT_ADDR (3U)
#else
#define RPMSG_CONSOLE_EPT_ADDR (4u)
#endif

#define RPMSG_TTY_CHANNEL_NAME "rpmsg-tty"

#define UART_RX_BUFF_SIZE    2U
#define RPMSG_RX_MAX_BUFF_SIZE    256U

static uint8_t g_rx_buff[UART_RX_BUFF_SIZE] = { 0 };
static volatile uint8_t g_rx_size = 0U;
static bool console_initialized = false;
static bool message_available = false;

/* This function builds a string from characters read from the UART console
 */
void uart_get_message(char *dest_buffer)
{
    while (!message_available)
    {
        g_rx_size = MSS_UART_get_rx(UART_APP, g_rx_buff, sizeof(g_rx_buff));
        if (g_rx_size > 0u) {

            if(g_rx_buff[0u] != '\r')
            {
                MSS_UART_polled_tx(UART_APP, g_rx_buff, g_rx_size);
                strcat(dest_buffer,g_rx_buff);
            }
            else
            {
                message_available = true;
            }
            g_rx_size = 0u;
        }
    }
}

/* This function creates the RPMsg endpoint for the console demo.
 * When in master mode, it sends a name service announcement to the software
   context configured as RPMsg remote to announce the existance of the console
   service.
 */
void rpmsg_console_demo_setup(rpmsg_comm_stack_handle_t handle)
{
    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    rpmsgHandle->ctrl_ept = rpmsg_lite_create_ept(rpmsgHandle->my_rpmsg, RPMSG_CONSOLE_EPT_ADDR, 
                                                    rpmsg_queue_rx_cb, rpmsgHandle->ctrl_q);
}

/* This is the main function for the console application demo.
 * 
 * The software context configured as RPMsg master receives strings from the
 * UART1 console and sends them to the software context configured as RPMsg
 * remote using the RPMsg framework.
 * 
 * The software context configured as RPMsg remote receives the messages 
 * and prints them on the UART3 console.
 */
void rpmsg_console_demo(rpmsg_comm_stack_handle_t handle)
{
    char buff[RPMSG_RX_MAX_BUFF_SIZE];
    uint32_t remote_addr;
    uint8_t quit_message;
    uint32_t len;
    rpmsg_comm_stack_t *rpmsgHandle;

    rpmsgHandle = (rpmsg_comm_stack_t *)handle;
    quit_message = 0;

    if (!console_initialized) {
        rpmsg_console_demo_setup(handle);
        console_initialized = true;
    }
    else
    {
        printf_to(UART_APP, "\r\nEnd of tty/console demo. Press 0 to show menu\r\n");
        return;
    }

#ifndef RPMSG_MASTER
    MSS_UART_polled_tx_string(UART_APP, "\r\nSending name service announcement\r\n");
    printf_to(UART_APP, "\r\nPlease run console demo on the RPMsg master context\r\n\r\n");
    rpmsg_ns_announce(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, RPMSG_TTY_CHANNEL_NAME, RL_NS_CREATE);
#endif

#ifdef RPMSG_MASTER
    printf_to(UART_APP, "\r\nPlease run console demo on the RPMsg remote context\r\n\r\n");
#endif

#ifdef RPMSG_MASTER
    while(rpmsgHandle->remote_addr != 4);
#endif

    strcpy(buff,"");

#ifdef RPMSG_MASTER

    MSS_UART_polled_tx_string(UART_APP, "\r\nEnter message to send to remote or type quit to exit:");

    while (!quit_message)
    {
        uart_get_message(buff);

        if (message_available) {

            if(!strncmp(buff, "quit", strlen("quit")))
            {
                quit_message = 1;
                printf_to(UART_APP, "\r\nEnd of console/tty demo. Press 0 to show menu\r\n");
            }

            (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
                                    rpmsgHandle->remote_addr, buff, strlen(buff), RL_BLOCK);
            MSS_UART_polled_tx_string(UART_APP, "\r\n");

            strcpy(buff,"");
            message_available = false;
        }
    }

#else
    while(!quit_message)
    {
        rpmsg_queue_recv(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_q, 
                            (uint32_t *)&remote_addr, (char *)&buff, 
                            sizeof(buff), &len, RL_BLOCK);
        {
            MSS_UART_polled_tx(UART_APP, buff, len);
            MSS_UART_polled_tx_string(UART_APP, "\r\n");

            if(!strncmp(buff, "quit", len))
            {
                quit_message = 1;
                printf_to(UART_APP, "\r\nEnd of console/tty demo. Press 0 to show menu\r\n");
            }
        }
    }
#endif
}
