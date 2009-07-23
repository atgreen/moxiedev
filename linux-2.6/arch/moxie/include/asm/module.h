/*
 * Copyright (C) 2006 Atmark Techno, Inc.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */

#ifndef _ASM_MOXIE_MODULE_H
#define _ASM_MOXIE_MODULE_H

#include <asm-generic/module.h>

/* Moxie Relocations */
#define R_MOXIE_NONE 0
#define R_MOXIE_32 1
#define R_MOXIE_32_PCREL 2
#define R_MOXIE_64_PCREL 3
#define R_MOXIE_32_PCREL_LO 4
#define R_MOXIE_64 5
#define R_MOXIE_32_LO 6
#define R_MOXIE_SRO32 7
#define R_MOXIE_SRW32 8
#define R_MOXIE_64_NONE 9
#define R_MOXIE_32_SYM_OP_SYM 10
/* Keep this the last entry. */
#define R_MOXIE_NUM 11

typedef struct { volatile int counter; } module_t;

#endif /* _ASM_MOXIE_MODULE_H */
