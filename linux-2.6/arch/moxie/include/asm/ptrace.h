/*
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
	moxie_reg_t r14;
	moxie_reg_t r15;
	moxie_reg_t r16;
	moxie_reg_t r17;
	moxie_reg_t r18;
	moxie_reg_t r19;
	moxie_reg_t r20;
	moxie_reg_t r21;
	moxie_reg_t r22;
	moxie_reg_t r23;
	moxie_reg_t r24;
	moxie_reg_t r25;
	moxie_reg_t r26;
	moxie_reg_t r27;
	moxie_reg_t r28;
	moxie_reg_t r29;
	moxie_reg_t r30;
	moxie_reg_t r31;
	moxie_reg_t pc;
	moxie_reg_t msr;
	moxie_reg_t ear;
	moxie_reg_t esr;
	moxie_reg_t fsr;
	int pt_mode;
};

#define kernel_mode(regs)		((regs)->pt_mode)
#define user_mode(regs)			(!kernel_mode(regs))

#define instruction_pointer(regs)	((regs)->pc)
#define profile_pc(regs)		instruction_pointer(regs)

void show_regs(struct pt_regs *);

#endif /* __ASSEMBLY__ */

#endif /* _ASM_MOXIE_PTRACE_H */
