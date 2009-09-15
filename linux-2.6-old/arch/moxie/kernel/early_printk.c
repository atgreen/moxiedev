/*
 * Early printk support for Microblaze.
 *
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2003-2006 Yasushi SHOJI <yashi@atmark-techno.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/tty.h>
#include <linux/io.h>
#include <asm/processor.h>
#include <linux/fcntl.h>
#include <asm/setup.h>
#include <asm/prom.h>

static u32 early_console_initialized;
static u32 base_addr;

static void early_libgloss_printk_write(struct console *unused,
					const char *s, unsigned n)
{
	/* First, try the simulator escape... */
	asm ("ldi.l $r0, 0x0");
	asm ("swi 5");
	
	/* Then try writing to the qemu serial port... */
	{
		int i = 0;
		while (i < n)
			*(char *)0x3f8 = s[i++];
		return n;
	}
}

extern struct tty_driver *hp_simserial_driver;

static struct tty_driver *simcons_console_device (struct console *c, int *index)
{
	*index = c->index;
	return hp_simserial_driver;
}

struct console early_serial_console = {
	.name = "earlyser",
	.write = early_libgloss_printk_write,
	.flags = CON_PRINTBUFFER,
	.device = simcons_console_device,
	.index = -1,
};

static struct console *early_console = &early_serial_console;

void early_printk(const char *fmt, ...)
{
	char buf[512];
	int n;
	va_list ap;

	if (early_console_initialized) {
		va_start(ap, fmt);
		n = vscnprintf(buf, 512, fmt, ap);
		early_console->write(early_console, buf, n);
		va_end(ap);
	}
}

int __init setup_early_printk(char *opt)
{
  early_console_initialized = 1;
}

void __init disable_early_printk(void)
{
#if 0
	if (!early_console_initialized || !early_console)
		return;
	printk(KERN_WARNING "disabling early console\n");
	unregister_console(early_console);
	early_console_initialized = 0;
#endif
}
