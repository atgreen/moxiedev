/* We need to call:
   __uClibc_main (int (*main) (int, char **, char **), int argc,
		      char **argv, void (*init) (void), void (*fini) (void),
		      void (*rtld_fini) (void), void *stack_end)
*/

.text
	.global _start
	.type _start, @function
	.type _init,@function
	.type _fini,@function
	.weak _init
	.weak _fini
	.type main,@function
	.type __uClibc_main,@function
_start:
	ldi.l  $r0, main	/* main */
	ldo.l  $r1, 0($sp)	/* argc */
	ldo.l  $r2, 4($sp)	/* argv */
	dec    $sp, 28
	xor    $r3, $r3		/* rtld_fini */
	sto.l  20($sp), $r3
	mov    $r3, $sp		/* stack_end */
	sto.l  24($sp), $r3
	ldi.l  $r3, _init	/* _init */
	ldi.l  $r4, _fini	/* _fini */
	jsra   __uClibc_main

/* Stick in a dummy reference to `main', so that if an application
   is linking when the `main' function is in a static library (.a)
   we can be sure that `main' actually gets linked in.  */
L_dummy_main_reference:
	.long	main

/* Define a symbol for the first piece of initialized data.  */
	.data
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start
