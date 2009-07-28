/*
 * libc/sysdeps/linux/moxie/bits/setjmp.h -- microblaze version of `jmp_buf' type
 *
 *  Copyright (C) 2009  Anthony Green <green@moxielogic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License.  See the file COPYING.LIB in the main
 * directory of this archive for more details.
 *
 * Written by Anthony Green <green@moxielogic.com>
 */

#ifndef _BITS_SETJMP_H
#define _BITS_SETJMP_H	1

#if !defined _SETJMP_H && !defined _PTHREAD_H
# error "Never include <bits/setjmp.h> directly; use <setjmp.h> instead."
#endif

#ifndef _ASM
typedef struct
  {
    /* Frame pointer.  */
    void *__fp;

    /* Stack pointer.  */
    void *__sp;

    /* Other registers.  */
    int __regs[14];
  } __jmp_buf[1];
#endif

#define JB_SIZE		(4 * 16)

/* Test if longjmp to JMPBUF would unwind the frame
   containing a local variable at ADDRESS.  */
#define _JMPBUF_UNWINDS(jmpbuf, address) \
  ((void *) (address) < (void *) (jmpbuf)[0].__fp)

#endif	/* bits/setjmp.h */
