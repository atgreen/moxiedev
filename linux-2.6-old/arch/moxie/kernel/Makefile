#
# Makefile
#

extra-y := head.o vmlinux.lds

#obj-y += exceptions.o \
#	hw_exception_handler.o init_task.o intc.o irq.o of_device.o \
#	of_platform.o process.o prom.o prom_parse.o ptrace.o \
#	setup.o signal.o sys_moxie.o timer.o traps.o

obj-y += exception_handler.o exceptions.o \
	init_task.o intc.o irq.o of_device.o \
	of_platform.o process.o prom.o prom_parse.o ptrace.o \
	setup.o signal.o sys_moxie.o timer.o traps.o \
	simserial.o

obj-y += cpu/

obj-$(CONFIG_EARLY_PRINTK)	+= early_printk.o
obj-$(CONFIG_SELFMOD)		+= selfmod.o
obj-$(CONFIG_HEART_BEAT)	+= heartbeat.o
obj-$(CONFIG_MODULES)		+= moxie_ksyms.o module.o
obj-$(CONFIG_MMU)		+= misc.o

obj-y	+= entry$(MMUEXT).o
