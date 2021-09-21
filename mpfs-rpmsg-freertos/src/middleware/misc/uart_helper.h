#ifndef UART_HELPER_H
#define UART_HELPER_H

/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *
 * Implementation of UART helper routines
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mpfs_hal/mss_hal.h"

/**
 * \brief HSS Cores Enumeration
 */
enum HartId {
    HART_E51 = 0,
    HART_U54_1,
    HART_U54_2,
    HART_U54_3,
    HART_U54_4,

    HART_NUM_PEERS,
    HART_ALL = HART_NUM_PEERS
};


int uart_putstring(int hartid, char *p);
int uart_putstring_to(mss_uart_instance_t *this_uart, char *p);
void uart_putc(int hartid, const char ch);

#ifdef __cplusplus
}
#endif

#endif
