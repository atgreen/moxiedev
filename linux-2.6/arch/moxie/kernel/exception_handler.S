.section .text
.align 4
.global moxie_exception_handler
moxie_exception_handler:
	
	/* Multiply the index by four and look up the system call
	address in our table. */

	add.l	$r5, $r5
	add.l	$r5, $r5
	ldo.l	$r5, sys_call_table($r5)
	jsr	$r5
	ret
