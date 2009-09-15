/*
 * Copyright (C) 2009 Anthony Green
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef _ASM_MOXIE_PTRACE_H
#define _ASM_MOXIE_PTRACE_H

#ifndef __ASSEMBLY__

typedef unsigned long moxie_reg_t;

struct pt_regs {
	moxie_reg_t fp;
	moxie_reg_t sp;
	moxie_reg_t r0;
	moxie_reg_t r1;
	moxie_reg_t r2;
	moxie_reg_t r3;
	moxie_reg_t r4;
	moxie_reg_t r5;
	moxie_reg_t r6;
	moxie_reg_t r7;
	moxie_reg_t r8;
	moxie_reg_t r9;
	moxie_reg_t r10;
	moxie_reg_t r11;
	moxie_reg_t r12;
	moxie_reg_t r13;
	moxie_reg_t pc;
	int pt_mode;
};

#define kernel_mode(regs)		((regs)->pt_mode)
#define user_mode(regs)			(!kernel_mode(regs))

#define instruction_pointer(regs)	((regs)->pc)
#define profile_pc(regs)		instruction_pointer(regs)

void show_regs(struct pt_regs *);

#endif /* __ASSEMBLY__ */

#endif /* _ASM_MOXIE_PTRACE_H */
