MSOURCES = sim.v testram.v \
	$(wildcard ../rtl/*.v) \
	$(wildcard ../../../cores/moxie/*.v) \
	$(wildcard ../../../cores/wishbone/*.v) \
	$(wildcard ../../../cores/LVT-regs/*.v) \
	$(wildcard ../../../cores/uart16550/rtl/verilog/*.v) 

PROJECT = muskoka

all: a.out

bootrom.vh: ../../../firmware/bootrom.s
	moxie-elf-as -o bootrom.x $<
	moxie-elf-objcopy -O verilog bootrom.x bootrom.vh

a.out: $(MSOURCES) bootrom.vh
	iverilog -I../../../cores/moxie \
		 -I../../../cores/LVT-regs \
		 -I../../../cores/uart16550/rtl/verilog $(MSOURCES)

clean: 
	-rm -f a.out *.vh *.x *.vcd *~


