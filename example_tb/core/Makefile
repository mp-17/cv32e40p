# Copyright 2019 Clifford Wolf
# Copyright 2019 Robert Balas
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

# Author: Robert Balas (balasr@iis.ee.ethz.ch)
#
# Description: Makefile for running the following examples:
#
#              - 'Hello World' (custom)
#              - 'Floating Point example' (custom_fp)
#              - 'Interrupt example' (interrupt)

MAKE			= make

# vsim configuration
VVERSION                = "10.7b"

VLIB			= vlib-$(VVERSION)
VWORK			= work

VLOG			= vlog-$(VVERSION)
VLOG_FLAGS		= -pedanticerrors -suppress 2577 -suppress 2583 -define CV32E40P_TRACE_EXECUTION -define CV32E40P_ASSERT_ON
VLOG_LOG                = vloggy

VOPT			= vopt-$(VVERSION)
VOPT_FLAGS		= -debugdb -fsmdebug -pedanticerrors +acc #=mnprft

VSIM			= vsim-$(VVERSION)
VSIM_HOME               = /usr/pack/modelsim-$(VVERSION)-kgf/questasim
VSIM_FLAGS              =  # user defined
ALL_VSIM_FLAGS		= $(VSIM_FLAGS)
VSIM_DEBUG_FLAGS	= -debugdb
VSIM_GUI_FLAGS          = -gui -debugdb
VSIM_SCRIPT             = vsim.tcl

# RTL source files
RTLSRC_HOME             := ../..
RTLSRC_TB_PKG		:= include/perturbation_pkg.sv include/cv32e40p_tracer_pkg.sv
RTLSRC_TB_TOP		:= tb_top.sv
RTLSRC_TB		:= $(wildcard *.sv)
RTLSRC_VERI_TB          := $(filter-out tb_top.sv, $(wildcard *.sv))
RTLSRC_INCDIR           := $(RTLSRC_HOME)/rtl/include
RTLSRC_PKG		:= $(RTLSRC_HOME)/rtl/fpnew/src/fpnew_pkg.sv \
				$(addprefix $(RTLSRC_HOME)/rtl/include/,\
				cv32e40p_apu_core_pkg.sv cv32e40p_pkg.sv)
RTLSRC			:= $(RTLSRC_HOME)/bhv/cv32e40p_sim_clock_gate.sv $(filter-out $(RTLSRC_HOME)/rtl/cv32e40p_register_file_latch.sv,\
				$(wildcard $(RTLSRC_HOME)/rtl/*.sv))
RTLSRC_FPNEW_INCDIR := $(RTLSRC_HOME)/rtl/fpnew/src/common_cells/include
RTLSRC_FPNEWPKG     := $(RTLSRC_HOME)/rtl/fpnew/src/fpu_div_sqrt_mvp/hdl/defs_div_sqrt_mvp.sv
RTLSRC_FPNEW        := $(wildcard $(RTLSRC_HOME)/rtl/fpnew/src/*.sv) $(wildcard $(RTLSRC_HOME)/rtl/fpnew/src/fpu_div_sqrt_mvp/hdl/*.sv) \
                    $(wildcard $(RTLSRC_HOME)/rtl/fpnew/src/common_cells/src/*.sv) $(wildcard $(RTLSRC_HOME)/rtl/fpnew/src/common_cells/src/deprecated/*.sv)

RTLSRC_VLOG_TB_TOP	:= $(basename $(notdir $(RTLSRC_TB_TOP)))
RTLSRC_VOPT_TB_TOP	:= $(addsuffix _vopt, $(RTLSRC_VLOG_TB_TOP))

# riscv toolchain install path
RISCV                    ?= ~/.riscv
RISCV_EXE_PREFIX         = $(RISCV)/bin/riscv32-unknown-elf-

# assume vsim if no target chosen
.DEFAULT_GOAL := custom-vsim-run

# GCC configuration
CUSTOM_GCC_FLAGS           =
INTERRUPT_GCC_FLAGS        =

all: custom-vsim-run

# vsim testbench compilation and optimization
vlib: .lib-rtl

.lib-rtl:
	$(VLIB) $(VWORK)
	touch .lib-rtl

# rebuild if we change some sourcefile
.build-rtl: .lib-rtl $(RTLSRC_PKG) $(RTLSRC) $(RTLSRC_TB_PKG) $(RTLSRC_TB)
	$(VLOG) -work $(VWORK) +incdir+$(RTLSRC_INCDIR) $(VLOG_FLAGS) \
	$(RTLSRC_PKG) $(RTLSRC) $(RTLSRC_TB_PKG) $(RTLSRC_TB)
	touch .build-rtl

vsim-all: .opt-rtl

.opt-rtl: .build-rtl
	$(VOPT) -work $(VWORK) $(VOPT_FLAGS) $(RTLSRC_VLOG_TB_TOP) -o \
	$(RTLSRC_VOPT_TB_TOP)
	touch .opt-rtl

# rebuild if we change some sourcefile
.build-rtl-fp: .lib-rtl $(RTLSRC_PKG) $(RTLSRC_FPNEWPKG) $(RTLSRC_FPNEW) $(RTLSRC) $(RTLSRC_TB_PKG) $(RTLSRC_TB)
	$(VLOG) -work $(VWORK) +incdir+$(RTLSRC_INCDIR) +incdir+$(RTLSRC_FPNEW_INCDIR) $(VLOG_FLAGS) \
	$(RTLSRC_PKG) $(RTLSRC_FPNEWPKG) $(RTLSRC_FPNEW) $(RTLSRC) $(RTLSRC_TB_PKG) $(RTLSRC_TB)
	touch .build-rtl-fp

vsim-all-fp: .opt-rtl-fp

.opt-rtl-fp: .build-rtl-fp
	$(VOPT) -work $(VWORK) $(VOPT_FLAGS) -g FPU=1 $(RTLSRC_VLOG_TB_TOP) -o \
	$(RTLSRC_VOPT_TB_TOP)
	touch .opt-rtl-fp

# run tb and exit
.PHONY: vsim-run
vsim-run: ALL_VSIM_FLAGS += -c
vsim-run: vsim-all
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) \
	$(RTLSRC_VOPT_TB_TOP) -do 'source $(VSIM_SCRIPT); exit -f'

# run tb and drop into interactive shell
.PHONY: vsim-run-sh
vsim-run-sh: ALL_VSIM_FLAGS += -c
vsim-run-sh: vsim-all
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) \
	$(RTLSRC_VOPT_TB_TOP) -do $(VSIM_SCRIPT)

# run tb with simulator gui
.PHONY: vsim-run-gui
vsim-run-gui: ALL_VSIM_FLAGS += $(VSIM_GUI_FLAGS)
vsim-run-gui: vsim-all
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) \
	$(RTLSRC_VOPT_TB_TOP) -do $(VSIM_SCRIPT)

.PHONY: tb-clean
tb-clean:
	if [ -d $(VWORK) ]; then rm -r $(VWORK); fi
	rm -f transcript vsim.wlf vsim.dbg trace_core*.log \
	.build-rtl .opt-rtl .lib-rtl *.vcd objdump

# run tb and exit
.PHONY: vsim-run-fp
vsim-run-fp: ALL_VSIM_FLAGS += -c
vsim-run-fp: vsim-all-fp
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) -GFPU=1 \
	$(RTLSRC_VOPT_TB_TOP) -do 'source $(VSIM_SCRIPT); exit -f'

# run tb and drop into interactive shell
.PHONY: vsim-run-sh-fp
vsim-run-sh-fp: ALL_VSIM_FLAGS += -c
vsim-run-sh-fp: vsim-all-fp
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) \
	$(RTLSRC_VOPT_TB_TOP) -do $(VSIM_SCRIPT)

# run tb with simulator gui
.PHONY: vsim-run-gui-fp
vsim-run-gui-fp: ALL_VSIM_FLAGS += $(VSIM_GUI_FLAGS)
vsim-run-gui-fp: vsim-all-fp
	$(VSIM) -work $(VWORK) $(ALL_VSIM_FLAGS) -GFPU=1 \
	$(RTLSRC_VOPT_TB_TOP) -do $(VSIM_SCRIPT)

.PHONY: tb-clean-fp
tb-clean-fp:
	if [ -d $(VWORK) ]; then rm -r $(VWORK); fi
	rm -f transcript vsim.wlf vsim.dbg trace_core*.log \
	.build-rtl-fp .opt-rtl-fp .lib-rtl-fp *.vcd objdump

# rules to generate hex (loadable by simulators) from elf
%.hex: %.elf
	$(RISCV_EXE_PREFIX)objcopy -O verilog $< $@

# Running custom programs:
# This is an example for running a Hello World in the testbench
# We link with our custom crt0.s and syscalls.c against newlib so that we can
# use the c standard library
custom/hello_world.elf: custom/hello_world.c
	$(RISCV_EXE_PREFIX)gcc -march=rv32imc -o $@ -w -Os -g -nostdlib \
		$(CUSTOM_GCC_FLAGS) \
		-T custom/link.ld \
		-static \
		custom/crt0.S \
		$^ mem_stall/mem_stall.c custom/syscalls.c custom/vectors.S \
		-I $(RISCV)/riscv32-unknown-elf/include \
		-I mem_stall \
		-L $(RISCV)/riscv32-unknown-elf/lib \
		-lc -lm -lgcc
custom-clean:
	rm -rf custom/hello_world.elf custom/hello_world.hex

.PHONY: custom-vsim-run
custom-vsim-run: vsim-all custom/hello_world.hex
custom-vsim-run: ALL_VSIM_FLAGS += "+firmware=custom/hello_world.hex"
custom-vsim-run: vsim-run

.PHONY: custom-vsim-run-gui
custom-vsim-run-gui: vsim-all custom/hello_world.hex
custom-vsim-run-gui: ALL_VSIM_FLAGS += "+firmware=custom/hello_world.hex"
custom-vsim-run-gui: vsim-run-gui

# Running custom fp programs:
# This is an example for running a floating-point matrix multiplication in the testbench
# We link with our custom crt0.s and syscalls.c against newlib so that we can
# use the c standard library
custom_fp/main.elf: custom_fp/matmulNxN.c custom_fp/main.c
	$(RISCV_EXE_PREFIX)gcc -march=rv32imfc -mabi=ilp32f -o $@ -w -g -nostdlib \
		$(CUSTOM_GCC_FLAGS) \
		-T custom/link.ld  \
		-static \
		custom/crt0.S \
		$^ custom/syscalls.c custom/vectors.S \
		-I $(RISCV)/riscv32-unknown-elf/include \
		-L $(RISCV)/riscv32-unknown-elf/lib \
		-lc -lm -lgcc
custom-fp-clean:
	rm -rf custom_fp/main.elf custom_fp/main.hex

.PHONY: custom-fp-vsim-run
custom-fp-vsim-run: vsim-all-fp custom_fp/main.hex
custom-fp-vsim-run: ALL_VSIM_FLAGS += "+firmware=custom_fp/main.hex"
custom-fp-vsim-run: vsim-run-fp


.PHONY: custom-fp-vsim-run-gui
custom-fp-vsim-run-gui: vsim-all-fp custom_fp/main.hex
custom-fp-vsim-run-gui: ALL_VSIM_FLAGS += "+firmware=custom_fp/main.hex"
custom-fp-vsim-run-gui: vsim-run-gui-fp

# compile and run interrupt
interrupt/interrupt.elf: interrupt/interrupt.c
	$(RISCV_EXE_PREFIX)gcc -march=rv32imc -o $@ -w -Os -g -nostdlib \
		${INTERRUPT_GCC_FLAGS} \
		-T custom/link.ld  \
		-static \
		custom/crt0.S \
		$^ mem_stall/mem_stall.c custom/syscalls.c interrupt/vectors.S \
		-I $(RISCV)/riscv32-unknown-elf/include \
		-I mem_stall/ \
		-L $(RISCV)/riscv32-unknown-elf/lib \
		-lc -lm -lgcc
interrupt-clean:
	rm -rf interrupt/interrupt.elf interrupt/interrupt.hex

.PHONY: interrupt-vsim-run
interrupt-vsim-run: vsim-all interrupt/interrupt.hex
interrupt-vsim-run: ALL_VSIM_FLAGS += "+firmware=interrupt/interrupt.hex"
interrupt-vsim-run: vsim-run

.PHONY: interrupt-vsim-run-gui

interrupt-vsim-run-gui: vsim-all interrupt/interrupt.hex
interrupt-vsim-run-gui: ALL_VSIM_FLAGS += "+firmware=interrupt/interrupt.hex"
interrupt-vsim-run-gui: vsim-run-gui

# general targets
.PHONY: clean
clean: tb-clean tb-clean-fp custom-clean custom-fp-clean interrupt-clean
