/* check_cmp for The Incredible Moxielyzer (C) 2009 Anthony Green

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
#include <bfd.h>

#include "moxielyzer.h"

static void
do_check_cmp (moxie_analyzer *analyzer, unsigned long index, enum moxie_insn insn)
{
  /* The moxie GCC port sometimes places a jump right between a cmp
     and branch.  This is very wrong!  Make sure we don't have any of
     these.  */
  if (analyzer->code[analyzer->iflags[index+1]] & IS_JUMP)
    printf ("Instruction %lu is a cmp followed by a jump!\n", index);
  if (! (analyzer->code[analyzer->iflags[index+1]] & IS_BRANCH))
    printf ("Instruction %lu is a cmp not followed by a branch!\n", index);
}

static void
init_check_cmp (moxie_analyzer *analyzer)
{
  analyzer->callback[moxie_i_cmp] = do_check_cmp;
}

static const struct plugin check_cmp_plugin = {
  .name  = "check_cmp",
  .desc  = "Check that no cmp instruction is immediately followed by a jump of any kind.",
  .init  = init_check_cmp
};

REGISTER(check_cmp_plugin);
