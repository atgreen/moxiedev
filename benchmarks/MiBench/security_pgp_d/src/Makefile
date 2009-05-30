# Declarations
COMPILER = moxie-elf-gcc -Tsim.ld
OPTS_DEF = -O2
MISC_OPTS = -lm -DUNIX -DPORTABLE

# Actions
all:		
	$(COMPILER) *.c $(OPTS_DEF) $(MISC_OPTS) 
