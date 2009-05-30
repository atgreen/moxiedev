all: helloq.x hello.x

clean: 
	rm -f *.x *.o *.i *.s *~

helloq.x: hello.c
	moxie-elf-gcc -fno-inline -o $@ -g hello.c -Tqemu.ld -save-temps

hello.x: hello.c
	moxie-elf-gcc -fno-inline -o $@ -g hello.c -Tsim.ld

