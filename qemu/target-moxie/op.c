#include "exec.h"
#include "host-utils.h"

#define REGNAME r0
#define REG (env->gregs[0])
#include "op_template.h"

#define REGNAME r1
#define REG (env->gregs[1])
#include "op_template.h"

#define REGNAME r2
#define REG (env->gregs[2])
#include "op_template.h"

#define REGNAME r3
#define REG (env->gregs[3])
#include "op_template.h"

#define REGNAME r4
#define REG (env->gregs[4])
#include "op_template.h"

#define REGNAME r5
#define REG (env->gregs[5])
#include "op_template.h"

#define REGNAME r6
#define REG (env->gregs[6])
#include "op_template.h"

#define REGNAME r7
#define REG (env->gregs[7])
#include "op_template.h"

#define REGNAME r8
#define REG (env->gregs[8])
#include "op_template.h"

#define REGNAME r9
#define REG (env->gregs[9])
#include "op_template.h"

#define REGNAME r10
#define REG (env->gregs[10])
#include "op_template.h"

#define REGNAME r11
#define REG (env->gregs[11])
#include "op_template.h"

#define REGNAME r12
#define REG (env->gregs[12])
#include "op_template.h"

#define REGNAME r13
#define REG (env->gregs[13])
#include "op_template.h"

#define REGNAME r14
#define REG (env->gregs[14])
#include "op_template.h"

#define REGNAME r15
#define REG (env->gregs[15])
#include "op_template.h"

void op_and (void)
{
  asm("nop");
    T0 &= T1;
    FORCE_RET();
}

void op_or (void)
{
  asm("nop");
    T0 |= T1;
    FORCE_RET();
}

void op_xor (void)
{
  asm("nop");
    T0 ^= T1;
    FORCE_RET();
}

void op_ashl (void)
{
  asm("nop");
    T0 <<= T1;
    FORCE_RET();
}

void op_ashr (void)
{
  asm("nop");
    T0 >>= T1;
    FORCE_RET();
}

void op_lshr (void)
{
  asm("nop");
  T0 = (unsigned) (((unsigned) T0) >> T1);
    FORCE_RET();
}

void op_not (void)
{
  asm("nop");
    T0 ^= 0xffffffff;
    FORCE_RET();
}

void OPPROTO op_debug(void)
{
    env->exception_index = EXCP_DEBUG;
    cpu_loop_exit();
}

/* Branches */
void OPPROTO op_goto_tb0(void)
{
  asm("nop");
    GOTO_TB(op_goto_tb0, PARAM1, 0);
    FORCE_RET();
}

void OPPROTO op_goto_tb1(void)
{
  asm("nop");
    GOTO_TB(op_goto_tb1, PARAM1, 1);
    FORCE_RET();
}

void op_save_pc (void)
{
  asm("nop");
    env->pc = PARAM1;
    FORCE_RET();
}

void op_beq (void)
{
  asm("nop");
  if (env->cc & CC_EQ)
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bgtu (void)
{
  asm("nop");
  if (env->cc & CC_GTU)
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bge (void)
{
  asm("nop");
  if ((env->cc & CC_GT) || (env->cc & CC_EQ))
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_ble (void)
{
  asm("nop");
  if ((env->cc & CC_LT) || (env->cc & CC_EQ))
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_blt (void)
{
  asm("nop");
  if (env->cc & CC_LT)
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bgt (void)
{
  asm("nop");
  if (env->cc & CC_GT)
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bleu (void)
{
  asm("nop");
  if ((env->cc & CC_LTU) || (env->cc & CC_EQ))
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bgeu (void)
{
  asm("nop");
  if ((env->cc & CC_GTU) || (env->cc & CC_EQ))
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_bne (void)
{
  asm("nop");
  if (! (env->cc & CC_EQ))
    GOTO_LABEL_PARAM(1);
  FORCE_RET();
}

void op_exit_tb (void)
{
  asm("nop");
    EXIT_TB();
    FORCE_RET();
}

#define SET_RESET(treg, tregname)        \
    void glue(op_set, tregname)(void)    \
    {                                    \
  asm("nop"); \
        treg = PARAM1;                   \
        FORCE_RET();                     \
    }                                    \
    void glue(op_reset, tregname)(void)  \
    {                                    \
  asm("nop"); \
        treg = 0;                        \
        FORCE_RET();                     \
    }

SET_RESET(T0, _T0)

void OPPROTO op_movl_T0_im (void)
{
  asm("nop");
	T0 = PARAM1;
	FORCE_RET();
}

void OPPROTO op_dec (void)
{
  asm("nop");
	T0 -= PARAM1;
	FORCE_RET();
}

void OPPROTO op_inc (void)
{
  asm("nop");
	T0 += PARAM1;
	FORCE_RET();
}

void OPPROTO op_movl_T1_im (void)
{
  asm("nop");
	T1 = PARAM1;
	FORCE_RET();
}

void OPPROTO op_subl_T0_im (void)
{
  asm("nop");
	T0 -= PARAM1;
	FORCE_RET();
}

void OPPROTO op_addl_T0_im (void)
{
	T0 += PARAM1;
	FORCE_RET();
}

void OPPROTO op_addl_T0_T1 (void)
{
	T0 += T1;
	FORCE_RET();
}

void OPPROTO op_subl_T0_T1 (void)
{
	T0 -= T1;
	FORCE_RET();
}

void OPPROTO op_neg_T0_T1 (void)
{
	T0 = -T1;
	FORCE_RET();
}

/* compare */
void OPPROTO op_cmp (void)
{
  int32_t cc;

  asm("nop");
  if ((int32_t) T0 == (int32_t) T1)
    cc = CC_EQ;
  else
    {
      cc |= ((int32_t) T0 < (int32_t) T1 ? CC_LT : 0);
      cc |= ((int32_t) T0 > (int32_t) T1 ? CC_GT : 0);
      cc |= ((uint32_t) T0 < (uint32_t) T1 ? CC_LTU : 0);
      cc |= ((uint32_t) T0 > (uint32_t) T1 ? CC_GTU : 0);
    }
  env->cc = cc;
  FORCE_RET();
}

void OPPROTO op_movl_T1_T0 (void)
{
  asm("nop");
	T1 = T0;
	FORCE_RET();
}

void OPPROTO op_movl_T0_T1 (void)
{
  asm("nop");
        T0 = T1;
	FORCE_RET();
}

void OPPROTO op_jmp_T0 (void)
{
    env->pc = T0;
    FORCE_RET();
}

/* Load and store */
#define MEMSUFFIX _raw
#include "op_mem.c"
#undef MEMSUFFIX
#if !defined(CONFIG_USER_ONLY)
#define MEMSUFFIX _user
#include "op_mem.c"
#undef MEMSUFFIX

#define MEMSUFFIX _kernel
#include "op_mem.c"
#undef MEMSUFFIX
#endif
