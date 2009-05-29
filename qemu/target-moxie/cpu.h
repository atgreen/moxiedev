/*
 *  moxie emulation
 *
 *  Copyright (c) 2008 Anthony Green
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _CPU_MOXIE_H
#define _CPU_MOXIE_H

#include "config.h"
#define CPUState struct CPUMOXIEState

#define TARGET_LONG_BITS 32
#define TARGET_HAS_ICE 1

#define ELF_MACHINE	0xFEED /* EM_MOXIE */

#define MOXIE_EX_DIV0    0
#define MOXIE_EX_BAD     1
#define MOXIE_EX_IRQ     2
#define MOXIE_EX_SWI     3

#define EXCP_MMU_EXEC    0
#define EXCP_MMU_READ    1
#define EXCP_MMU_WRITE   2
#define EXCP_MMU_FLUSH   3
#define EXCP_MMU_MISS    4
#define EXCP_BREAK      16 /* trap.  */

#include "cpu-defs.h"

#include "softfloat.h"

#define TARGET_PAGE_BITS 12	/* 4k XXXXX */

#define SR_MD (1 << 30)
#define SR_RB (1 << 29)
#define SR_BL (1 << 28)
#define SR_FD (1 << 15)
#define SR_M  (1 << 9)
#define SR_Q  (1 << 8)
#define SR_S  (1 << 1)
#define SR_T  (1 << 0)

#define FPSCR_FR (1 << 21)
#define FPSCR_SZ (1 << 20)
#define FPSCR_PR (1 << 19)
#define FPSCR_DN (1 << 18)
#define DELAY_SLOT             (1 << 0)
#define DELAY_SLOT_CONDITIONAL (1 << 1)
#define DELAY_SLOT_TRUE        (1 << 2)
#define DELAY_SLOT_CLEARME     (1 << 3)
/* The dynamic value of the DELAY_SLOT_TRUE flag determines whether the jump
 * after the delay slot should be taken or not. It is calculated from SR_T.
 *
 * It is unclear if it is permitted to modify the SR_T flag in a delay slot.
 * The use of DELAY_SLOT_TRUE flag makes us accept such SR_T modification.
 */

/* XXXXX The structure could be made more compact */
typedef struct tlb_t {
    uint8_t asid;		/* address space identifier */
    uint32_t vpn;		/* virtual page number */
    uint8_t v;			/* validity */
    uint32_t ppn;		/* physical page number */
    uint8_t sz;			/* page size */
    uint32_t size;		/* cached page size in bytes */
    uint8_t sh;			/* share status */
    uint8_t c;			/* cacheability */
    uint8_t pr;			/* protection key */
    uint8_t d;			/* dirty */
    uint8_t wt;			/* write through */
    uint8_t sa;			/* space attribute (PCMCIA) */
    uint8_t tc;			/* timing control */
} tlb_t;

#define UTLB_SIZE 64
#define ITLB_SIZE 4

#define NB_MMU_MODES 1

#define CC_GT  1<<0
#define CC_LT  1<<1
#define CC_EQ  1<<2
#define CC_GTU 1<<3
#define CC_LTU 1<<4

typedef struct CPUMOXIEState {


  uint32_t flags;		/* general execution flags */
  uint32_t gregs[16];		/* general registers */
  uint32_t sregs[256];		/* special registers */
  uint32_t pc;
  uint32_t cc;                /* condition codes */
  
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  
  void *irq[8];
  
  uint32_t nb_breakpoints;

  int user_mode_only;
  void *intc_handle;

  CPU_COMMON 

} CPUMOXIEState;

CPUMOXIEState *cpu_moxie_init(const char *cpu_model);
int cpu_moxie_exec(CPUMOXIEState * s);
int cpu_moxie_signal_handler(int host_signum, void *pinfo,
			       void *puc);

#include "softfloat.h"

#define cpu_init cpu_moxie_init
#define cpu_exec cpu_moxie_exec
#define cpu_gen_code cpu_moxie_gen_code
#define cpu_signal_handler cpu_moxie_signal_handler

/* MMU modes definitions */
#define MMU_MODE0_SUFFIX _kernel
#define MMU_MODE1_SUFFIX _user
#define MMU_USER_IDX 1
static inline int cpu_mmu_index (CPUState *env)
{
    return 0;
}

#include "cpu-all.h"
#include "exec-all.h"

/* Memory access type */
enum {
    /* Privilege */
    ACCESS_PRIV = 0x01,
    /* Direction */
    ACCESS_WRITE = 0x02,
    /* Type of instruction */
    ACCESS_CODE = 0x10,
    ACCESS_INT = 0x20
};

/* MMU control register */
#define MMUCR    0x1F000010
#define MMUCR_AT (1<<0)
#define MMUCR_SV (1<<8)

static inline void cpu_pc_from_tb(CPUState *env, TranslationBlock *tb)
{
    env->pc = tb->pc;
    env->flags = tb->flags;
}

static inline void cpu_get_tb_cpu_state(CPUState *env, target_ulong *pc,
                                        target_ulong *cs_base, int *flags)
{
    *pc = env->pc;
    *cs_base = 0;
    *flags = env->flags;
}

#endif				/* _CPU_MOXIE_H */
