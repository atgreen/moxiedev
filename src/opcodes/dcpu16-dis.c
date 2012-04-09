/* Disassemble dcpu16 instructions.
   Copyright 2009, 2012
   Free Software Foundation, Inc.

   This file is part of the GNU opcodes library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdio.h>
#include "sysdep.h"
#define STATIC_TABLE
#define DEFINE_TABLE

#include "opcode/dcpu16.h"
#include "dis-asm.h"

static fprintf_ftype fpr;
static void *stream;

static const char * reg_names[8] =
  { "A", "B", "C", "X", "Y", "Z", "I", "J" };

static char obuf1[128];
static char obuf2[128];

static const char *operand (bfd_vma addr, 
			    struct disassemble_info * info,
			    int op,
			    int *length)
{
  int status, imm;
  bfd_byte buffer[2];
	  
  if (op < 8)
    return reg_names[op];
  else if (op < 16)
    {
      sprintf (obuf2, "[%s]", reg_names[op-8]);
      return obuf2;
    }
  else if (op < 0x18)
    {
      if ((status = 
	   info->read_memory_func (addr, buffer, 2, info)))
	goto fail;
      imm = bfd_getb16 (buffer);
      sprintf (obuf2, "[0x%x+%s]", imm, reg_names[op-16]);
      *length += 1;
      return obuf2;
    }
  else if (op > 0x1f)
    {
      sprintf (obuf1, "0x%x", op-0x20);
      return obuf1;
    }
  else
    switch (op)
      {
      case 0x18:
	return "POP";
      case 0x19:
	return "PEEK";
      case 0x1a:
	return "PUSH";
      case 0x1b:
	return "SP";
      case 0x1c:
	return "PC";
      case 0x1d:
	return "0x0";
      case 0x1e:
	{
	  if ((status = 
	       info->read_memory_func (addr, buffer, 2, info)))
	    goto fail;
	  imm = bfd_getb16 (buffer);
	  sprintf (obuf1, "[0x%x]", imm);
	  *length += 1;
	  return obuf1;
	}
      case 0x1f:
	{
	  if ((status = 
	       info->read_memory_func (addr, buffer, 2, info)))
	    goto fail;
	  imm = bfd_getb16 (buffer);
	  sprintf (obuf1, "0x%x", imm);
	  *length += 1;
	  return obuf1;
	}
      default:
	abort ();
      }

 fail:
  info->memory_error_func (status, addr, info);
  return "BAD";
}

int
print_insn_dcpu16 (bfd_vma addr, struct disassemble_info * info)
{
  int length = 1;
  int status;
  stream = info->stream;
  const dcpu16_opc_info_t * opcode;
  bfd_byte buffer[4];
  unsigned short iword;
  fpr = info->fprintf_func;

  if ((status = info->read_memory_func (addr, buffer, 2, info)))
    goto fail;
  iword = bfd_getb16 (buffer);

  /* Basic instructions have non-zero 4-bit opcodes.  */
  if ((iword & 0xf) != 0)
    {
      /* Print the opcode.  */
      opcode = &dcpu16_basic_opc_info[iword & 0xf];
      fpr (stream, "%s\t", opcode->name);
      fpr (stream, "%s,\t", operand (addr+length, info, 
				     (iword >> 4) & ((1<<6)-1),
				     &length));
      fpr (stream, "%s", operand (addr+length, info,
				  (iword >> 10) & ((1<<6)-1),
				  &length));
    }
  else
    {
      if (((iword >> 4) & ((1<<6)-1)) == 0x1)
	{
	  fpr (stream, "jsr\t%s", operand (addr+length, info, iword>>10, &length));
	}
      else
	abort();
    }

  return length * 2; /* length is in words, but we need to return octet count. */

 fail:
  info->memory_error_func (status, addr, info);
  return -1;
}
