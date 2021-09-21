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

void rpmsg_echo_demo_setup(rpmsg_comm_stack_handle_t handle)
{
    rpmsg_comm_stack_t *rpmsgHandle;
    rpmsgHandle = (rpmsg_comm_stack_t *)handle;

    rpmsgHandle->ctrl_ept = rpmsg_lite_create_ept(rpmsgHandle->my_rpmsg, RPMSG_ECHO_EPT_ADDR, rpmsg_queue_rx_cb, rpmsgHandle->ctrl_q);

#ifndef RPMSG_MASTER
    printf_to(UART_APP, "\r\nSending name service announcement\r\n");

    rpmsg_ns_announce(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, RPMSG_ECHO_CHANNEL_NAME, RL_NS_CREATE);
#endif
}

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

#ifdef RPMSG_MASTER
    /* send hello world handshake to remote processor */
    (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
        rpmsgHandle->remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);

    while(1)
    {
        wait_rx_message(handle);

        if(rnum >= MSG_LIMIT)
        {
            printf_to(UART_APP, "goodbye!\r\n");
            break;
        }

        (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
            rpmsgHandle->remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);
    }

#else
    /*receive hello world handshake message*/
    rpmsg_queue_recv(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_q, 
        (uint32_t *)&remote_addr, (char *)&buff, sizeof(buff), &len, RL_BLOCK);

    for (i = 1; i <= MSG_LIMIT; i++)
    {
        (void)rpmsg_lite_send(rpmsgHandle->my_rpmsg, rpmsgHandle->ctrl_ept, 
            remote_addr, HELLO_MSG, strlen(HELLO_MSG), RL_BLOCK);

        printf_to(UART_APP, "rpmsg sample test: message %d sent\r\n", i);

        if(i < MSG_LIMIT)
            wait_rx_message(handle);
    }
#endif

    printf_to(UART_APP, "**********************************\r\n");
    printf_to(UART_APP, " Test Results: Error count = %d\r\n", err_cnt);
    printf_to(UART_APP, "**********************************\r\n");

}
