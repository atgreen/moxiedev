/*
 * Preliminary support for HW exception handing for Moxie
 *
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 * Copyright (C) 2008-2009 Michal Simek <monstr@monstr.eu>
 * Copyright (C) 2008-2009 PetaLogix
 * Copyright (C) 2005 John Williams <jwilliams@itee.uq.edu.au>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License. See the file COPYING in the main directory of this
 * archive for more details.
 */

#ifndef _ASM_MOXIE_EXCEPTIONS_H
#define _ASM_MOXIE_EXCEPTIONS_H

#ifdef __KERNEL__
#ifndef __ASSEMBLY__

asmlinkage void full_exception(struct pt_regs *regs, unsigned int type,
							int fsr, int addr);

void die(const char *str, struct pt_regs *fp, long err);
void _exception(int signr, struct pt_regs *regs, int code, unsigned long addr);

#ifdef CONFIG_MMU
void __bug(const char *file, int line, void *data);
int bad_trap(int trap_num, struct pt_regs *regs);
int debug_trap(struct pt_regs *regs);
#endif /* CONFIG_MMU */

#if defined(CONFIG_KGDB)
void (*debugger)(struct pt_regs *regs);
int (*debugger_bpt)(struct pt_regs *regs);
int (*debugger_sstep)(struct pt_regs *regs);
int (*debugger_iabr_match)(struct pt_regs *regs);
int (*debugger_dabr_match)(struct pt_regs *regs);
void (*debugger_fault_handler)(struct pt_regs *regs);
#else
#define debugger(regs)			do { } while (0)
#define debugger_bpt(regs)		0
#define debugger_sstep(regs)		0
#define debugger_iabr_match(regs)	0
#define debugger_dabr_match(regs)	0
#define debugger_fault_handler		((void (*)(struct pt_regs *))0)
#endif

#endif /*__ASSEMBLY__ */
#endif /* __KERNEL__ */
#endif /* _ASM_MOXIE_EXCEPTIONS_H */
