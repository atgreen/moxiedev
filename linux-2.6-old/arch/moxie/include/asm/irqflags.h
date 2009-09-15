/*
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef _ASM_MOXIE_IRQFLAGS_H
#define _ASM_MOXIE_IRQFLAGS_H

#include <linux/irqflags.h>

/* MOXIE-FIXME */

# define local_irq_save(flags)				\
	do {						\
	  flags = 0;					\
	} while (0)

# define local_irq_disable()					\
	do {							\
	} while (0)

# define local_irq_enable()					\
	do {							\
	} while (0)

#define local_save_flags(flags)					\
	do {							\
	  flags = 0;						\
	} while (0)

#define local_irq_restore(flags)			\
	do {						\
	  flags = 0;					\
	} while (0)

static inline int irqs_disabled(void)
{
        return 0;
}

#define raw_irqs_disabled irqs_disabled
#define raw_irqs_disabled_flags(flags)	((flags) == 0)

#endif /* _ASM_MOXIE_IRQFLAGS_H */
