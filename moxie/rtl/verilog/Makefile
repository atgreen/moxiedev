
MSOURCES = muskoka.v moxie.v cpu_ififo.v cpu_fetch.v \
cpu_decode.v cpu_execute.v cpu_registerfile.v \
altera_sdram/sdram_controller.v \
wb_intercon.v

PROJECT = muskoka

ASSIGNMENT_FILES = $(PROJECT).qpf $(PROJECT).qsf

all: a.out smart.log $(PROJECT).asm.rpt $(PROJECT).tan.rpt

sim: a.out

hello.x: hello.c
	moxie-elf-gcc -o hello.x -O2 hello.c -Tsim.ld

hello.vh: hello.x
	moxie-elf-objcopy -O verilog hello.x hello.vh

a.out: $(MSOURCES) hello.vh
	iverilog $(MSOURCES)

map: smart.log $(PROJECT).map.rpt
fit: smart.log $(PROJECT).fit.rpt
asm: smart.log $(PROJECT).asm.rpt
tan: smart.log $(PROJECT).tan.rpt
smart: smart.log

MAP_ARGS = --family=CycloneII
FIT_ARGS = --part=EP2C35F672C6
ASM_ARGS = 
TAN_ARGS = 

STAMP = echo done >

$(ASSIGNMENT_FILES).qsf: setup_proj.tcl
	quartus_sh -t setup_proj.tcl

$(PROJECT).map.rpt: map.chg $(MSOURCES)
	quartus_map $(MAP_ARGS) $(PROJECT)
	$(STAMP) fit.chg

$(PROJECT).fit.rpt: fit.chg $(PROJECT).map.rpt
	quartus_fit $(FIT_ARGS) $(PROJECT)
	$(STAMP) asm.chg
	$(STAMP) tan.chg

$(PROJECT).asm.rpt: asm.chg $(PROJECT).fit.rpt
	quartus_asm $(ASM_ARGS) $(PROJECT)

$(PROJECT).tan.rpt: tan.chg $(PROJECT).fit.rpt
	quartus_tan $(TAN_ARGS) $(PROJECT)

smart.log: $(ASSIGNMENT_FILES)
	quartus_sh --determine_smart_action $(PROJECT) > smart.log

################################################################
# Project initialization
################################################################
map.chg:
	$(STAMP) map.chg

fit.chg:
	$(STAMP) fit.chg

tan.chg:
	$(STAMP) tan.chg

asm.chg:
	$(STAMP) asm.chg

clean: 
	-rm -f a.out *.vh *.x *.vcd *~
	-rm -rf *.rpt *.chg smart.log *.htm *.eqn *.pin *.sof *.pof 
	-rm -rf db incremental_db *.qpf *.qsf muskoka.done
	-rm -rf *.ssf *.csf *.esf *.fsf *.psf *.quartus *.qws
	-rm -rf *.summary *.smsg


