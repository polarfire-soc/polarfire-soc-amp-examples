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
# Defines Rules such as compiler and tool choice/settings, build flags, and
# basic build rules (.c to .o, etc)
#

CROSS_COMPILE?=riscv64-unknown-elf-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
SIZE=$(CROSS_COMPILE)size
GPROF=$(CROSS_COMPILE)gprof
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
READELF=$(CROSS_COMPILE)readelf
NM=$(CROSS_COMPILE)nm
ECHO=echo
MAKE=make
CP=cp
MAKEDEP=makedepend


PLATFORM_RISCV_ABI=lp64d
PLATFORM_RISCV_ISA=rv64gc

CORE_CFLAGS+=$(MCMODEL) 

ifdef CONFIG_WITH_ARCH
CORE_CFLAGS+=-mabi=$(PLATFORM_RISCV_ABI) -march=$(PLATFORM_RISCV_ISA)
endif

CORE_CFLAGS+= $(EXT_CFLAGS) -DDEFINE_MALLOC -DDEFINE_FREE -no-pie -fno-PIE 

ifdef MASTER
CORE_CFLAGS+= -DRPMSG_MASTER
endif

ifdef REMOTEPROC
ifndef MASTER
CORE_CFLAGS+= -DREMOTEPROC
endif
endif

CORE_CFLAGS+=-msmall-data-limit=8 -mstrict-align -mno-save-restore -O0 \
    -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3

CFLAGS=-std=gnu11 $(CORE_CFLAGS) $(PLATFORM_CFLAGS)

##############################################################################
#
# Build Rules
#

# Check if verbosity is ON for build process
CMD_PREFIX_DEFAULT := @
ifeq ($(V), 1)
  CMD_PREFIX :=
else
  CMD_PREFIX := $(CMD_PREFIX_DEFAULT)
endif

OBJS = $(addprefix $(BINDIR)/,$(ASM_SRCS:.S=.o) $(SRCS:.c=.o))

$(BINDIR)/%.o: %.c $(BINDIR)/%.d
	@$(ECHO) " CC $@";
	$(CMD_PREFIX)$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(BINDIR)/%.o: %.S
	@$(ECHO) " CC  $@";
	$(CMD_PREFIX)$(CC) $(CFLAGS) $(defs) -D__ASSEMBLY__=1 -c $(INCLUDES) $< -o $@

$(BINDIR)/%.d: %.c
	@mkdir -p $(@D)
	$(CMD_PREFIX)$(MAKEDEP) -f - $(INCLUDES) $< 2>/dev/null | sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\n$(BINDIR)/\1 : \2,g' > $(BINDIR)/$*.d

$(BINDIR)/%.d: %.S
	@mkdir -p $(@D)
	$(CMD_PREFIX)$(MAKEDEP) -f - $(INCLUDES) $< 2>/dev/null | sed 's,\($*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\n$(BINDIR)/\1 : \2,g' > $(BINDIR)/$*.d


