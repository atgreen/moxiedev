/*
 *  Moxie emulation for qemu: main translation routines.
 *
 *  Copyright (c) 2009 Anthony Green
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA
 * 02110-1301 USA
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#if 0
#define DEBUG_DISAS

#define DISAS_MOXIE 1
#if DISAS_MOXIE
#define DIS(x) x
#else
#define DIS(x)
#endif
#endif

#include "cpu.h"
#include "exec-all.h"
#include "disas.h"
#include "tcg-op.h"

#include "helper.h"
#define GEN_HELPER 1
#include "helper.h"

#ifdef USE_DIRECT_JUMP
#define TBPARAM(x)
#else
#define TBPARAM(x) (long)(x)
#endif

/* This is the state at translation time.  */
typedef struct DisasContext {
  struct TranslationBlock *tb;
  target_ulong pc, saved_pc;
  uint32_t opcode;
  uint32_t fp_status;
  /* Routine used to access memory */
  int memidx;
  uint32_t hflags, saved_hflags;
  int bstate;
  target_ulong btarget;
  void *last_T0_store;
  int last_T0_gpr;
  int singlestep_enabled;
} DisasContext;

enum {
  BS_NONE     = 0, /* We go out of the TB without reaching a branch or an
		    * exception condition */
  BS_STOP     = 1, /* We want to stop translation for any reason */
  BS_BRANCH   = 2, /* We reached a branch condition     */
  BS_EXCP     = 3, /* We reached an exception condition */
};

static TCGv cpu_pc;
static TCGv cpu_gregs[16];
static TCGv cpu_sregs[256];
static TCGv_ptr cpu_env;
static TCGv cc;

#define REG(x) (cpu_gregs[x])
#define SREG(x) (cpu_sregs[x])

/* The code generator doesn't like lots of temporaries, so maintain our own
   cache for reuse within a function.  */
#define MAX_TEMPS 8
static int num_temps;
static TCGv temps[MAX_TEMPS];

/* Allocate a temporary variable.  */
static TCGv_i32 new_tmp(void)
{
  TCGv tmp;
  if (num_temps == MAX_TEMPS)
    abort();

  if (GET_TCGV_I32(temps[num_temps]))
    return temps[num_temps++];

  tmp = tcg_temp_new_i32();
  temps[num_temps++] = tmp;
  return tmp;
}

/* Release a temporary variable.  */
static void dead_tmp(TCGv tmp)
{
  int i;
  num_temps--;
  i = num_temps;
  if (TCGV_EQUAL(temps[i], tmp))
    return;

  /* Shuffle this temp to the last slot.  */
  while (!TCGV_EQUAL(temps[i], tmp))
    i--;
  while (i < num_temps) {
    temps[i] = temps[i + 1];
    i++;
  }
  temps[i] = tmp;
}

void cpu_dump_state(CPUState * env, FILE * f,
		    int (*cpu_fprintf) (FILE * f, const char *fmt, ...),
		    int flags)
{
  int i;
  cpu_fprintf(f, "pc=0x%08x\n", env->pc);
  cpu_fprintf(f, "$fp=0x%08x $sp=0x%08x $r0=0x%08x $r1=0x%08x\n",
	      env->gregs[0], env->gregs[1], env->gregs[2], env->gregs[3]);
  for (i = 4; i < 16; i += 4) {
    cpu_fprintf(f, "$r%d=0x%08x $r%d=0x%08x $r%d=0x%08x $r%d=0x%08x\n",
		i-2, env->gregs[i], i-1, env->gregs[i + 1],
		i, env->gregs[i + 2], i+1, env->gregs[i + 3]);
  }
  cpu_fprintf(f, "sr0=0x%08x $r1=0x%08x\n",
	      env->sregs[0], env->sregs[1]);
  
}

static void moxie_translate_init(void)
{
  int i;
  static int done_init = 0;
  cpu_env = tcg_global_reg_new_ptr(TCG_AREG0, "env");
  static const char * const gregnames[16] = {
    "$fp", "$sp", "$r0", "$r1", 
    "$r2", "$r3", "$r4", "$r5", 
    "$r6", "$r7", "$r8", "$r9", 
    "$r10", "$r11", "$r12", "$r13"
  };
  
  static const char * const sregnames[256] = {
    "$sw", "$ea", "$sr2", "$sr3", "$s4", "$sr5", "$sr6", "$sr7", "$sr8", "$sr9", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", "$srX", 
    "$srX", "$srX", "$srX", "$srX", "$srX", "$srX" 
  };
  
  if (done_init)
    return;
  cpu_pc = tcg_global_mem_new_i32(TCG_AREG0,
				  offsetof(CPUState, pc), "$pc");
  for (i = 0; i < 24; i++)
    cpu_gregs[i] = tcg_global_mem_new_i32(TCG_AREG0,
					  offsetof(CPUState, gregs[i]),
					  gregnames[i]);

  for (i = 0; i < 256; i++)
    cpu_sregs[i] = tcg_global_mem_new_i32(TCG_AREG0,
					  offsetof(CPUState, sregs[i]),
					  sregnames[i]);

  cc = tcg_global_mem_new_i32(TCG_AREG0, offsetof(CPUState, cc), "cc");

  done_init = 1;
}

CPUMOXIEState *cpu_moxie_init (const char *cpu_model)
{
  CPUMOXIEState *env;
  
  env = qemu_mallocz(sizeof(CPUMOXIEState));
  if (!env)
    return NULL;
  cpu_exec_init(env);

  moxie_translate_init();

  cpu_reset(env);
  return env;
}

void cpu_reset (CPUMOXIEState *env)
{
  memset(env, 0, offsetof(CPUMOXIEState, breakpoints));
  tlb_flush(env, 1);
}

static always_inline void gen_save_pc(target_ulong pc)
{
  gen_op_save_pc(pc);
}

static always_inline void restore_cpu_state (CPUState *env, DisasContext *ctx)
{
  ctx->saved_hflags = ctx->hflags;
}

static always_inline void save_cpu_state (DisasContext *ctx, int do_save_pc)
{

}

static always_inline void gen_goto_tb(DisasContext *ctx, int n, target_ulong dest)
{
  TranslationBlock *tb;
  tb = ctx->tb;

  if ((tb->pc & TARGET_PAGE_MASK) == (dest & TARGET_PAGE_MASK) &&
      !ctx->singlestep_enabled) {
    tcg_gen_goto_tb(n);
    tcg_gen_movi_i32(cpu_pc, dest);
    tcg_gen_exit_tb((long) tb + n);
  } else {
    tcg_gen_movi_i32(cpu_pc, dest);
    if (ctx->singlestep_enabled)
      gen_helper_debug();
    tcg_gen_exit_tb(0);
  }
}

/* Create a new temporary and set it to the value of a CPU register.  */
static inline TCGv load_reg(DisasContext *s, int reg)
{
  TCGv tmp = new_tmp();
  tcg_gen_ld_i32(tmp, cpu_env, offsetof(CPUState, gregs[reg]));
  return tmp;
}

static int decode_opc (CPUState *env, DisasContext *ctx)
{
  /* Local cache for the instruction opcode.  */
  int opcode;
  /* Set the default instruction length.  */
  int length = 2;

  /* Examine the 16-bit opcode.  */
  opcode = ctx->opcode;

  /* Decode instruction.  */
  if (opcode & (1 << 15))
    {
      if (opcode & (1 << 14))
	{
	  /* This is a Form 3 instruction.  */
	  /* We haven't implemented any yet.  */
	}
      else
	{
	  /* This is a Form 2 instruction.  */
	  int inst = (opcode >> 12 & 0x3);
	  switch (inst)
	    {
	    case 0x00: /* inc */
	      {
		int a = (opcode >> 8) & 0xf;
		unsigned int v = (opcode & 0xff);
		
		TCGv t1 = new_tmp();
		tcg_gen_addi_i32(t1, REG(a), v);
		tcg_gen_mov_i32(REG(a), t1);
		dead_tmp(t1);
	      }
	      break;
	    case 0x01: /* dec */
	      {
		int a = (opcode >> 8) & 0xf;
		unsigned int v = (opcode & 0xff);
		
		TCGv t1 = new_tmp();
		tcg_gen_subi_i32(t1, REG(a), v);
		tcg_gen_mov_i32(REG(a), t1);
		dead_tmp(t1);
	      }
	      break;
	    case 0x02: /* gsr */
	      {
		int a = (opcode >> 8) & 0xf;
		unsigned v = (opcode & 0xff);
		tcg_gen_mov_i32(REG(a), SREG(v));
	      }
	      break;
	    case 0x03: /* ssr */
	      {
		int a = (opcode >> 8) & 0xf;
		unsigned v = (opcode & 0xff);
		tcg_gen_mov_i32(SREG(v), REG(a));
	      }
	      break;
	    default:
	      printf("* 0x%x\tForm 2 ********** 0x%x\n", ctx->pc, inst); 
	      cpu_dump_state(env, logfile, fprintf, 0);
	      abort();
	      break;
	    }
	}
    }
  else
    {
      /* This is a Form 1 instruction.  */
      int inst = opcode >> 8;
      switch (inst)
	{
	case 0x00: /* nop */
	  break;
	case 0x01: /* ldi.l (immediate) */
	  {
	    int reg = (opcode >> 4) & 0xf;
	    int val = ldl_code(ctx->pc+2);
	    tcg_gen_movi_i32(REG(reg), val);
	    length = 6;
	  }
	  break;
	case 0x02: /* mov (register-to-register) */
	  {
	    int dest  = (opcode >> 4) & 0xf;
	    int src = opcode & 0xf;
	    tcg_gen_mov_i32(REG(dest), REG(src));
	  }
	  break;
	case 0x03: /* jsra */
	  {
	    /* Load the stack pointer into T0.  */
	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();

	    tcg_gen_movi_i32(t1, ctx->pc+6);

	    /* Make space for the static chain and return address.  */
	    tcg_gen_subi_i32(t2, REG(1), 8);
	    tcg_gen_mov_i32(REG(1), t2);
    	    tcg_gen_qemu_st32(t1, REG(1), ctx->memidx);

	    /* Push the current frame pointer.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
	    tcg_gen_mov_i32(REG(1), t2);
	    tcg_gen_qemu_st32(REG(0), REG(1), ctx->memidx);
			      
	    /* Set the pc and $fp.  */
	    tcg_gen_mov_i32(REG(0), REG(1));

	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));

	    dead_tmp(t1);
	    dead_tmp(t2);
	    
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x04: /* ret */
	  {
	    TCGv t1 = new_tmp();

	    /* The new $sp is the old $fp.  */
	    tcg_gen_mov_i32(REG(1), REG(0));
	    
	    /* Pop the frame pointer.  */
	    tcg_gen_qemu_ld32u(REG(0), REG(1), ctx->memidx);
	    tcg_gen_addi_i32(t1, REG(1), 4);
	    tcg_gen_mov_i32(REG(1), t1);


	    /* Pop the return address and skip over the static chain
	       slot.  */
	    tcg_gen_qemu_ld32u(cpu_pc, REG(1), ctx->memidx);
	    tcg_gen_addi_i32(t1, REG(1), 8);
	    tcg_gen_mov_i32(REG(1), t1);
	    
	    dead_tmp(t1);

	    /* Jump... */
	    tcg_gen_exit_tb(0);

	    ctx->bstate = BS_BRANCH;
	  }
	  break;
	case 0x05: /* add.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_add_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x06: /* push */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    tcg_gen_subi_i32(t1, REG(a), 4);
	    tcg_gen_mov_i32(REG(a), t1);
	    tcg_gen_qemu_st32(REG(b), REG(a), ctx->memidx);
	    dead_tmp(t1);
	  }
	  break;
	case 0x07: /* pop */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
	    TCGv t1 = new_tmp();
	
	    tcg_gen_qemu_ld32u(REG(b), REG(a), ctx->memidx);
	    tcg_gen_addi_i32(t1, REG(a), 4);
	    tcg_gen_mov_i32(REG(a), t1);
	    dead_tmp(t1);
	  }
	  break;
	case 0x08: /* lda.l */
	  {
	    int reg = (opcode >> 4) & 0xf;
	    
	    TCGv ptr = new_tmp();
	    tcg_gen_movi_i32(ptr, ldl_code(ctx->pc+2));
	    tcg_gen_qemu_ld32u(REG(reg), ptr, ctx->memidx);
	    dead_tmp(ptr);

	    length = 6;
	  }
	  break;
	case 0x09: /* sta.l */
	  {
	    int val = (opcode >> 4) & 0xf;
	    
	    TCGv ptr = new_tmp();
	    tcg_gen_movi_i32(ptr, ldl_code(ctx->pc+2));
	    tcg_gen_qemu_st32(REG(val), ptr, ctx->memidx);
	    dead_tmp(ptr);

	    length = 6;
	  }
	  break;
	case 0x0a: /* ld.l (register indirect) */
	  {
	    int src  = opcode & 0xf;
	    int dest = (opcode >> 4) & 0xf;

	    tcg_gen_qemu_ld32u(REG(dest), REG(src), ctx->memidx);
	  }
	  break;
	case 0x0b: /* st.l */
	  {
	    int dest = (opcode >> 4) & 0xf;
	    int val  = opcode & 0xf;

	    tcg_gen_qemu_st32(REG(val), REG(dest), ctx->memidx);
	  }
	  break;
	case 0x0c: /* ldo.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(b), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_ld32u(t2, t1, ctx->memidx);
	    tcg_gen_mov_i32(REG(a), t2);

	    dead_tmp(t1);
	    dead_tmp(t2);
	    
	    length = 6;
	  }
	  break;
	case 0x0d: /* sto.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(a), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_st32(REG(b), t1, ctx->memidx);

	    dead_tmp(t1);
	    dead_tmp(t2);

	    length = 6;
	  }
	  break;
	case 0x0e: /* cmp */
	  {
	    int a  = (opcode >> 4) & 0xf;
	    int b  = opcode & 0xf;

	    int label_equal = gen_new_label();
	    int label_done = gen_new_label();

	    /* Clear CC */
	    tcg_gen_movi_i32(cc, 0);

	    tcg_gen_brcond_i32(TCG_COND_EQ, REG(a), REG(b), label_equal);

#define CMPTEST(t,CODE)						\
	    {							\
	      int lyes = gen_new_label();			\
	      int lno = gen_new_label();			\
	      TCGv t1 = new_tmp();				\
	      tcg_gen_brcond_i32(t, REG(a), REG(b), lyes);	\
	      tcg_gen_br(lno);					\
	      gen_set_label(lyes);				\
	      tcg_gen_ori_i32(t1, cc, CODE);			\
	      tcg_gen_mov_i32(cc, t1);				\
	      gen_set_label(lno);				\
	      dead_tmp(t1);					\
	    }
	    CMPTEST(TCG_COND_LT,CC_LT);
	    CMPTEST(TCG_COND_GT,CC_GT);
	    CMPTEST(TCG_COND_LTU,CC_LTU); 
	    CMPTEST(TCG_COND_GTU,CC_GTU);
	    tcg_gen_br(label_done);
	    gen_set_label(label_equal);
	    tcg_gen_movi_i32(cc, CC_EQ);
	    gen_set_label(label_done);
	  }
	  break;
	case 0x0f: /* beq */
	  {
	    int l1 = gen_new_label();
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, cc, CC_EQ, l1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x10: /* bne */
	  {
	    int l1 = gen_new_label();
	    tcg_gen_brcondi_i32 (TCG_COND_NE, cc, CC_EQ, l1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x11: /* blt */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_LT);
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, t1, CC_LT, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x12: /* bgt */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_GT);
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, t1, CC_GT, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x13: /* bltu */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_LTU);
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, t1, CC_LTU, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x14: /* bgtu */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_GTU);
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, t1, CC_GTU, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x15: /* bge */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_GT|CC_EQ);
	    tcg_gen_brcondi_i32 (TCG_COND_GT, t1, 0, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x16: /* ble */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_LT|CC_EQ);
	    tcg_gen_brcondi_i32 (TCG_COND_GT, t1, 0, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x17: /* bgeu */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_GTU|CC_EQ);
	    tcg_gen_brcondi_i32 (TCG_COND_GT, t1, 0, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x18: /* bleu */
	  {
	    int l1 = gen_new_label();
	    TCGv t1 = new_tmp();
	    tcg_gen_andi_i32(t1, cc, CC_LTU|CC_EQ);
	    tcg_gen_brcondi_i32 (TCG_COND_GT, t1, 0, l1);
	    dead_tmp(t1);
	    gen_goto_tb(ctx, 1, ctx->pc+6);
	    gen_set_label(l1);
	    gen_goto_tb(ctx, 0, ldl_code(ctx->pc+2));
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x19: /* jsr */
	  {
	    int fnreg = (opcode >> 4) & 0xf;

	    /* Load the stack pointer into T0.  */
	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();

	    tcg_gen_movi_i32(t1, ctx->pc+2);

	    /* Make space for the static chain and return address.  */
	    tcg_gen_subi_i32(t2, REG(1), 8);
	    tcg_gen_mov_i32(REG(1), t2);
    	    tcg_gen_qemu_st32(t1, REG(1), ctx->memidx);
	    
	    /* Push the current frame pointer.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
	    tcg_gen_mov_i32(REG(1), t2);
	    tcg_gen_qemu_st32(REG(0), REG(1), ctx->memidx);
			      
	    /* Set the pc and $fp.  */
	    tcg_gen_mov_i32(REG(0), REG(1));
	    tcg_gen_mov_i32(cpu_pc, REG(fnreg));
	    dead_tmp(t1);
	    dead_tmp(t2);
	    tcg_gen_exit_tb(0);
	    ctx->bstate = BS_BRANCH;
	  }
	  break;
	case 0x1a: /* jmpa */
	  {
	    tcg_gen_movi_i32(cpu_pc, ldl_code(ctx->pc+2));
	    tcg_gen_exit_tb(0);
	    ctx->bstate = BS_BRANCH;
	    length = 6;
	  }
	  break;
	case 0x1b: /* ldi.b (immediate) */
	  {
	    int reg = (opcode >> 4) & 0xf;
	    int val = ldl_code(ctx->pc+2);
	    tcg_gen_movi_i32(REG(reg), val);
	    length = 6;
	  }
	  break;
	case 0x1c: /* ld.b (register indirect) */
	  {
	    int src  = opcode & 0xf;
	    int dest = (opcode >> 4) & 0xf;

	    tcg_gen_qemu_ld8u(REG(dest), REG(src), ctx->memidx);
	  }
	  break;
	case 0x1d: /* lda.b */
	  {
	    int reg = (opcode >> 4) & 0xf;
	    
	    TCGv ptr = new_tmp();
	    tcg_gen_movi_i32(ptr, ldl_code(ctx->pc+2));
	    tcg_gen_qemu_ld8u(REG(reg), ptr, ctx->memidx);
	    dead_tmp(ptr);

	    length = 6;
	  }
	  break;
	case 0x1e: /* st.b */
	  {
	    int dest = (opcode >> 4) & 0xf;
	    int val  = opcode & 0xf;

	    tcg_gen_qemu_st8(REG(val), REG(dest), ctx->memidx);
	  }
	  break;
	case 0x1f: /* sta.b */
	  {
	    int val = (opcode >> 4) & 0xf;
	    
	    TCGv ptr = new_tmp();
	    tcg_gen_movi_i32(ptr, ldl_code(ctx->pc+2));
	    tcg_gen_qemu_st8(REG(val), ptr, ctx->memidx);
	    dead_tmp(ptr);

	    length = 6;
	  }
	  break;
	case 0x20: /* ldi.s (immediate) */
	  {
	    int reg = (opcode >> 4) & 0xf;
	    int val = ldl_code(ctx->pc+2);
	    tcg_gen_movi_i32(REG(reg), val);
	    length = 6;
	  }
	  break;
	case 0x21: /* ld.s (register indirect) */
	  {
	    int src  = opcode & 0xf;
	    int dest = (opcode >> 4) & 0xf;

	    tcg_gen_qemu_ld16u(REG(dest), REG(src), ctx->memidx);
	  }
	  break;
#if 0
	case 0x22: /* lda.s */
	  {
	    int reg = (inst >> 4) & 0xf;
	    unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
	    TRACE("lda.s");
	    cpu.asregs.regs[reg] = rsat (opc, addr);
	    pc += 4;
	  }
	  break;
	case 0x23: /* st.s */
	  {
	    int dest = (inst >> 4) & 0xf;
	    int val  = inst & 0xf;
	    TRACE("st.s");
	    wsat (opc, cpu.asregs.regs[dest], cpu.asregs.regs[val]);
	  }
	  break;
	case 0x24: /* sta.s */
	  {
	    int reg = (inst >> 4) & 0xf;
	    unsigned int addr = EXTRACT_WORD(&memory[pc+2]);
	    TRACE("sta.s");
	    wsat (opc, addr, cpu.asregs.regs[reg]);
	    pc += 4;
	  }
	  break;
	case 0x25: /* jmp */
	  {
	    int reg = (inst >> 4) & 0xf;
	    TRACE("jmp");
	    pc = cpu.asregs.regs[reg] - 2;
	  }
	  break;
#endif
	case 0x26: /* and */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
	    TCGv t1 = new_tmp();
	    tcg_gen_and_i32(t1, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), t1);
	    dead_tmp(t1);
	  }
	  break;
	case 0x27: /* lshr */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_shr_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x28: /* ashl */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_shl_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x29: /* sub.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
		
	    TCGv result = new_tmp();
	    tcg_gen_sub_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x2a: /* neg */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
		
	    tcg_gen_neg_i32(REG(a), REG(b));
	  }
	  break;
	case 0x2b: /* or */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
	    TCGv t1 = new_tmp();
	    tcg_gen_or_i32(t1, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), t1);
	    dead_tmp(t1);
	  }
	  break;
	case 0x2c: /* not */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
	    tcg_gen_not_i32(REG(a), REG(b));
	  }
	  break;
	case 0x2d: /* ashr */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_sar_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x2e: /* xor */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;
	    TCGv t1 = new_tmp();
	    tcg_gen_xor_i32(t1, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), t1);
	    dead_tmp(t1);
	  }
	  break;
	case 0x2f: /* mul.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_mul_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x30: /* swi */
	  {
	    int val = ldl_code(ctx->pc+2);
	    TCGv code = new_tmp();

	    /* Load the stack pointer into T0.  */
	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();

	    tcg_gen_movi_i32(SREG(2), MOXIE_EX_SWI);
	    tcg_gen_movi_i32(code, val);
	    tcg_gen_mov_i32(SREG(3), code);

	    tcg_gen_movi_i32(t1, ctx->pc+2);

	    /* Make space for the static chain and return address.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
    	    tcg_gen_qemu_st32(code, t2, ctx->memidx);

	    tcg_gen_subi_i32(REG(1), t2, 4);
    	    tcg_gen_qemu_st32(t1, REG(1), ctx->memidx);
	    
	    /* Push the current frame pointer.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
	    tcg_gen_mov_i32(REG(1), t2);
	    tcg_gen_qemu_st32(REG(0), REG(1), ctx->memidx);
			      
	    /* Set the pc and $fp.  */
	    tcg_gen_mov_i32(REG(0), REG(1));
	    tcg_gen_mov_i32(cpu_pc, SREG(1));
	    dead_tmp(t1);
	    dead_tmp(t2);
	    dead_tmp(code);
	    tcg_gen_exit_tb(0);
	    ctx->bstate = BS_BRANCH;

	    length = 6;
	  }
	  break;
#if 0
	case 0x31: /* div.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    TCGv code = new_tmp();
	    TCGv result = new_tmp();

	    int l1 = gen_new_label();
	    int l2 = gen_new_label();
	    tcg_gen_brcondi_i32 (TCG_COND_EQ, REG(b), 0, l1);

	    tcg_gen_div_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    tcg_gen_br(l2);

	    gen_set_label(l1);
	    
	    /* Divide by zero */
	    /* Load the stack pointer into T0.  */
	    tcg_gen_movi_i32(code, MOXIE_EX_DIV0);
	    tcg_gen_mov_i32(SREG(2), code);

	    tcg_gen_movi_i32(t1, ctx->pc+2);

	    /* Make space for the static chain and return address.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
    	    tcg_gen_qemu_st32(code, t2, ctx->memidx);

	    tcg_gen_subi_i32(REG(1), t2, 4);
    	    tcg_gen_qemu_st32(t1, REG(1), ctx->memidx);
	    
	    /* Push the current frame pointer.  */
	    tcg_gen_subi_i32(t2, REG(1), 4);
	    tcg_gen_mov_i32(REG(1), t2);
	    tcg_gen_qemu_st32(REG(0), REG(1), ctx->memidx);
			      
	    /* Set the pc and $fp.  */
	    tcg_gen_mov_i32(REG(0), REG(1));
	    tcg_gen_mov_i32(cpu_pc, SREG(1));
	    dead_tmp(t1);
	    dead_tmp(t2);
	    dead_tmp(code);
	    tcg_gen_exit_tb(0);
	    ctx->bstate = BS_BRANCH;

	    gen_set_label(l2);

	    dead_tmp(result);
	  }
	  break;
#else
	case 0x31: /* div.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_div_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
#endif
	case 0x32: /* udiv.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_divu_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x33: /* mod.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_rem_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
	case 0x34: /* umod.l */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv result = new_tmp();
	    tcg_gen_remu_i32(result, REG(a), REG(b));
	    tcg_gen_mov_i32(REG(a), result);
	    dead_tmp(result);
	  }
	  break;
#if 0
	case 0x35: /* brk */
	  TRACE("brk");
	  cpu.asregs.exception = SIGTRAP;
	  pc -= 2; /* Adjust pc */
	  break;
#endif
	case 0x36: /* ldo.b */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(b), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_ld8u(t2, t1, ctx->memidx);
	    tcg_gen_mov_i32(REG(a), t2);

	    dead_tmp(t1);
	    dead_tmp(t2);
	    
	    length = 6;
	  }
	  break;
	case 0x37: /* sto.b */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(a), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_st8(REG(b), t1, ctx->memidx);

	    dead_tmp(t1);
	    dead_tmp(t2);

	    length = 6;
	  }
	  break;
	case 0x38: /* ldo.s */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(b), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_ld16u(t2, t1, ctx->memidx);
	    tcg_gen_mov_i32(REG(a), t2);

	    dead_tmp(t1);
	    dead_tmp(t2);
	    
	    length = 6;
	  }
	  break;
	case 0x39: /* sto.s */
	  {
	    int a = (opcode >> 4) & 0xf;
	    int b = opcode & 0xf;

	    TCGv t1 = new_tmp();
	    TCGv t2 = new_tmp();
	    tcg_gen_addi_i32(t1, REG(a), ldl_code(ctx->pc+2));
	    tcg_gen_qemu_st16(REG(b), t1, ctx->memidx);

	    dead_tmp(t1);
	    dead_tmp(t2);

	    length = 6;
	  }
	  break;
	default:
	  printf("* 0x%x\tForm 1 ********** 0x%x\n", ctx->pc, inst); 
	  cpu_dump_state(env, logfile, fprintf, 0);
	  abort();
	  break;
	}
    }

  return length;
}

/* generate intermediate code for basic block 'tb'.  */
struct DisasContext ctx;
static void
gen_intermediate_code_internal(CPUState *env, TranslationBlock *tb,
                               int search_pc)
{
  DisasContext ctx;
  target_ulong pc_start;
  uint16_t *gen_opc_end;
  CPUBreakpoint *bp;
  int j, lj = -1;

  /* generate intermediate code */
  num_temps = 0;
  memset(temps, 0, sizeof(temps));

  if (search_pc && loglevel)
    fprintf (logfile, "search pc %d\n", search_pc);

  pc_start = tb->pc;
  gen_opc_ptr = gen_opc_buf;
  gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;
  gen_opparam_ptr = gen_opparam_buf;
  ctx.pc = pc_start;
  ctx.saved_pc = -1;
  ctx.tb = tb;
  ctx.memidx = 0;
  ctx.singlestep_enabled = 0;
  ctx.bstate = BS_NONE;
  /* Restore delay slot state from the tb context.  */
  ctx.hflags = (uint32_t)tb->flags; /* FIXME: maybe use 64 bits here? */
  restore_cpu_state(env, &ctx);
#ifdef DEBUG_DISAS
  if (loglevel & CPU_LOG_TB_CPU) {
    fprintf(logfile, "------------------------------------------------\n");
    /* FIXME: This may print out stale hflags from env... */
    cpu_dump_state(env, logfile, fprintf, 0);
  }
#endif

  do {
    if (unlikely(!TAILQ_EMPTY(&env->breakpoints))) {
      TAILQ_FOREACH(bp, &env->breakpoints, entry) {
	if (ctx.pc == bp->pc) {
	  save_cpu_state(&ctx, 1);
	  tcg_gen_movi_i32(cpu_pc, ctx.pc);
	  gen_helper_debug();
	  ctx.bstate = BS_EXCP;
	  goto done_generating;
	}
      }
    }
    
    if (search_pc) {
      j = gen_opc_ptr - gen_opc_buf;
      if (lj < j) {
	lj++;
	while (lj < j)
	  gen_opc_instr_start[lj++] = 0;
      }
      gen_opc_pc[lj] = ctx.pc;
      gen_opc_instr_start[lj] = 1;
    }
    ctx.opcode = ldsw_code(ctx.pc);
    ctx.pc += decode_opc(env, &ctx);
      
    if (env->singlestep_enabled)
      break;
      
    if ((ctx.pc & (TARGET_PAGE_SIZE - 1)) == 0)
      break;
  } while (ctx.bstate == BS_NONE && gen_opc_ptr < gen_opc_end);
    
  if (env->singlestep_enabled) {
    save_cpu_state(&ctx, ctx.bstate == BS_NONE);
    tcg_gen_movi_tl(cpu_pc, ctx.pc);
    gen_helper_debug();
  } else {
    switch (ctx.bstate) {
    case BS_STOP:
    case BS_NONE:
      save_cpu_state(&ctx, 0);
      gen_goto_tb(&ctx, 0, ctx.pc);
      break;
    case BS_EXCP:
      /* AG FIXME gen_op_interrupt_restart(); 
	 gen_op_reset_T0();
	 gen_op_exit_tb();*/
      break;
    case BS_BRANCH:
    default:
      break;
    }
  }
 done_generating:
  ctx.last_T0_store = NULL;
  *gen_opc_ptr = INDEX_op_end;
  if (search_pc) {
    j = gen_opc_ptr - gen_opc_buf;
    lj++;
    while (lj <= j)
      gen_opc_instr_start[lj++] = 0;
  } else {
    tb->size = ctx.pc - pc_start;
  }
#ifdef DEBUG_DISAS
  if (loglevel & CPU_LOG_TB_IN_ASM) {
    fprintf(logfile, "IN: %s\n", lookup_symbol(pc_start));
    target_disas(logfile, pc_start, ctx.pc - pc_start, 0);
    fprintf(logfile, "\n");
  }
  if (loglevel & CPU_LOG_TB_CPU) {
    fprintf(logfile, "---------------- %d %08x\n", ctx.bstate, ctx.hflags);
  }
#endif
}

void gen_intermediate_code(CPUState * env, struct TranslationBlock *tb)
{
  gen_intermediate_code_internal(env, tb, 0);
}

void gen_intermediate_code_pc(CPUState * env, struct TranslationBlock *tb)
{
  gen_intermediate_code_internal(env, tb, 1);
}

void gen_pc_load(CPUState *env, struct TranslationBlock *tb,
                 unsigned long searched_pc, int pc_pos, void *puc)
{
  env->pc = gen_opc_pc[pc_pos];
}
