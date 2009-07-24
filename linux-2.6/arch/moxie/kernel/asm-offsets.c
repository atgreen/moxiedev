/*
 * Copyright (C) 2007-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2007-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/init.h>
#include <linux/stddef.h>
#include <linux/sched.h>
#include <linux/kernel_stat.h>
#include <linux/ptrace.h>
#include <linux/hardirq.h>
#include <linux/thread_info.h>
#include <linux/kbuild.h>

int main(int argc, char *argv[])
{
	/* struct pt_regs */
	DEFINE(PT_SIZE, sizeof(struct pt_regs));
	DEFINE(PT_FP, offsetof(struct pt_regs, fp));
	DEFINE(PT_SP, offsetof(struct pt_regs, sp));
	DEFINE(PT_R0, offsetof(struct pt_regs, r0));
	DEFINE(PT_R1, offsetof(struct pt_regs, r1));
	DEFINE(PT_R2, offsetof(struct pt_regs, r2));
	DEFINE(PT_R3, offsetof(struct pt_regs, r3));
	DEFINE(PT_R4, offsetof(struct pt_regs, r4));
	DEFINE(PT_R5, offsetof(struct pt_regs, r5));
	DEFINE(PT_R6, offsetof(struct pt_regs, r6));
	DEFINE(PT_R7, offsetof(struct pt_regs, r7));
	DEFINE(PT_R8, offsetof(struct pt_regs, r8));
	DEFINE(PT_R9, offsetof(struct pt_regs, r9));
	DEFINE(PT_R10, offsetof(struct pt_regs, r10));
	DEFINE(PT_R11, offsetof(struct pt_regs, r11));
	DEFINE(PT_R12, offsetof(struct pt_regs, r12));
	DEFINE(PT_R13, offsetof(struct pt_regs, r13));
	DEFINE(PT_PC, offsetof(struct pt_regs, pc));
	DEFINE(PT_MODE, offsetof(struct pt_regs, pt_mode));
	BLANK();

	/* Magic offsets for PTRACE PEEK/POKE etc */
	DEFINE(PT_TEXT_ADDR, sizeof(struct pt_regs) + 1);
	DEFINE(PT_TEXT_LEN, sizeof(struct pt_regs) + 2);
	DEFINE(PT_DATA_ADDR, sizeof(struct pt_regs) + 3);
	BLANK();

	/* struct task_struct */
	DEFINE(TS_THREAD_INFO, offsetof(struct task_struct, stack));
#ifdef CONFIG_MMU
	DEFINE(TASK_STATE, offsetof(struct task_struct, state));
	DEFINE(TASK_FLAGS, offsetof(struct task_struct, flags));
	DEFINE(TASK_PTRACE, offsetof(struct task_struct, ptrace));
	DEFINE(TASK_BLOCKED, offsetof(struct task_struct, blocked));
	DEFINE(TASK_MM, offsetof(struct task_struct, mm));
	DEFINE(TASK_ACTIVE_MM, offsetof(struct task_struct, active_mm));
	DEFINE(TASK_PID, offsetof(struct task_struct, pid));
	DEFINE(TASK_THREAD, offsetof(struct task_struct, thread));
	DEFINE(THREAD_KSP, offsetof(struct thread_struct, ksp));
	BLANK();

	DEFINE(PGDIR, offsetof(struct thread_struct, pgdir));
	BLANK();
#endif

	/* struct thread_info */
	DEFINE(TI_TASK, offsetof(struct thread_info, task));
	DEFINE(TI_FLAGS, offsetof(struct thread_info, flags));
	DEFINE(TI_ADDR_LIMIT, offsetof(struct thread_info, addr_limit));
	DEFINE(TI_CPU_CONTEXT, offsetof(struct thread_info, cpu_context));
	BLANK();

	/* struct cpu_context */
	DEFINE(CC_FP, offsetof(struct cpu_context, fp)); /* frame pointer */
	DEFINE(CC_SP, offsetof(struct cpu_context, sp)); /* stack pointer */
	DEFINE(CC_R0, offsetof(struct cpu_context, r0)); 
	DEFINE(CC_R1, offsetof(struct cpu_context, r1)); 
	DEFINE(CC_R2, offsetof(struct cpu_context, r2));
	DEFINE(CC_R3, offsetof(struct cpu_context, r3));
	DEFINE(CC_R4, offsetof(struct cpu_context, r4));
	DEFINE(CC_R5, offsetof(struct cpu_context, r5));
	DEFINE(CC_R6, offsetof(struct cpu_context, r6));
	DEFINE(CC_R7, offsetof(struct cpu_context, r7));
	DEFINE(CC_R8, offsetof(struct cpu_context, r8));
	DEFINE(CC_R9, offsetof(struct cpu_context, r9));
	DEFINE(CC_R10, offsetof(struct cpu_context, r10));
	DEFINE(CC_R11, offsetof(struct cpu_context, r11));
	DEFINE(CC_R12, offsetof(struct cpu_context, r12));
	DEFINE(CC_R13, offsetof(struct cpu_context, r13));
	DEFINE(CC_PC, offsetof(struct cpu_context, pc));
	BLANK();

	return 0;
}
