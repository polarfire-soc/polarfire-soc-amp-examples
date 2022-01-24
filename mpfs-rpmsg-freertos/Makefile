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

SHELL=/bin/sh
PYTHON?=python3

#
# To build the HSS under SoftConsole on Windows, we need to use SoftConsole-provided
# tools, and potentially to modify paths
#

.ONESHELL:

ifeq ($(OS), Windows_NT)
  #If we need to patch up path for Windows, we could do it here...
  #TOOLPATH:=${SC_INSTALL_DIR}riscv-unknown-elf-gcc\bin
  #export PATH:="$(TOOLPATH);$(PATH)"
  $(info INFO: Windows detected)
  HOST_WINDOWS:=true
else
  SYSTEM:=$(shell uname -s)
  ifneq (, $(findstring Linux, $(SYSTEM)))         # Linux-specific mods
    # Nothing special needed
    $(info INFO: Linux detected)
    HOST_LINUX:=true
  else
    $(error Unsupported build platform $(SYSTEM))
  endif
endif

include src/application/Makefile

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

ifeq ("$(wildcard $(BINDIR))","")
  $(info $(shell mkdir -p $(BINDIR)))
endif

MCMODEL=-mcmodel=medany

include src/application/rules.mk
include src/application/targets.mk
include src/platform/Makefile
include src/middleware/Makefile

DEPENDENCIES = $(addprefix $(BINDIR)/, $(SRCS:.c=.d))
.PHONY: dep
dep: $(DEPENDENCIES)

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDENCIES)
endif

$(RISCV_TARGET): $(OBJS)  $(DEPENDENCIES) $(LINKER_SCRIPT)
	@$(ECHO) " LD    $@";
	$(CMD_PREFIX)$(CC) -T $(LINKER_SCRIPT) -static -nostdlib -nostartfiles -nodefaultlibs -Wl,-Map=$(BINDIR)/output.map -Wl,--gc-sections -o $(BINDIR)/$@ $(OBJS)
	@$(ECHO) " NM        `basename $@ .elf`.sym";
	$(CMD_PREFIX)$(NM) -n $(BINDIR)/$@ > $(BINDIR)/`basename $@ .elf`.sym
	@$(ECHO) " BIN       `basename $@ .elf`.bin"
	$(CMD_PREFIX)$(OBJCOPY) -O binary $(BINDIR)/$@ $(BINDIR)/`basename $@ .elf`.bin
	$(CMD_PREFIX)$(SIZE) $(BINDIR)/$@ 2>/dev/null
