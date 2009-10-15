/* The Incredible Moxielyzer (C) 2009 Anthony Green

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

#ifndef MOXIELYZER_H
#define MOXIELYZER_H

#include "insns.h"

struct moxie_analyzer_type;

/* Each instruction gets a 32-bitmap of flags for tracking things we
   care about.  See iflags below. These are the flags.  */
#define IS_BRANCH            0x1
#define IS_JUMP              0x2
#define IS_ADDRESSING_OFFSET 0x4
#define IS_INDIRECT_A        0x8
#define IS_INDIRECT_B        0x10

struct plugin {
  const char *name;
  const char *desc;
  void (*init)();
  void (*fini)();
};

typedef struct moxie_analyzer_type {
  void (*callback[MOXIE_LAST_INSN])(struct moxie_analyzer_type *analyzer, unsigned long index, enum moxie_insn);
  size_t *ioffsets;
  unsigned long *iflags;
  bfd_byte *code;
  struct plugin *plugin;
} moxie_analyzer;

static void 
register_this_plugin (void) __attribute__((constructor));

#define REGISTER(PLUGIN)					\
  static void							\
  register_this_plugin ()					\
  {								\
    register_plugin (&PLUGIN);					\
  }

void register_plugin (struct plugin *p);

#endif

