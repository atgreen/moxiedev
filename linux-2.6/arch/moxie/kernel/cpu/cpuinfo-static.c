/*
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2007 John Williams <john.williams@petalogix.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <asm/cpuinfo.h>
#include <asm/pvr.h>

static const char family_string[] = "Moxie";
static const char cpu_ver_string[] = "1.0";

#define err_printk(x) \
	early_printk("ERROR: Moxie " x "- different for kernel and DTS\n");

void __init set_cpuinfo_static(struct cpuinfo *ci, struct device_node *cpu)
{
  MOXIE_DIE;
}
