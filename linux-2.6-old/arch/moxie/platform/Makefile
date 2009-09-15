#
# Makefile for arch/microblaze/platform directory
#
#obj-$(CONFIG_PLATFORM_GENERIC) += generic/

$(obj)/platform-plus-dtb.o: $(obj)/platform.o $(obj)/system.dtb
	moxie-elf-objcopy --add-section=.kernel:dtb=$(obj)/system.dtb $(obj)/platform.o $(obj)/platform-plus-dtb.o 
	moxie-elf-objcopy --set-section-flags .kernel:dtb=contents,data,readonly,load,alloc $(obj)/platform-plus-dtb.o

$(obj)/system.dtb: $(src)/generic/system.dts
	./scripts/dtc/dtc -O dtb -o $(obj)/system.dtb -b -0 $<

obj-y	+= platform-plus-dtb.o


