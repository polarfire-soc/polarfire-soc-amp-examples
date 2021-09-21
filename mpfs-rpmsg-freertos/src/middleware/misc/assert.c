/*******************************************************************************
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions..
 *
 * SPDX-License-Identifier: MIT
 *
 * MPFS Embedded Software
 *
 */

/*!
 * \file assert implementation
 * \brrief Local assert
 */
#include <stdlib.h>

/*!
 * \brief Local implemention of assert fail
 */
void __assert_func(const char *__file, unsigned int __line,
        const char *__function, const char *__assertion) //__attribute__ ((__noreturn__))
{
#ifndef __riscv
    exit(1);
#else
    while(1);
#endif
}

void __assert_fail(const char *__file, unsigned int __line,
        const char *__function, const char *__assertion) //__attribute__ ((__noreturn__))
{
   __assert_func(__file, __line, __function, __assertion);
}

