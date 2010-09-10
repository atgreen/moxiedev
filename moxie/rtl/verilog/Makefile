
MSOURCES = muskoka.v testm.v cpu_ififo.v cpu_fetch.v cpu_decode.v \
cpu_execute.v cpu_registerfile.v

all: a.out 

hello.x: hello.c
	moxie-elf-gcc -o hello.x -O2 hello.c -Tsim.ld

hello.vh: hello.x
	moxie-elf-objcopy -O verilog hello.x hello.vh

a.out: $(MSOURCES) hello.vh
	iverilog $(MSOURCES)

clean: 
	-rm -f a.out *.vh *.x *.vcd *~

