/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "uart_helper.h"
#include "ee_vsprintf.h"

#include "rpmsg_lite.h"
#include "rpmsg_queue.h"
#include "rpmsg_ns.h"

#define printf_to(...) ee_printf_to(__VA_ARGS__)

#ifdef RPMSG_MASTER
#define UART_APP &g_mss_uart2_lo
#else
#define UART_APP &g_mss_uart3_lo
#endif

typedef void *rpmsg_comm_stack_handle_t;

/*! @brief rpmsg state structure. */
typedef struct _rpmsg_comm_stack
{
    struct rpmsg_lite_endpoint *ctrl_ept;
    rpmsg_queue_handle ctrl_q;
    struct rpmsg_lite_instance *my_rpmsg;
    rpmsg_ns_handle ns_handle;
    volatile uint32_t remote_addr;
} rpmsg_comm_stack_t;

/**
 * functions
 */
void start_demo(void);

void rpmsg_pingpong_demo_setup(rpmsg_comm_stack_handle_t handle);
void rpmsg_pingpong_demo(rpmsg_comm_stack_handle_t handle);

void rpmsg_console_demo_setup(rpmsg_comm_stack_handle_t handle);
void rpmsg_console_demo(rpmsg_comm_stack_handle_t handle);

void rpmsg_echo_demo_setup(rpmsg_comm_stack_handle_t handle);
void rpmsg_echo_demo(rpmsg_comm_stack_handle_t handle);

#endif /* APPLICATION_H_ */
