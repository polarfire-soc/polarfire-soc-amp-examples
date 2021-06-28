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
# Toplevel Makefile
#

# All Target
RISCV_TARGET:=mpfs-amp-freertos.elf
TARGET:=$(RISCV_TARGET)

all: ${TARGET}

ifneq ("$(wildcard src/boards/${BOARD}/Makefile)","")
  include src/boards/${BOARD}/Makefile
else
  ifndef BOARD
    $(info BOARD not specified) # default to icicle if nothing found
    BOARD:=icicle-kit-es
    include src/boards/${BOARD}/Makefile
  else
    $(error Board >>${BOARD}<< not found)
  endif
endif

LINKER_SCRIPT=src/boards/${BOARD}/platform_config/linker/mpfs-ddr-loaded-by-boot-loader.ld

RISCV_TARGET=mpfs-amp-freertos.elf
BINDIR=Default

ifeq ("$(wildcard $(BINDIR))","")
  $(info $(shell mkdir -p $(BINDIR)))
endif

MCMODEL=-mcmodel=medany

include rules.mk
include targets.mk
include src/platform/Makefile
include src/application/Makefile
include src/middleware/Makefile


DEPENDENCIES = $(addprefix $(BINDIR)/, $(SRCS:.c=.d))
.PHONY: dep
dep: $(DEPENDENCIES)

-include $(DEPENDENCIES)


$(RISCV_TARGET): $(OBJS)  $(DEPENDENCIES) $(LINKER_SCRIPT) $(LIBS)
	@$(ECHO) " LD    $@";
	$(CMD_PREFIX)$(CC) -T $(LINKER_SCRIPT) -nostdlib -nostartfiles -nodefaultlibs -Wl,-Map=$(BINDIR)/output.map -Wl,--gc-sections -o $(BINDIR)/$@ $(OBJS) $(LIBS)
	@$(ECHO) " NM        `basename $@ .elf`.sym";
	$(CMD_PREFIX)$(NM) -n $(BINDIR)/$@ > $(BINDIR)/`basename $@ .elf`.sym
	@$(ECHO) " BIN       `basename $@ .elf`.bin"
	$(CMD_PREFIX)$(OBJCOPY) -O binary $(BINDIR)/$@ $(BINDIR)/`basename $@ .elf`.bin
	@$(ECHO) " HEX       `basename $@ .elf`.hex";
	$(CMD_PREFIX)$(OBJCOPY) -O ihex $(BINDIR)/$@ $(BINDIR)/`basename $@ .elf`.hex
	$(CMD_PREFIX)$(SIZE) $(BINDIR)/$@ 2>/dev/null


