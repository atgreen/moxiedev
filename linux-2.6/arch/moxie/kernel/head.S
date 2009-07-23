/*
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * MMU code derived from arch/ppc/kernel/head_4xx.S:
 *    Copyright (c) 1995-1996 Gary Thomas <gdt@linuxppc.org>
 *      Initial PowerPC version.
 *    Copyright (c) 1996 Cort Dougan <cort@cs.nmt.edu>
 *      Rewritten for PReP
 *    Copyright (c) 1996 Paul Mackerras <paulus@cs.anu.edu.au>
 *      Low-level exception handers, MMU support, and rewrite.
 *    Copyright (c) 1997 Dan Malek <dmalek@jlc.net>
 *      PowerPC 8xx modifications.
 *    Copyright (c) 1998-1999 TiVo, Inc.
 *      PowerPC 403GCX modifications.
 *    Copyright (c) 1999 Grant Erickson <grant@lcse.umn.edu>
 *      PowerPC 403GCX/405GP modifications.
 *    Copyright 2000 MontaVista Software Inc.
 *	PPC405 modifications
 *      PowerPC 403GCX/405GP modifications.
 * 	Author: MontaVista Software, Inc.
 *         	frank_rowand@mvista.com or source@mvista.com
 * 	   	debbie_chu@mvista.com
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/linkage.h>
#include <asm/thread_info.h>
#include <asm/page.h>

#ifdef CONFIG_MMU
#include <asm/setup.h> /* COMMAND_LINE_SIZE */
#include <asm/mmu.h>
#include <asm/processor.h>

.data
.global empty_zero_page
.align 12
empty_zero_page:
	.space	4096
.global swapper_pg_dir
swapper_pg_dir:
	.space	4096

#endif /* CONFIG_MMU */

	.text
ENTRY(_start)

	/* Initialize stack and frame pointers.  */
	ldi.l	$sp, init_thread_union + THREAD_SIZE - 4
	mov	$fp, $sp
	jsra	machine_early_init
	jmpa	start_kernel

	.text
ENTRY(of_device_get_modalias)
	ret
ENTRY(of_release_dev)
	ret
ENTRY(of_device_register)
	ret
ENTRY(ret_from_fork)
	ret
ENTRY(sys_execve)
	ret
ENTRY(sys_clone)
	ret
ENTRY(sys_rt_sigreturn_wrapper)
	ret
ENTRY(sys_rt_sigsuspend_wrapper)
	ret
ENTRY(sys_vfork)
	ret
ENTRY(__umodsi3)
	ret
ENTRY(__udivsi3)
	ret
ENTRY(__mulsi3)
	ret
ENTRY(__modsi3)
	ret
ENTRY(__divsi3)
	ret
ENTRY(of_bus_type_init)
	ret
ENTRY(_switch_to)
	ret

