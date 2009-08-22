/*
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define MOXIE_EX_DIV0 0 /* Divide by zero */
#define MOXIE_EX_BAD  1 /* Illegal instruction */
#define MOXIE_EX_IRQ  2 /* Interrupt request */
#define MOXIE_EX_SWI  3 /* Software interrupt */
	
.section .text
.align 4
.global moxie_exception_handler
moxie_exception_handler:

	/* ---- Populate a pt_regs struct on the stack.  ---------- */

	sto.l	-68($sp), $sp	/* Save stack pointer.  */
	dec	$sp, 8		/* Skip pt_mode and pc */
	push	$sp, $r13
	push	$sp, $r12
	push	$sp, $r11
	push	$sp, $r10
	push	$sp, $r9
	push	$sp, $r8
	push	$sp, $r7
	push	$sp, $r6
	push	$sp, $r5
	push	$sp, $r4
	push	$sp, $r3
	push	$sp, $r2
	push	$sp, $r1
	push	$sp, $r0
	dec	$sp, 4    /* Skip over $sp, which was alread saved.  */
   	push    $sp, $fp
	/* Save the return address.  This destroys $r7, but we'll
	   just restore it again once we return from the exception
	   below.  */
	ldo.l	$r7, 4($fp)
	sto.l	64($sp), $r7
	
	/* What kind of exception is this?  The answer is stored in
	   special register 2.  */
	gsr	$r9, 2

	ldi.l	$r10, MOXIE_EX_SWI
	cmp	$r9, $r10
	bne	.return_from_exception /* Only handle swi for now. */

	/* It's a software interrupt.  The interrupt number is held in
	   special register 3.  Ignore everything except a system 
	   call.  */
	gsr	$r9, 3
	ldi.l	$r10, 0xffffffff
	cmp	$r9, $r10
	bne	.return_from_exception
	
	/* ---- Make the system call. ----------------------------- */

	/* Store pt_regs in $r4.  */
	mov	$r8, $sp
	
	/* Multiply the index by four and look up the system call
	address in our table. */

	add.l	$r6, $r6
	add.l	$r6, $r6
	ldo.l	$r6, sys_call_table($r6)
	jsr	$r6

.return_from_exception:	

	/* ---- Restore all registers from the stack. ------------- */

	pop	$sp, $fp	/* Pop frame pointer.  */
	inc	$sp, 28		/* skip $sp & $r0 through $r5.  */
	pop	$sp, $r6
	pop	$sp, $r7
	pop	$sp, $r8
	pop	$sp, $r9
	pop	$sp, $r10
	pop	$sp, $r11
	pop	$sp, $r12
	pop	$sp, $r13
	inc	$sp, 8		/* Skip pc and pt_mode.  */

	/* start_thread pushed a fake call frame on the stack, so
	   returning should send us to the new thread.  */
	ret
