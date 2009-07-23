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

#include <linux/linkage.h>
#include <asm/thread_info.h>
#include <linux/errno.h>
#include <asm/entry.h>
#include <asm/asm-offsets.h>
#include <asm/registers.h>
#include <asm/unistd.h>
#include <asm/percpu.h>
#include <asm/signal.h>

.section .rodata,"a"
#include "syscall_table.S"

syscall_table_size=(.-sys_call_table)
