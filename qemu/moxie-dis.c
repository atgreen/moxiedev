/* Disassembler code for MOXIE.
   Copyright 2000, 2001, 2002, 2004, 2005, 2006 Free Software Foundation, Inc.
   Contributed by Axis Communications AB, Lund, Sweden.
   Written by Hans-Peter Nilsson.

   This file is part of the GNU binutils and GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "dis-asm.h"


static fprintf_ftype fpr;
static void *stream;

/*
  Form 1 instructions come in different flavors:

    Some have no arguments                          (MOXIE_F1_NARG)
    Some only use the A operand                     (MOXIE_F1_A)
    Some use A and B registers                      (MOXIE_F1_AB)
    Some use A and consume a 4 byte immediate value (MOXIE_F1_A4)
    Some use just a 4 byte immediate value          (MOXIE_F1_4)
    Some use B and an indirect A                    (MOXIE_F1_AiB)
    Some use A and an indirect B                    (MOXIE_F1_ABi)
    Some consume a 4 byte immediate value and use X (MOXIE_F1_4A)
    Some use B and an indirect A plus 4 bytes       (MOXIE_F1_AiB4)
    Some use A and an indirect B plus 4 bytes       (MOXIE_F1_ABi4)

  Form 2 instructions also come in different flavors:

    Some have no arguments                          (MOXIE_F2_NARG)
    Some use the A register and an 8-bit value      (MOXIE_F2_A8V)

  Form 3 instructions also come in different flavors:

    Some have no arguments                          (MOXIE_F3_NARG)
*/

#define MOXIE_F1_NARG 0x100
#define MOXIE_F1_A    0x101
#define MOXIE_F1_AB   0x102
// #define MOXIE_F1_ABC  0x103
#define MOXIE_F1_A4   0x104
#define MOXIE_F1_4    0x105
#define MOXIE_F1_AiB  0x106
#define MOXIE_F1_ABi  0x107
#define MOXIE_F1_4A   0x108
#define MOXIE_F1_AiB4 0x109
#define MOXIE_F1_ABi4 0x10a

#define MOXIE_F2_NARG 0x200
#define MOXIE_F2_A8V  0x201

#define MOXIE_F3_NARG 0x300

typedef struct moxie_opc_info_t
{
  short opcode;
  unsigned itype;
  const char *name;
} moxie_opc_info_t;

extern const moxie_opc_info_t moxie_form1_opc_info[64];
extern const moxie_opc_info_t moxie_form2_opc_info[4];
extern const moxie_opc_info_t moxie_form3_opc_info[4];

/*
  The moxie processor's 16-bit instructions come in two forms:

  FORM 1 instructions start with a 0 bit...

    0oooooooaaaabbbb
    0              F
 
   ooooooo - form 1 opcode number
   aaaa    - operand A
   bbbb    - operand B

  FORM 2 instructions start with bits "10"...

    10ooaaaavvvvvvvv
    0              F
  
   oo       - form 2 opcode number
   aaaa     - operand A
   vvvvvvvv - 8-bit immediate value

  FORM 3 instructions start with a bits "11"...

    11oovvvvvvvvvvvv
    0              F
  
   oo           - form 3 opcode number
   vvvvvvvvvvvv - 12-bit immediate value

*/

const moxie_opc_info_t moxie_form1_opc_info[64] =
  {
    { 0x00, MOXIE_F1_NARG, "nop" },
    { 0x01, MOXIE_F1_A4,   "ldi.l" },
    { 0x02, MOXIE_F1_AB,   "mov" },
    { 0x03, MOXIE_F1_4,    "jsra" },
    { 0x04, MOXIE_F1_NARG, "ret" },
    { 0x05, MOXIE_F1_AB,   "add.l" },
    { 0x06, MOXIE_F1_AB,   "push" },
    { 0x07, MOXIE_F1_AB,   "pop" },
    { 0x08, MOXIE_F1_A4,   "lda.l" },
    { 0x09, MOXIE_F1_4A,   "sta.l" },
    { 0x0a, MOXIE_F1_ABi,  "ld.l" },
    { 0x0b, MOXIE_F1_AiB,  "st.l" },
    { 0x0c, MOXIE_F1_ABi4, "ldo.l" },
    { 0x0d, MOXIE_F1_AiB4, "sto.l" },
    { 0x0e, MOXIE_F1_AB,   "cmp" },
    { 0x0f, MOXIE_F1_4,    "beq" },
    { 0x10, MOXIE_F1_4,    "bne" },
    { 0x11, MOXIE_F1_4,    "blt" },
    { 0x12, MOXIE_F1_4,    "bgt" },
    { 0x13, MOXIE_F1_4,    "bltu" },
    { 0x14, MOXIE_F1_4,    "bgtu" },
    { 0x15, MOXIE_F1_4,    "bge" },
    { 0x16, MOXIE_F1_4,    "ble" },
    { 0x17, MOXIE_F1_4,    "bgeu" },
    { 0x18, MOXIE_F1_4,    "bleu" },
    { 0x19, MOXIE_F1_A,    "jsr" },
    { 0x1a, MOXIE_F1_4,    "jmpa" },
    { 0x1b, MOXIE_F1_A4,   "ldi.b" },
    { 0x1c, MOXIE_F1_ABi,  "ld.b" },
    { 0x1d, MOXIE_F1_A4,   "lda.b" },
    { 0x1e, MOXIE_F1_AiB,  "st.b" },
    { 0x1f, MOXIE_F1_4A,   "sta.b" },
    { 0x20, MOXIE_F1_A4,   "ldi.s" },
    { 0x21, MOXIE_F1_ABi,  "ld.s" },
    { 0x22, MOXIE_F1_A4,   "lda.s" },
    { 0x23, MOXIE_F1_AiB,  "st.s" },
    { 0x24, MOXIE_F1_4A,   "sta.s" },
    { 0x25, MOXIE_F1_A,    "jmp" },
    { 0x26, MOXIE_F1_AB,   "and" },
    { 0x27, MOXIE_F1_AB,   "lshr" },
    { 0x28, MOXIE_F1_AB,   "ashl" },
    { 0x29, MOXIE_F1_AB,   "sub.l" },
    { 0x2a, MOXIE_F1_AB,   "neg" },
    { 0x2b, MOXIE_F1_AB,   "or" },
    { 0x2c, MOXIE_F1_AB,   "not" },
    { 0x2d, MOXIE_F1_AB,   "ashr" },
    { 0x2e, MOXIE_F1_AB,   "xor" },
    { 0x2f, MOXIE_F1_AB,   "mul.l" },
    { 0x30, MOXIE_F1_4,    "swi" },
    { 0x31, MOXIE_F1_AB,   "div.l" },
    { 0x32, MOXIE_F1_AB,   "udiv.l" },
    { 0x33, MOXIE_F1_AB,   "mod.l" },
    { 0x34, MOXIE_F1_AB,   "umod.l" },
    { 0x35, MOXIE_F1_NARG, "brk" },
    { 0x36, MOXIE_F1_ABi4, "ldo.b" },
    { 0x37, MOXIE_F1_AiB4, "sto.b" },
    { 0x38, MOXIE_F1_ABi4, "ldo.s" },
    { 0x39, MOXIE_F1_AiB4, "sto.s" },
    { 0x3a, MOXIE_F1_NARG, "bad" },
    { 0x3b, MOXIE_F1_NARG, "bad" },
    { 0x3c, MOXIE_F1_NARG, "bad" },
    { 0x3d, MOXIE_F1_NARG, "bad" },
    { 0x3e, MOXIE_F1_NARG, "bad" },
    { 0x3f, MOXIE_F1_NARG, "bad" }
  };

const moxie_opc_info_t moxie_form2_opc_info[4] =
  {
    { 0x00, MOXIE_F2_A8V,  "inc" },
    { 0x01, MOXIE_F2_A8V,  "dec" },
    { 0x02, MOXIE_F2_A8V,  "gsr" },
    { 0x03, MOXIE_F2_A8V,  "ssr" }
  };

const moxie_opc_info_t moxie_form3_opc_info[4] =
  {
    { 0x00, MOXIE_F2_NARG, "bad" },
    { 0x01, MOXIE_F2_NARG, "bad" },
    { 0x02, MOXIE_F2_NARG, "bad" },
    { 0x03, MOXIE_F2_NARG, "bad" }
  };

/* Macros to extract operands from the instruction word.  */
#define OP_A(i) ((i >> 4) & 0xf)
#define OP_B(i) (i & 0xf)

static const char *reg_names[16] = 
  { "$fp", "$sp", "$r0", "$r1", "$r2", "$r3", "$r4", "$r5", 
    "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13" };

int
print_insn_moxie (bfd_vma addr, struct disassemble_info *info)
{
  int length = 2;
  int status;
  stream = info->stream;
  const moxie_opc_info_t *opcode;
  bfd_byte buffer[4];
  unsigned short iword;
  fpr = info->fprintf_func;

  if ((status = info->read_memory_func (addr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16 (buffer);

  /* Form 1 instructions have the high bit set to 0.  */
  if ((iword & (1<<15)) == 0)
    {
      /* Extract the Form 1 opcode.  */
      opcode = &moxie_form1_opc_info[iword >> 8];
      switch (opcode->itype)
	{
	case MOXIE_F1_NARG:
	  fpr (stream, "%s", opcode->name);
	  break;
	case MOXIE_F1_A:
	  fpr (stream, "%s\t%s", opcode->name,
	       reg_names[OP_A(iword)]);
	  break;
	case MOXIE_F1_AB:
	  fpr (stream, "%s\t%s, %s", opcode->name,
	       reg_names[OP_A(iword)], 
	       reg_names[OP_B(iword)]);
	  break;
	case MOXIE_F1_A4:
	  {
	    unsigned imm;
	    if ((status = info->read_memory_func (addr+2, buffer, 4, info)))
	      goto fail;
	    imm = bfd_getb32 (buffer);
	    fpr (stream, "%s\t%s, 0x%x", opcode->name, 
		 reg_names[OP_A(iword)], imm);
	    length = 6;
	  }
	  break;
	case MOXIE_F1_4:
	  {
	    unsigned imm;
	    if ((status = info->read_memory_func (addr+2, buffer, 4, info)))
	      goto fail;
	    imm = bfd_getb32 (buffer);
	    fpr (stream, "%s\t0x%x", opcode->name, imm);
	    length = 6;
	  }
	  break;
	case MOXIE_F1_AiB:
	  fpr (stream, "%s\t(%s), %s", opcode->name,
	       reg_names[OP_A(iword)], reg_names[OP_B(iword)]);
	  break;
	case MOXIE_F1_ABi:
	  fpr (stream, "%s\t%s, (%s)", opcode->name,
	       reg_names[OP_A(iword)], reg_names[OP_B(iword)]);
	  break;
	case MOXIE_F1_4A:
	  {
	    unsigned imm;
	    if ((status = info->read_memory_func (addr+2, buffer, 4, info)))
	      goto fail;
	    imm = bfd_getb32 (buffer);
	    fpr (stream, "%s\t0x%x, %s", 
		 opcode->name, imm, reg_names[OP_A(iword)]);
	    length = 6;
	  }
	  break;
	case MOXIE_F1_AiB4:
	  {
	    unsigned imm;
	    if ((status = info->read_memory_func (addr+2, buffer, 4, info)))
	      goto fail;
	    imm = bfd_getb32 (buffer);
	    fpr (stream, "%s\t0x%x(%s), %s", opcode->name, 
		 imm,
		 reg_names[OP_A(iword)], 
		 reg_names[OP_B(iword)]);
	    length = 6;
	  }
	  break;
	case MOXIE_F1_ABi4:
	  {
	    unsigned imm;
	    if ((status = info->read_memory_func (addr+2, buffer, 4, info)))
	      goto fail;
	    imm = bfd_getb32 (buffer);
	    fpr (stream, "%s\t%s, 0x%x(%s)", 
		 opcode->name, 
		 reg_names[OP_A(iword)],
		 imm,
		 reg_names[OP_B(iword)]);
	    length = 6;
	  }
	  break;
	default:
	  abort();
	}
    }
  else if ((iword & (1<<14)) == 0)
    {
      /* Extract the Form 2 opcode.  */
      opcode = &moxie_form2_opc_info[(iword >> 12) & 3];
      switch (opcode->itype)
	{
	case MOXIE_F2_A8V:
	  fpr (stream, "%s\t%s, 0x%x", 
	       opcode->name,
	       reg_names[(iword >> 8) & 0xf],
	       iword & ((1 << 8) - 1));
	  break;
	case MOXIE_F2_NARG:
	  fpr (stream, "%s", opcode->name);
	  break;
	default:
	  abort();
	}
    }
  else 
    {
      /* Extract the Form 3 opcode.  */
      opcode = &moxie_form2_opc_info[(iword >> 12) & 3];
      switch (opcode->itype)
	{
	case MOXIE_F3_NARG:
	  fpr (stream, "%s", opcode->name);
	  break;
	default:
	  abort();
	}
    }

  return length;

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}


