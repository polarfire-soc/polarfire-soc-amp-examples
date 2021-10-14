#
# MPFS Embedded Software
#
# Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#
#
# Baremetal Drivers and HAL Support

ASM_SRCS += \
	src/platform/mpfs_hal/startup_gcc/mss_entry.S \
 	src/platform/mpfs_hal/startup_gcc/mss_utils.S \
	src/platform/hal/hw_reg_access.S \

SRCS += \
	src/platform/mpfs_hal/startup_gcc/newlib_stubs.c \
	src/platform/mpfs_hal/startup_gcc/system_startup.c \
	src/platform/mpfs_hal/common/nwc/mss_cfm.c \
	src/platform/mpfs_hal/common/nwc/mss_ddr_debug.c \
	src/platform/mpfs_hal/common/nwc/mss_ddr_test_pattern.c \
	src/platform/mpfs_hal/common/nwc/mss_ddr.c \
	src/platform/mpfs_hal/common/nwc/mss_io.c \
	src/platform/mpfs_hal/common/nwc/mss_nwc_init.c \
	src/platform/mpfs_hal/common/nwc/mss_pll.c \
	src/platform/mpfs_hal/common/nwc/mss_sgmii.c \
	src/platform/mpfs_hal/common/mss_axiswitch.c \
	src/platform/mpfs_hal/common/mss_clint.c \
	src/platform/mpfs_hal/common/mss_h2f.c \
	src/platform/mpfs_hal/common/mss_irq_handler_stubs.c \
	src/platform/mpfs_hal/common/mss_l2_cache.c \
	src/platform/mpfs_hal/common/mss_mpu.c \
	src/platform/mpfs_hal/common/mss_mtrap.c \
	src/platform/mpfs_hal/common/mss_plic.c \
	src/platform/mpfs_hal/common/mss_peripherals.c \
	src/platform/mpfs_hal/common/mss_pmp.c \
	src/platform/mpfs_hal/common/mss_util.c \
	src/platform/hal/hal_irq.c \

SRCS += \
	src/platform/drivers/fpga_ip/miv_ihc/miv_ihc.c \
	src/platform/drivers/mss/mss_gpio/mss_gpio.c \
	src/platform/drivers/mss/mss_mmuart/mss_uart.c \
	
INCLUDES += \
	-Isrc/platform/drivers/fpga_ip/miv_ihc \
	-Isrc/platform/drivers/mss/mss_gpio \
	-Isrc/platform/drivers/mss/mss_mmuart \
	-Isrc/platform \
	-Isrc/platform/hal \
	-Isrc/platform/mpfs_hal/ \
	-Isrc/platform/mpfs_hal/common \
	-Isrc/platform/mpfs_hal/common/nwc \
	-Isrc/platform/mpfs_hal/startup_gcc \



