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
# Defines target-specific build-rules variables, extra sources and include paths
#

$(info ICICLE-KIT-ES selected)

ifdef MASTER
BINDIR=Master-Default
RISCV_TARGET:=mpfs-rpmsg-master.elf
else
BINDIR=Remote-Default
RISCV_TARGET:=mpfs-rpmsg-remote.elf
endif

TARGET:=$(RISCV_TARGET)

ifdef MASTER
LINKER_SCRIPT=src/boards/${BOARD}/platform_config/linker/mpfs-ddr-loaded-by-boot-loader-master.ld
else
LINKER_SCRIPT=src/boards/${BOARD}/platform_config/linker/mpfs-ddr-loaded-by-boot-loader-remote.ld
endif

BOARD_DIR=src/boards/icicle-kit-es

INCLUDES += \
    -I$(BOARD_DIR)/platform_config \
    -I$(BOARD_DIR)/fpga_design_config \
    -I$(BOARD_DIR) \
    -Iplatform \

#
# Extra dependency rules for auto-generated configuration files (from Libero XML)
#

SOC_CONFIG_FILES =  \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_ddr_pll.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_mss_cfm.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_mss_pll.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_sgmii_cfm.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_sgmii_pll.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_clk_sysreg.h \
	$(BOARD_DIR)/fpga_design_config/clocks/hw_mss_clks.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddr_io_bank.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddr_mode.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddr_off_mode.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddr_options.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddr_segs.h \
	$(BOARD_DIR)/fpga_design_config/ddr/hw_ddrc.h \
	$(BOARD_DIR)/fpga_design_config/general/hw_gen_peripherals.h \
	$(BOARD_DIR)/fpga_design_config/fpga_design_config.h \
	$(BOARD_DIR)/fpga_design_config/io/hw_hsio_mux.h \
	$(BOARD_DIR)/fpga_design_config/io/hw_mssio_mux.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_apb_split.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_cache.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_memory.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_crypto.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_fic0.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_fic1.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_fic2.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_gem0.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_gem1.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_mmc.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_scb.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_trace.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_mpu_usb.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_pmp_hart0.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_pmp_hart1.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_pmp_hart2.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_pmp_hart3.h \
	$(BOARD_DIR)/fpga_design_config/memory_map/hw_pmp_hart4.h \
	$(BOARD_DIR)/fpga_design_config/sgmii/hw_sgmii_tip.h \

$(SOC_CONFIG_FILES): $(BOARD_DIR)/fpga_design/design_description/ICICLE_MSS_cfg.xml
	@$(ECHO) " CFGGEN $<";
	@$(CMD_PREFIX)$(PYTHON) src/platform/soc_config_generator/mpfs_configuration_generator.py $(BOARD_DIR)/fpga_design/design_description/ICICLE_MSS_cfg.xml $(BOARD_DIR)

$(RISCV_TARGET): $(SOC_CONFIG_FILES)

