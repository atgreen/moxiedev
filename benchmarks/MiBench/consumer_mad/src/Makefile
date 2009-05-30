# Declarations
COMPILER = moxie-elf-gcc -Tsim.ld
OPTS_DEF = -O2
MISC_OPTS = -lm -lz -lesd -I. -DFPM_DEFAULT -DHAVE_CONFIG_H -DLOCALEDIR=\"/usr/local/share/locale\"

# Actions
all:		
	$(COMPILER) *.c $(OPTS_DEF) $(MISC_OPTS) 
