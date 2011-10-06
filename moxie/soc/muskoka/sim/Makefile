MSOURCES = sim.v testram.v \
	$(wildcard ../rtl/*.v) \
	$(wildcard ../../../cores/moxie/*.v) \
	$(wildcard ../../../cores/wishbone/*.v)

PROJECT = muskoka

all: a.out

bootrom.vh: ../../../firmware/bootrom.s
	moxie-elf-as -o bootrom.x $<
	moxie-elf-objcopy -O verilog bootrom.x bootrom.vh

a.out: $(MSOURCES) bootrom.vh
	iverilog -I../../../cores/moxie $(MSOURCES)

clean: 
	-rm -f a.out *.vh *.x *.vcd *~


