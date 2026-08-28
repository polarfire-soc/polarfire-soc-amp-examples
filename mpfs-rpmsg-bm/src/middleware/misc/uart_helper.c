/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Implementation of uart_putstring/g().
 * This is function is intended to be used from ee_printf().
 */

#include "drivers/mss/mss_mmuart/mss_uart.h"
#include <string.h>
#include <stdint.h>

#include "uart_helper.h"

static inline mss_uart_instance_t *get_uart_instance(int hartid)
{
    mss_uart_instance_t *pUart;

    switch (hartid) {
    case HART_E51:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART0_MASK)
        pUart = &g_mss_uart0_hi;
#else
        pUart = &g_mss_uart0_lo;
#endif
        break;
    case HART_U54_1:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART1_MASK)
        pUart = &g_mss_uart1_hi;
#else
        pUart = &g_mss_uart1_lo;
#endif
        break;
    case HART_U54_2:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART2_MASK)
        pUart = &g_mss_uart2_hi;
#else
        pUart = &g_mss_uart2_lo;
#endif
        break;
    case HART_U54_3:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART3_MASK)
        pUart = &g_mss_uart3_hi;
#else
        pUart = &g_mss_uart3_lo;
#endif
        break;
    case HART_U54_4:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART4_MASK)
        pUart = &g_mss_uart4_hi;
#else
        pUart = &g_mss_uart4_lo;
#endif
        break;
    default:
#if (LIBERO_SETTING_APBBUS_CR & APBBUS_CR_MMUART0_MASK)
        pUart = &g_mss_uart0_hi;
#else
        pUart = &g_mss_uart0_lo;
#endif
        break;
    }

    return pUart;
}

int uart_putstring(int hartid, char *p)
{
    const uint32_t len = (uint32_t)strlen(p);

    mss_uart_instance_t *pUart = get_uart_instance(hartid);
    MSS_UART_polled_tx_string(pUart, (const uint8_t *)p);
    return len;
}

int uart_putstring_to(mss_uart_instance_t *this_uart, char *p)
{
    const uint32_t len = (uint32_t)strlen(p);

    MSS_UART_polled_tx_string(this_uart, (const uint8_t *)p);
    return len;
}

void uart_putc(int hartid, const char ch)
{
    uint8_t string[2];
    string[0] = (uint8_t)ch;
    string[1] = 0u;

    mss_uart_instance_t *pUart = get_uart_instance(hartid);
    MSS_UART_polled_tx_string(pUart, (const uint8_t *)string);
}

