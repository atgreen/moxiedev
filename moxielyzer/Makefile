moxielyzer: moxielyzer.c scan.c check_cmp.c offset_regs.c
	gcc -g -o $@ $^ -I ../src/include -I ../build/src/bfd -I. \
		-L../build/src/bfd -lbfd -lz -L../build/src/libiberty -liberty

clean:
	rm moxielyzer