/*
 * Copyright (C) 2009 Anthony Green <green@moxielogic.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef _ASM_MOXIE_CURRENT_H
#define _ASM_MOXIE_CURRENT_H

/*
 * Register used to hold the current task pointer while in the kernel.
 * Any `call clobbered' register without a special meaning should be OK,
 * but check asm/moxie/kernel/entry.S to be sure.
 */
#define CURRENT_TASK	$r13
# ifndef __ASSEMBLY__
/*
 * Dedicate r31 to keeping the current task pointer
 */
register struct task_struct *current asm("$r13");

# define get_current()	current
# endif /* __ASSEMBLY__ */

#endif /* _ASM_MOXIE_CURRENT_H */
