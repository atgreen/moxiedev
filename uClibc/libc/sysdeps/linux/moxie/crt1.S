	.text
	.global _start
	.type _start, @function
_start:
	jsra	__uClibc_main

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
