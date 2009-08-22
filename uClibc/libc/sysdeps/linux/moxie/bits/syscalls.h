#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#ifndef __ASSEMBLER__

#include <errno.h>

#define ASMFMT_0
#define ASMFMT_1 \
	, "r" (r0)
#define ASMFMT_2 \
	, "r" (r0), "r" (r1)
#define ASMFMT_3 \
	, "r" (r0), "r" (r1), "r" (r2)
#define ASMFMT_4 \
	, "r" (r0), "r" (r1), "r" (r2), "r" (r3)
#define ASMFMT_5 \
	, "r" (r0), "r" (r1), "r" (r2), "r" (r3), "0" (r4)
#define ASMFMT_6 \
	, "r" (r0), "r" (r1), "r" (r2), "r" (r3), "0" (r4), "r" (r5)
#define ASMFMT_7 \
	, "r" (r0), "r" (r1), "r" (r2), "r" (r3), "0" (r4), "r" (r5), "r" (r6)

#define SUBSTITUTE_ARGS_0()
#define SUBSTITUTE_ARGS_1(arg1)					\
  long int _arg1 = (long int) (arg1);				\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1)
#define SUBSTITUTE_ARGS_2(arg1, arg2)				\
  long int _arg1 = (long int) (arg1);				\
  long int _arg2 = (long int) (arg2);				\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1);	\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2)
#define SUBSTITUTE_ARGS_3(arg1, arg2, arg3)			\
  long int _arg1 = (long int) (arg1);				\
  long int _arg2 = (long int) (arg2);				\
  long int _arg3 = (long int) (arg3);				\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1);	\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2);	\
  register long int r2 __asm__ ("$r2") = (long int) (_arg3)
#define SUBSTITUTE_ARGS_4(arg1, arg2, arg3, arg4)		\
  long int _arg1 = (long int) (arg1);				\
  long int _arg2 = (long int) (arg2);				\
  long int _arg3 = (long int) (arg3);				\
  long int _arg4 = (long int) (arg4);				\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1);	\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2);	\
  register long int r2 __asm__ ("$r2") = (long int) (_arg3);	\
  register long int r3 __asm__ ("$r3") = (long int) (_arg4)
#define SUBSTITUTE_ARGS_5(arg1, arg2, arg3, arg4, arg5)		\
  long int _arg1 = (long int) (arg1);				\
  long int _arg2 = (long int) (arg2);				\
  long int _arg3 = (long int) (arg3);				\
  long int _arg4 = (long int) (arg4);				\
  long int _arg5 = (long int) (arg5);				\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1);	\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2);	\
  register long int r2 __asm__ ("$r2") = (long int) (_arg3);	\
  register long int r3 __asm__ ("$r3") = (long int) (_arg4);	\
  register long int r4 __asm__ ("$r4") = (long int) (_arg5)
#define SUBSTITUTE_ARGS_6(arg1, arg2, arg3, arg4, arg5, arg6)	\
  long int _arg1 = (long int) (arg1);				\
  long int _arg2 = (long int) (arg2);				\
  long int _arg3 = (long int) (arg3);				\
  long int _arg4 = (long int) (arg4);				\
  long int _arg5 = (long int) (arg5);				\
  long int _arg6 = (long int) (arg6);				\
  register long int r0 __asm__ ("$r0") = (long int)(_arg1);	\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2);	\
  register long int r2 __asm__ ("$r2") = (long int) (_arg3);	\
  register long int r3 __asm__ ("$r3") = (long int) (_arg4);	\
  register long int r4 __asm__ ("$r4") = (long int) (_arg5);	\
  register long int r5 __asm__ ("$r5") = (long int) (_arg6)
#define SUBSTITUTE_ARGS_7(arg1, arg2, arg3, arg4, arg5, arg6, arg7)	\
  long int _arg1 = (long int) (arg1);					\
  long int _arg2 = (long int) (arg2);					\
  long int _arg3 = (long int) (arg3);					\
  long int _arg4 = (long int) (arg4);					\
  long int _arg5 = (long int) (arg5);					\
  long int _arg6 = (long int) (arg6);					\
  long int _arg7 = (long int) (arg7);					\
  register long int r0 __asm__ ("$r0") = (long int) (_arg1);		\
  register long int r1 __asm__ ("$r1") = (long int) (_arg2);		\
  register long int r2 __asm__ ("$r2") = (long int) (_arg3);		\
  register long int r3 __asm__ ("$r3") = (long int) (_arg4);		\
  register long int r4 __asm__ ("$r4") = (long int) (_arg5);		\
  register long int r5 __asm__ ("$r5") = (long int) (_arg6);		\
  register long int r5 __asm__ ("$r6") = (long int) (_arg7)

/* The _NCS variant allows non-constant syscall numbers.  */
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({									\
    register unsigned long int resultvar __asm__ ("$r0");		\
    register long int scnum __asm__ ("$r6") = (name);			\
    SUBSTITUTE_ARGS_##nr(args);						\
    									\
    __asm__ volatile ("swi 0xffffffff"					\
		      : "=r" (resultvar)				\
		      : "r" (scnum) ASMFMT_##nr				\
		      : "memory");					\
    									\
    (int) resultvar; })

#define INTERNAL_SYSCALL_ERROR_P(val, err)	\
  ((unsigned int) (val) >= 0xfffff001u)

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */

