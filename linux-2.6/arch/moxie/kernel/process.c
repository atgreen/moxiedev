/*
 * Copyright (C) 2009 Anthony Green <moxielogic.com>
 * Copyright (C) 2008-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2008-2009 PetaLogix
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/tick.h>
#include <linux/bitops.h>
#include <asm/system.h>
#include <asm/pgalloc.h>

void show_regs(struct pt_regs *regs)
{
	printk(KERN_INFO " Registers dump: mode=%X\r\n", regs->pt_mode);
	printk(KERN_INFO " fp=%08lX, sp=%08lX, r0=%08lX, r1=%08lX\n",
	                        regs->fp, regs->sp, regs->r0, regs->r1);
	printk(KERN_INFO " r2=%08lX, r3=%08lX, r4=%08lX, r5=%08lX\n",
				regs->r2, regs->r3, regs->r4, regs->r5);
	printk(KERN_INFO " r6=%08lX, r7=%08lX, r8=%08lX, r9=%08lX\n",
				regs->r6, regs->r7, regs->r8, regs->r9);
	printk(KERN_INFO " r10=%08lX, r11=%08lX, r12=%08lX, r13=%08lX\n",
				regs->r10, regs->r11, regs->r12, regs->r13);
	printk(KERN_INFO " PC=%08lX\n", regs->pc);
}

void (*pm_idle)(void);
void (*pm_power_off)(void) = NULL;
EXPORT_SYMBOL(pm_power_off);

static int hlt_counter = 1;

void disable_hlt(void)
{
	hlt_counter++;
}
EXPORT_SYMBOL(disable_hlt);

void enable_hlt(void)
{
	hlt_counter--;
}
EXPORT_SYMBOL(enable_hlt);

static int __init nohlt_setup(char *__unused)
{
	hlt_counter = 1;
	return 1;
}
__setup("nohlt", nohlt_setup);

static int __init hlt_setup(char *__unused)
{
	hlt_counter = 0;
	return 1;
}
__setup("hlt", hlt_setup);

void default_idle(void)
{
	if (!hlt_counter) {
		clear_thread_flag(TIF_POLLING_NRFLAG);
		smp_mb__after_clear_bit();
		local_irq_disable();
		while (!need_resched())
			cpu_sleep();
		local_irq_enable();
		set_thread_flag(TIF_POLLING_NRFLAG);
	} else
		while (!need_resched())
			cpu_relax();
}

void cpu_idle(void)
{
	set_thread_flag(TIF_POLLING_NRFLAG);

	/* endless idle loop with no priority at all */
	while (1) {
		void (*idle)(void) = pm_idle;

		if (!idle)
			idle = default_idle;

		tick_nohz_stop_sched_tick(1);
		while (!need_resched())
			idle();
		tick_nohz_restart_sched_tick();

		preempt_enable_no_resched();
		schedule();
		preempt_disable();
		check_pgt_cache();
	}
}

void flush_thread(void)
{
}

int copy_thread(unsigned long clone_flags, unsigned long usp,
		unsigned long unused,
		struct task_struct *p, struct pt_regs *regs)
{
	struct pt_regs *childregs = task_pt_regs(p);
	struct thread_info *ti = task_thread_info(p);

	*childregs = *regs;
	if (user_mode(regs))
		childregs->sp = usp;
	else
		childregs->sp = ((unsigned long) ti) + THREAD_SIZE;

#ifndef CONFIG_MMU
	memset(&ti->cpu_context, 0, sizeof(struct cpu_context));
	ti->cpu_context.sp = (unsigned long)childregs;

	ti->cpu_context.fp = ti->cpu_context.sp;
	childregs->fp = childregs->sp;
#else

	/* if creating a kernel thread then update the current reg (we don't
	 * want to use the parent's value when restoring by POP_STATE) */
	if (kernel_mode(regs))
		/* save new current on stack to use POP_STATE */
		childregs->CURRENT_TASK = (unsigned long)p;
	/* if returning to user then use the parent's value of this register */

	/* if we're creating a new kernel thread then just zeroing all
	 * the registers. That's OK for a brand new thread.*/
	/* Pls. note that some of them will be restored in POP_STATE */
	if (kernel_mode(regs))
		memset(&ti->cpu_context, 0, sizeof(struct cpu_context));
	/* if this thread is created for fork/vfork/clone, then we want to
	 * restore all the parent's context */
	/* in addition to the registers which will be restored by POP_STATE */
	else {
		ti->cpu_context = *(struct cpu_context *)regs;
		childregs->msr |= MSR_UMS;
	}

	/* FIXME STATE_SAVE_PT_OFFSET; */
	ti->cpu_context.sp  = (unsigned long)childregs - STATE_SAVE_ARG_SPACE;
	/* we should consider the fact that childregs is a copy of the parent
	 * regs which were saved immediately after entering the kernel state
	 * before enabling VM. This MSR will be restored in switch_to and
	 * RETURN() and we want to have the right machine state there
	 * specifically this state must have INTs disabled before and enabled
	 * after performing rtbd
	 * compose the right MSR for RETURN(). It will work for switch_to also
	 * excepting for VM and UMS
	 * don't touch UMS , CARRY and cache bits
	 * right now MSR is a copy of parent one */
	childregs->msr |= MSR_BIP;
	childregs->msr &= ~MSR_EIP;
	childregs->msr |= MSR_IE;
	childregs->msr &= ~MSR_VM;
	childregs->msr |= MSR_VMS;
	childregs->msr |= MSR_EE; /* exceptions will be enabled*/

	ti->cpu_context.msr = (childregs->msr|MSR_VM);
	ti->cpu_context.msr &= ~MSR_UMS; /* switch_to to kernel mode */
#endif
	ti->cpu_context.pc = (unsigned long) ret_from_fork;

	if (clone_flags & CLONE_SETTLS)
		;

	return 0;
}

#ifndef CONFIG_MMU
/*
 * Return saved PC of a blocked thread.
 */
unsigned long thread_saved_pc(struct task_struct *tsk)
{
	struct cpu_context *ctx =
		&(((struct thread_info *)(tsk->stack))->cpu_context);

	return (unsigned long) ctx->pc;
}
#endif

static void kernel_thread_helper(int (*fn)(void *), void *arg)
{
	fn(arg);
	do_exit(-1);
}

int kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	struct pt_regs regs;

	memset(&regs, 0, sizeof(regs));
	/* store them in non-volatile registers */
	regs.r0 = (unsigned long)fn;
	regs.r1 = (unsigned long)arg;
	regs.pc = (unsigned long)kernel_thread_helper;
	regs.pt_mode = 1;

	show_regs(&regs);

	return do_fork(flags | CLONE_VM | CLONE_UNTRACED, 0,
			&regs, 0, NULL, NULL);
}
EXPORT_SYMBOL_GPL(kernel_thread);

unsigned long get_wchan(struct task_struct *p)
{
/* TBD (used by procfs) */
	return 0;
}

/* Set up a thread for executing a new program */
void start_thread(struct pt_regs *regs, unsigned long pc, unsigned long usp)
{
	set_fs(USER_DS);
	regs->pc = pc;
	regs->sp = usp;
	regs->pt_mode = 0;
}

#ifdef CONFIG_MMU
#include <linux/elfcore.h>
/*
 * Set up a thread for executing a new program
 */
int dump_fpu(struct pt_regs *regs, elf_fpregset_t *fpregs)
{
	return 0; /* MicroBlaze has no separate FPU registers */
}
#endif /* CONFIG_MMU */