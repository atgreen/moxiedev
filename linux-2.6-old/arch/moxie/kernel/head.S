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
#include <asm/asm-offsets.h>
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

	/* Initialize $r13 with init_task.  */
	ldi.l	$r13, init_task
	
	jsra	machine_early_init
	jmpa	start_kernel

	.text
ENTRY(ret_from_fork)
	jsra schedule_tail
	ret
ENTRY(sys_clone)
	bad
ENTRY(sys_rt_sigreturn_wrapper)
	bad
ENTRY(sys_rt_sigsuspend_wrapper)
	bad

ENTRY(sys_vfork)
	/* Move pt_regs to the first argument register and jump to
	   moxie_vfork.  */
	mov	$r0, $r8	
	jmpa	moxie_vfork

ENTRY(__umodsi3)
	bad
ENTRY(__udivsi3)
	bad
ENTRY(__mulsi3)
	bad
ENTRY(__modsi3)
	bad
ENTRY(__divsi3)
	bad

# struct task_struct *__switch_to(struct thread_struct *prev,
#				  struct thread_struct *next,
#				  struct task_struct *prev_task)
	
ENTRY(_switch_to)
	sto.l	TI_CPU_CONTEXT+CC_FP($r0), $fp
	sto.l	TI_CPU_CONTEXT+CC_SP($r0), $sp
	sto.l	TI_CPU_CONTEXT+CC_R0($r0), $r0
	sto.l	TI_CPU_CONTEXT+CC_R1($r0), $r1
	sto.l	TI_CPU_CONTEXT+CC_R2($r0), $r2
	sto.l	TI_CPU_CONTEXT+CC_R3($r0), $r3
	sto.l	TI_CPU_CONTEXT+CC_R4($r0), $r4
	sto.l	TI_CPU_CONTEXT+CC_R5($r0), $r5
	sto.l	TI_CPU_CONTEXT+CC_R6($r0), $r6
	sto.l	TI_CPU_CONTEXT+CC_R7($r0), $r7
	sto.l	TI_CPU_CONTEXT+CC_R8($r0), $r8
	sto.l	TI_CPU_CONTEXT+CC_R9($r0), $r9
	sto.l	TI_CPU_CONTEXT+CC_R10($r0), $r10
	sto.l	TI_CPU_CONTEXT+CC_R11($r0), $r11
	sto.l	TI_CPU_CONTEXT+CC_R12($r0), $r12
	sto.l	TI_CPU_CONTEXT+CC_R13($r0), $r13

	ldo.l	$fp, TI_CPU_CONTEXT+CC_FP($r1)
	ldi.l	$r12, 0
	cmp	$fp, $r12
	beq	.new_thread

	ldo.l	$sp, TI_CPU_CONTEXT+CC_SP($r1)
	ldo.l	$r0, TI_CPU_CONTEXT+CC_R0($r1)
	ldo.l	$r2, TI_CPU_CONTEXT+CC_R2($r1)
	ldo.l	$r3, TI_CPU_CONTEXT+CC_R3($r1)
	ldo.l	$r4, TI_CPU_CONTEXT+CC_R4($r1)
	ldo.l	$r5, TI_CPU_CONTEXT+CC_R5($r1)
	ldo.l	$r6, TI_CPU_CONTEXT+CC_R6($r1)
	ldo.l	$r7, TI_CPU_CONTEXT+CC_R7($r1)
	ldo.l	$r8, TI_CPU_CONTEXT+CC_R8($r1)
	ldo.l	$r9, TI_CPU_CONTEXT+CC_R9($r1)
	ldo.l	$r10, TI_CPU_CONTEXT+CC_R10($r1)
	ldo.l	$r11, TI_CPU_CONTEXT+CC_R11($r1)
	ldo.l	$r12, TI_CPU_CONTEXT+CC_PC($r12)
	ldo.l	$r13, TI_CPU_CONTEXT+CC_R13($r1)
	ldo.l	$r1, TI_CPU_CONTEXT+CC_R1($r1)
	ret

.new_thread:		
	ldo.l	$sp, TI_CPU_CONTEXT+CC_SP($r1)
	ldo.l	$r0, TI_CPU_CONTEXT+CC_R0($r1)
	ldo.l	$r2, TI_CPU_CONTEXT+CC_R2($r1)
	ldo.l	$r3, TI_CPU_CONTEXT+CC_R3($r1)
	ldo.l	$r4, TI_CPU_CONTEXT+CC_R4($r1)
	ldo.l	$r5, TI_CPU_CONTEXT+CC_R5($r1)
	ldo.l	$r6, TI_CPU_CONTEXT+CC_R6($r1)
	ldo.l	$r7, TI_CPU_CONTEXT+CC_R7($r1)
	ldo.l	$r8, TI_CPU_CONTEXT+CC_R8($r1)
	ldo.l	$r9, TI_CPU_CONTEXT+CC_R9($r1)
	ldo.l	$r10, TI_CPU_CONTEXT+CC_R10($r1)
	ldo.l	$r11, TI_CPU_CONTEXT+CC_R11($r1)
	ldo.l	$r12, TI_CPU_CONTEXT+CC_PC($r1)
	ldo.l	$r13, TI_CPU_CONTEXT+CC_R13($r1)
	ldo.l	$r1, TI_CPU_CONTEXT+CC_R1($r1)
	jmp	$r12

	
	