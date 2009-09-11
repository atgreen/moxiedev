all: helloq.x hello.x init

clean: 
	rm -f *.x *.o *.i *.s *~

helloq.x: hello.c
	moxie-elf-gcc -g -O0 -fno-inline -o $@ -g hello.c -Tqemu.ld -save-temps

hello.x: hello.c
	moxie-elf-gcc -g -O0 -fno-inline -o $@ -g hello.c -Tsim.ld

init: init.c
	moxie-uclinux-gcc -g -O2 -o init init.c
