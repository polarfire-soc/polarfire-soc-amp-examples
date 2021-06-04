#
# MPFS Embedded Software
#
# Copyright 2019 Microchip Corporation.
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
# FreeRTOS support

EXTRA_SRCS-y += \
	src/middleware/FreeRTOS/croutine.c \
	src/middleware/FreeRTOS/event_groups.c \
	src/middleware/FreeRTOS/list.c \
	src/middleware/FreeRTOS/queue.c \
	src/middleware/FreeRTOS/tasks.c \
	src/middleware/FreeRTOS/timers.c \
	src/middleware/FreeRTOS/portable/GCC/RISCV/port.c \
	src/middleware/FreeRTOS/portable/MemMang/heap_3.c \

ASM_SRCS += \
	src/middleware/FreeRTOS/portable/GCC/RISCV/portasm.S \


INCLUDES += \
	-Isrc/middleware/FreeRTOS/include \
	-Isrc/middleware/config \
	-Isrc/middleware/FreeRTOS/portable/GCC/RISCV \

src/middleware/FreeRTOS/croutine.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/event_groups.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/list.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/queue.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/tasks.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/timers.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/portable/GCC/RISCV/port.o: CFLAGS=$(CFLAGS_GCCEXT)
src/middleware/FreeRTOS/portable/MemMang/heap_3.o: CFLAGS=$(CFLAGS_GCCEXT)
