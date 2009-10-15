/* insn_hist for The Incredible Moxielyzer (C) 2009 Anthony Green

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

static unsigned long insn_count[MOXIE_LAST_INSN];

static void
track_insn_usage (moxie_analyzer *analyzer, 
		  unsigned long index, enum moxie_insn insn)
{
  insn_count[insn]++;
}

static void
init_insn_hist (moxie_analyzer *analyzer)
{
  memset (insn_count, 0, sizeof (unsigned long) * MOXIE_LAST_INSN);
  analyzer->callback[moxie_i] = track_insn_usage;
}

static void
report_insn_hist ()
{
  int r;
  printf ("Instruction Usage: \n");
  for (r = 1; r < MOXIE_LAST_INSN; r++)
    {
      printf ("\t%s:\t%u\n", moxie_insn_names[r], insn_count[r]);
    }
}

static const struct plugin offset_regs = {
  .name  = "insn_hist",
  .desc  = "Dump the static frequency of use for each instruction.",
  .init  = init_insn_hist,
  .fini  = report_insn_hist
};

REGISTER(offset_regs);

