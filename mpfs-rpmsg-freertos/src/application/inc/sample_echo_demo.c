/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * RPMsg sample echo demo 
 * 
 * Example application demonstrating how to communicate with another software
 * context over the RPMsg bus.
 * 
 * When in RPMsg remote mode, this application sends a configurable number of
 * hello messages to the software context configured as RPMsg master 
 * 
 * When in RPMsg master mode, this application received and echoes back messages
 * received by the software context configured as RPMsg master.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "inc/demo_main.h"

#ifdef RPMSG_MASTER
#define RPMSG_ECHO_EPT_ADDR (5U)
#else
#define RPMSG_ECHO_EPT_ADDR (6u)
#endif

#define RPMSG_ECHO_CHANNEL_NAME "rpmsg-client-sample"
#define RPMSG_RX_MAX_BUFF_SIZE    256U

#define HELLO_MSG	"hello world!"
#define MSG_LIMIT	100

static bool echo_initialized = false;
static int rnum = 0;
static int err_cnt = 0;
static char buff[RPMSG_RX_MAX_BUFF_SIZE];

/* This function creates the RPMsg endpoint for the sample echo demo.
 * When in master mode, it sends a name service announcement to the software
   context configured as RPMsg remote to announce the existance of the echo demo
   service.
 */
void rpmsg_echo_demo_setup(rpmsg_comm_stack_handle_t handle)
{
    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    rpmsgHandle->ctrl_ept = rpmsg_lite_create_ept(rpmsgHandle->my_rpmsg, RPMSG_ECHO_EPT_ADDR, rpmsg_queue_rx_cb, rpmsgHandle->ctrl_q);
}

/* This is a blocking function that receives a message using the
 * RPMsg framework and then validates that the data received is a 
 * hello world message string
 */
void wait_rx_message(rpmsg_comm_stack_handle_t handle)
{
    uint32_t remote_addr;
    uint32_t len;

    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    rpmsg_queue_recv(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_q, 
        (uint32_t *)&remote_addr, (char *)&buff, sizeof(buff), &len, RL_BLOCK);

    printf_to(UART_APP, "received message %d: ", rnum + 1);
    MSS_UART_polled_tx(UART_APP, buff, len);
    printf_to(UART_APP, " of size %lu \r\n", (unsigned long) len);

    if (strncmp(buff, HELLO_MSG, len)) {
        printf_to(UART_APP, " Invalid message is received.\r\n");
        err_cnt++;
        return;
    }

    rnum++;
}

/* This is the main function for the sample echo application demo.
 * 
 * When in RPMsg remote mode, this application sends a configurable number of
 * hello messages to the software context configured as RPMsg master 
 * 
 * When in RPMsg master mode, this application received and echoes back messages
 * received by the software context configured as RPMsg master.
 */
void rpmsg_echo_demo(rpmsg_comm_stack_handle_t handle)
{
    uint32_t remote_addr;
    uint32_t len;
    int i;

    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    if (!echo_initialized) {
        rpmsg_echo_demo_setup(handle);
        echo_initialized = true;
    }
    else
    {
        printf_to(UART_APP, "\r\nEnd of sample echo demo. Press 0 to show menu\r\n");
        return;
    }

#ifndef RPMSG_MASTER
        printf_to(UART_APP, "\r\nSending name service announcement\r\n");
        printf_to(UART_APP, "\r\nPlease run sample echo demo on the RPMsg master context\r\n\r\n");
        rpmsg_ns_announce(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, RPMSG_ECHO_CHANNEL_NAME, RL_NS_CREATE);
#endif

#ifdef RPMSG_MASTER
    printf_to(UART_APP, "\r\nPlease run sample echo on the RPMsg remote context\r\n\r\n");
#endif

#ifdef RPMSG_MASTER
    while(rpmsgHandle->remote_addr != 6);
#endif

#ifdef RPMSG_MASTER
        /* send hello world handshake to remote processor */
        (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
            rpmsgHandle->remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);
#else
        /*receive hello world handshake message*/
        rpmsg_queue_recv(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_q, 
            (uint32_t *)&remote_addr, (char *)&buff, sizeof(buff), &len, RL_BLOCK);

        rpmsgHandle->remote_addr = remote_addr;
#endif

#ifdef RPMSG_MASTER

    while(1)
    {
        if(rnum >= MSG_LIMIT)
        {
            printf_to(UART_APP, "End of sample echo demo. Press 0 to show menu\r\n");
            return;
        }

        wait_rx_message(handle);

        vTaskDelay(1);

        (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
            rpmsgHandle->remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);
    }

#else

    if(rnum >= (MSG_LIMIT -1 ))
    {
            printf_to(UART_APP, "End of sample echo demo. Press 0 to show menu\r\n");
            return;
    }

    for (i = 1; i <= MSG_LIMIT; i++)
    {
        vTaskDelay(1);

        (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
            rpmsgHandle->remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);

        printf_to(UART_APP, "rpmsg sample test: message %d sent\r\n", i);

        if(i < MSG_LIMIT)
        {
            wait_rx_message(handle);
        }
    }
#endif

    printf_to(UART_APP, "**********************************\r\n");
    printf_to(UART_APP, " Test Results: Error count = %d\r\n", err_cnt);
    printf_to(UART_APP, "**********************************\r\n");
    printf_to(UART_APP, "\r\nEnd of sample echo demo. Press 0 to show menu\r\n");

}
