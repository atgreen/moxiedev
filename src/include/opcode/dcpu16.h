/* Definitions for decoding the dcpu16 opcode table.
   Copyright 2009, 2012 Free Software Foundation, Inc.
   Contributed by Anthony Green (green@moxielogic.com).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Instructions are either BASIC or NONBASIC. */

#define DCPU16_BASIC    0x0
#define DCPU16_NONBASIC 0x1

typedef struct dcpu16_opc_info_t
{
  short         opcode;
  unsigned      itype;
  const char *  name;
} dcpu16_opc_info_t;

extern const dcpu16_opc_info_t dcpu16_basic_opc_info[16];
extern const dcpu16_opc_info_t dcpu16_nonbasic_opc_info[2];
