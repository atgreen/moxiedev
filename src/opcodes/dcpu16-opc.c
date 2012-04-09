/* dcpu16-opc.c -- Definitions for dcpu16 opcodes.
   Copyright 2009, 2012 Free Software Foundation, Inc.
   Contributed by Anthony Green (green@moxielogic.com).

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
   along with this file; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "opcode/dcpu16.h"

const dcpu16_opc_info_t dcpu16_basic_opc_info[16] =
  {
    { 0x0, DCPU16_NONBASIC, "BAD" },
    { 0x1, DCPU16_BASIC, "set" },
    { 0x2, DCPU16_BASIC, "add" },
    { 0x3, DCPU16_BASIC, "sub" },
    { 0x4, DCPU16_BASIC, "mul" },
    { 0x5, DCPU16_BASIC, "div" },
    { 0x6, DCPU16_BASIC, "mod" },
    { 0x7, DCPU16_BASIC, "shl" },
    { 0x8, DCPU16_BASIC, "shr" },
    { 0x9, DCPU16_BASIC, "and" },
    { 0xa, DCPU16_BASIC, "bor" },
    { 0xb, DCPU16_BASIC, "xor" },
    { 0xc, DCPU16_BASIC, "ife" },
    { 0xd, DCPU16_BASIC, "ifn" },
    { 0xe, DCPU16_BASIC, "ifg" },
    { 0xf, DCPU16_BASIC, "ifb" }
  };

const dcpu16_opc_info_t dcpu16_nonbasic_opc_info[2] =
  {
    { 0x00, DCPU16_NONBASIC,  "BAD" },
    { 0x01, DCPU16_NONBASIC,  "jsr" } 
  };


