ifeq ($(CONFIG_MMU),y)
UTS_SYSNAME = -DUTS_SYSNAME=\"Linux\"
else
UTS_SYSNAME = -DUTS_SYSNAME=\"uClinux\"
endif

# What CPU vesion are we building for, and crack it open
# as major.minor.rev
CPU_VER=$(subst ",,$(CONFIG_XILINX_MOXIE0_HW_VER) )
CPU_MAJOR=$(shell echo $(CPU_VER) | cut -d '.' -f 1)
CPU_MINOR=$(shell echo $(CPU_VER) | cut -d '.' -f 2)
CPU_REV=$(shell echo $(CPU_VER) | cut -d '.' -f 3)

export CPU_VER CPU_MAJOR CPU_MINOR CPU_REV

# Use cpu-related CONFIG_ vars to set compile options.

CPUFLAGS-1 += $(call cc-option,-mcpu=v$(CPU_VER))

# The various CONFIG_XILINX cpu features options are integers 0/1/2...
# rather than bools y/n

# r13 holds current when in kernel mode
CFLAGS_KERNEL += -ffixed-\$r13 $(CPUFLAGS-1) $(CPUFLAGS-2)

LDFLAGS		:=
LDFLAGS_vmlinux	:=
LDFLAGS_BLOB := --format binary --oformat elf32-moxie

LIBGCC := $(shell $(CC) $(CFLAGS_KERNEL) -print-libgcc-file-name)

head-y		:= arch/moxie/kernel/head.o
libs-y		+= arch/moxie/lib/ $(LIBGCC)
core-y		+= arch/moxie/kernel/ arch/moxie/mm/ \
		   arch/moxie/platform/

boot := arch/$(ARCH)/boot

# defines filename extension depending memory management type
ifeq ($(CONFIG_MMU),)
MMUEXT		:= -nommu
endif
export	MMUEXT

all: linux.bin

archclean:
	$(Q)$(MAKE) $(clean)=$(boot)

linux.bin linux.bin.gz: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@

define archhelp
  echo  '* linux.bin    - Create raw binary'
  echo  '  linux.bin.gz - Create compressed raw binary'
endef
