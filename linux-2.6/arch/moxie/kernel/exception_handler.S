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
	/* Save the return address.  This destroys $r6, but we'll
	   just restore it again once we return from the system call
	   below.  */
	ldo.l	$r6, 4($fp)
	sto.l	64($sp), $r6
	
	/* ---- Make the system call. ----------------------------- */

	/* Store pt_regs in $r4.  */
	mov	$r4, $sp
	
	/* Multiply the index by four and look up the system call
	address in our table. */

	add.l	$r5, $r5
	add.l	$r5, $r5
	ldo.l	$r5, sys_call_table($r5)
	jsr	$r5

	/* ---- Restore all registers from the stack. ------------- */

	/* First comes the return address.  Let's stick it in $r2
	   for now.  We don't assume that $r2 is preserved over
	   system calls.  */
	pop	$sp, $fp
	inc	$sp, 24		/* skip $sp, $r0 through $r4 for now.  */
	pop	$sp, $r5
	pop	$sp, $r6
	pop	$sp, $r7
	pop	$sp, $r8
	pop	$sp, $r9
	pop	$sp, $r10
	pop	$sp, $r11
	pop	$sp, $r12
	pop	$sp, $r13

	/* Instead of returning normally, let's fake a return, because
	   we may not be returning to where we were expecting to go.
	   Just pop the frame pointer and throw away the return address
	   and static chain value.  We'll use the return address that
	   was saved in the pt_regs structure.  */
	pop	$sp, $r2	/* Pop the pc.  */
	dec	$sp, 4		/* Skip pt_mode.  */
	pop	$sp, $fp	/* Pop $fp from call stack.  */
	inc	$sp, 8		/* Skip return address and static chain.  */
	jmp	$r2
