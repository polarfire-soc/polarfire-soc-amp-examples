#
# MPFS Embedded Software
#
# Copyright 2019 Microchip FPGA Embedded Systems Solutions.
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
# Application code

all: $(SOC_CONFIG_FILES) $(TARGET)

SRCS += \
	src/application/hart0/e51.c \
	src/application/hart1/u54_1.c \
	src/application/hart2/u54_2.c \
	src/application/hart3/u54_3.c \
	src/application/hart4/u54_4.c \
	src/application/inc/demo_main.c \
	src/application/inc/pingpong_demo.c \
	src/application/inc/console_demo.c \
	src/application/inc/sample_echo_demo.c \

ifdef REMOTEPROC
SRCS += src/application/inc/rsc_table.c
endif

INCLUDES += \
	-Isrc/application \

