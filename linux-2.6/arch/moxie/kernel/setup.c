/*
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/seq_file.h>
#include <linux/screen_info.h>
#include <linux/cpu.h>
#include <linux/initrd.h>
#include <linux/console.h>
#include <linux/debugfs.h>

#include <asm/setup.h>
#include <asm/sections.h>
#include <asm/page.h>
#include <linux/io.h>
#include <linux/bug.h>
#include <linux/param.h>
#include <linux/cache.h>
#include <asm/cacheflush.h>
#include <asm/entry.h>
#include <asm/cpuinfo.h>

#include <asm/system.h>
#include <asm/prom.h>
#include <asm/pgtable.h>

#if defined(CONFIG_VGA_CONSOLE)
struct screen_info screen_info = {
	.orig_video_lines      = 25,
	.orig_video_cols       = 80,
	.orig_video_mode       = 0,
	.orig_video_ega_bx     = 0,
	.orig_video_isVGA      = 1,
	.orig_video_points     = 8
};
#endif

DEFINE_PER_CPU(unsigned int, KSP);	/* Saved kernel stack pointer */
DEFINE_PER_CPU(unsigned int, KM);	/* Kernel/user mode */
DEFINE_PER_CPU(unsigned int, ENTRY_SP);	/* Saved SP on kernel entry */
DEFINE_PER_CPU(unsigned int, R11_SAVE);	/* Temp variable for entry */
DEFINE_PER_CPU(unsigned int, CURRENT_SAVE);	/* Saved current pointer */

unsigned int boot_cpuid;
char cmd_line[COMMAND_LINE_SIZE];

extern struct console early_serial_console;

/* The exception handler is defined in exception_handler.S.  */
extern void moxie_exception_handler(void);

static void install_handler(void (*handler)(void))
{
  asm("ssr %0, 1" : : "r" (handler));
}

void __init setup_arch(char **cmdline_p)
{
	*cmdline_p = cmd_line;

	register_console(&early_serial_console);
	console_verbose();

	unflatten_device_tree();

	/* NOTE I think that this function is not necessary to call */
	/* irq_early_init(); */
	setup_cpuinfo();

	/* This is where we would normally set up the caches.  
	   Moxie currently has no cache, so there's nothing to do. 

	__invalidate_icache_all();
	__enable_icache();

	__invalidate_dcache_all();
	__enable_dcache(); */

	panic_timeout = 120;

	setup_memory();

#if defined(CONFIG_SELFMOD_INTC) || defined(CONFIG_SELFMOD_TIMER)
	printk(KERN_NOTICE "Self modified code enable\n");
#endif

#ifdef CONFIG_VT
#if defined(CONFIG_XILINX_CONSOLE)
	conswitchp = &xil_con;
#elif defined(CONFIG_DUMMY_CONSOLE)
	conswitchp = &dummy_con;
#endif
#endif
}

#ifdef CONFIG_MTD_UCLINUX
/* Handle both romfs and cramfs types, without generating unnecessary
 code (ie no point checking for CRAMFS if it's not even enabled) */
inline unsigned get_romfs_len(unsigned *addr)
{
#ifdef CONFIG_ROMFS_FS
	if (memcmp(&addr[0], "-rom1fs-", 8) == 0) /* romfs */
		return be32_to_cpu(addr[2]);
#endif

#ifdef CONFIG_CRAMFS
	if (addr[0] == le32_to_cpu(0x28cd3d45)) /* cramfs */
		return le32_to_cpu(addr[1]);
#endif
	return 0;
}
#endif	/* CONFIG_MTD_UCLINUX_EBSS */

void __init machine_early_init(const char *cmdline)
{
	unsigned long *src, *dst = (unsigned long *)0x0;
	void *fdt_start;

	/* Install the exception handler.  */
	install_handler (moxie_exception_handler);

	/* This kernel port assumes that the device firmware has
	   placed a pointer to the flattened device tree blob in
	   special register 9.  Extract it and place it in
	   fdt_start.  */
	asm("gsr %0, 9" : "=r"(fdt_start) : "0"(fdt_start));

	/* If it's not there, use the built in one and hope for the
	   best.  */
	if (fdt_start == 0)
	  fdt_start = _fdt_start;

/* clearing bss section */
	memset(__bss_start, 0, __bss_stop-__bss_start);

	/* Copy command line passed from bootloader */
#ifndef CONFIG_CMDLINE_BOOL
	if (cmdline && cmdline[0] != '\0')
		strlcpy(cmd_line, cmdline, COMMAND_LINE_SIZE);
#endif

/* initialize device tree for usage in early_printk */
	early_init_devtree((void *)fdt_start);

#ifdef CONFIG_EARLY_PRINTK
	setup_early_printk(NULL);
#endif

	early_printk(KERN_NOTICE "Found FDT at 0x%08x\n", fdt_start);

	for (src = __ivt_start; src < __ivt_end; src++, dst++)
		*dst = *src;

	/* Initialize global data */
	per_cpu(KM, 0) = 0x1;	/* We start in kernel mode */
	per_cpu(CURRENT_SAVE, 0) = (unsigned long)current;
}

#ifdef CONFIG_DEBUG_FS
struct dentry *of_debugfs_root;

static int moxie_debugfs_init(void)
{
	of_debugfs_root = debugfs_create_dir("moxie", NULL);

	return of_debugfs_root == NULL;
}
arch_initcall(moxie_debugfs_init);
#endif

void machine_restart(char *cmd)
{
	printk(KERN_NOTICE "Machine restart...\n");
	dump_stack();
	while (1)
		;
}

void machine_shutdown(void)
{
	printk(KERN_NOTICE "Machine shutdown...\n");
	while (1)
		;
}

void machine_halt(void)
{
	printk(KERN_NOTICE "Machine halt...\n");
	while (1)
		;
}

void machine_power_off(void)
{
	printk(KERN_NOTICE "Machine power off...\n");
	while (1)
		;
}
