/* offset_regs for The Incredible Moxielyzer (C) 2009 Anthony Green

This file is part of The Incredible Moxielyzer

The Incredible Moxielyzer is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3, or (at
your option) any later version.

The Incredible Moxielyzer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Incredible Moxielyzer; see the file COPYING3.  If not
see <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bfd.h>

#include "moxielyzer.h"

static unsigned long offset_count[16];

static int
get_reg_a (bfd_byte *code)
{
  return (code[3] >> 4);
}

static int
get_reg_b (bfd_byte *code)
{
  return (code[3] & 0xf);
}

static void
track_offset_regs (moxie_analyzer *analyzer, 
		   unsigned long index, enum moxie_insn insn)
{
  unsigned long flags = analyzer->iflags[index];

  /* Keep track of the registers used for offset indexing.  */
  if (flags & IS_ADDRESSING_OFFSET)
    {
      if (flags & IS_INDIRECT_A) 
	{
	  offset_count[get_reg_a(&analyzer->code[analyzer->ioffsets[index]])]++;
	}
      else if (flags & IS_INDIRECT_B)
	{
	  offset_count[get_reg_b(&analyzer->code[analyzer->ioffsets[index]])]++;
	}
      else
	abort ();
    }
}

static void
init_offset_regs (moxie_analyzer *analyzer)
{
  memset (offset_count, 0, sizeof (unsigned long) * 16);
  analyzer->callback[moxie_i] = track_offset_regs;
}

static void
report_offset_regs ()
{
  int r;
  printf ("Offset Register Usage: \n");
  for (r = 0; r < 16; r++)
    {
      printf ("\t%d:\t%u\n", r, offset_count[r]);
    }
}

static const struct plugin offset_regs = {
  .name  = "offset_regs",
  .desc  = "For each register, print the frequency of use as an offset value.",
  .init  = init_offset_regs,
  .fini  = report_offset_regs
};

REGISTER(offset_regs);

