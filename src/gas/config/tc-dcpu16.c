/* tc-dcpu16.c -- Assemble code for dcpu16
   Copyright 2009, 2012
   Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* Contributed by Anthony Green <green@moxielogic.com>.  */

#include "as.h"
#include "safe-ctype.h"
#include "opcode/dcpu16.h"
#include "elf/dcpu16.h"

extern const dcpu16_opc_info_t dcpu16_opc_info[128];

const char comment_chars[]        = "#";
const char line_separator_chars[] = ";";
const char line_comment_chars[]   = "#";

static int pending_reloc;
static struct hash_control *opcode_hash_control;

const pseudo_typeS md_pseudo_table[] =
{
  {0, 0, 0}
};

static char *regarray = "ABCXYZIJabcxyzij";

const char FLT_CHARS[] = "rRsSfFdDxXpP";
const char EXP_CHARS[] = "eE";

void
md_operand (expressionS *op __attribute__((unused)))
{
  /* Empty for now. */
}

/* This function is called once, at assembler startup time.  It sets
   up the hash table with all the opcodes in it, and also initializes
   some aliases for compatibility with other assemblers.  */

void
md_begin (void)
{
  int count;
  const dcpu16_opc_info_t *opcode;
  opcode_hash_control = hash_new ();

  /* Insert names into hash table.  */
  for (count = 0, opcode = dcpu16_basic_opc_info; count++ < 16; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  for (count = 0, opcode = dcpu16_nonbasic_opc_info; count++ < 2; opcode++)
    hash_insert (opcode_hash_control, opcode->name, (char *) opcode);

  bfd_set_arch_mach (stdoutput, TARGET_ARCH, 0);
}

/* Parse an expression and then restore the input line pointer.  */

static char *
parse_exp_save_ilp (char *s, expressionS *op)
{
  char *save = input_line_pointer;

  input_line_pointer = s;
  expression (op);
  s = input_line_pointer;
  input_line_pointer = save;
  return s;
}

static int
parse_operand (char **ptr)
{
  char *s = *ptr;

  switch (*s)
    {
    case 'A':
    case 'a':
      *ptr += 1;
      return 0;
    case 'B':
    case 'b':
      *ptr += 1;
      return 1;
    case 'C':
    case 'c':
      *ptr += 1;
      return 2;
    case 'X':
    case 'x':
      *ptr += 1;
      return 3;
    case 'Y':
    case 'y':
      *ptr += 1;
      return 4;
    case 'Z':
    case 'z':
      *ptr += 1;
      return 5;
    case 'I':
    case 'i':
      *ptr += 1;
      return 6;
    case 'J':
    case 'j':
      *ptr += 1;
      return 7;
    case 'P':
      {
	if (!strncmp ("POP", s, 3))
	  {
	    *ptr += 3;
	    return 0x18;
	  }
	if (!strncmp ("PEEK", s, 4))
	  {
	    *ptr += 4;
	    return 0x19;
	  }
	if (!strncmp ("PUSH", s, 4))
	  {
	    *ptr += 4;
	    return 0x1a;
	  }
	if (!strncmp ("PC", s, 2))
	  {
	    *ptr += 2;
	    return 0x1c;
	  }
	as_bad (_("bad operand a"));
	ignore_rest_of_line ();
	return -1;
      }
    case 'S':
      {
	if (!strncmp ("SP", s, 2))
	  {
	    *ptr += 2;
	    return 0x1b;
	  }
      }
    case '[':
      {
	/* Scan ahead to see if we're looking at an expression that ends in "+[REG]".  */
        {
	  char *p = *ptr;
	  
	  /* Skip to the closing ']' */
	  for (p = *ptr + 1; *p && *p != ']'; p++);
	  if (! *p)
	    {
	      as_bad (_("bad operand b"));
	      return -1;
	    }
	  /* Scan backwards, looking for a '+'.  */
	  for (p = p - 1; p != *ptr && *p != '+'; p--);

	  if (p == *ptr)
	    {
	      int o;
	      *ptr += 1;
	      o = parse_operand (ptr);
	      if (o != 0x1f)
		{
		  as_bad (_("bad operand c"));
		  return -1;
		}
	      while (ISSPACE (**ptr))
		*ptr += 1;
	      if (**ptr != ']')
		{
		  as_bad (_("bad operand d"));
		  return -1;
		}
	      *ptr += 1;
	      return 0x1e;
	    }
	  else
	    {
	      char *save = p;
	      /* Scan forwards, looking for register operand.  */
	      for (p = p+1; ISSPACE(*p); p++);
	      if (strchr (regarray, *p) == NULL)
		{
		  int o;
		  *ptr += 1;
		  o = parse_operand (ptr);
		  if (o != 0x1f)
		    {
		      as_bad (_("bad operand e"));
		      return -1;
		    }
		  while (ISSPACE (**ptr))
		    *ptr += 1;
		  if (**ptr != ']')
		    {
		      as_bad (_("bad operand f"));
		      return -1;
		    }
		  *ptr += 1;
		  return 0x1e;
		}
	      else
		{
		  int reg = strchr (regarray, *p) - regarray;
		  reg -= (reg > 7 ? 7 : 0);
		  *save = 0;
		  {
		    int o;
		    *ptr += 1;
		    o = parse_operand (ptr);
		    if (o != 0x1f)
		      {
			as_bad (_("bad operand g"));
			return -1;
		      }
		    *ptr = p + 1;
		    while (ISSPACE (**ptr))
		      *ptr += 1;
		    if (**ptr != ']')
		      {
			as_bad (_("bad operand h"));
			return -1;
		      }
		    *ptr = strchr (save+1, ']') + 1;
		    return 0x10 + reg;
		  }
		}
	    }
	}
      }
    default:
      {
	expressionS arg;
	char *where;

	*ptr = parse_exp_save_ilp (*ptr, &arg);
	where = frag_more (2);
	fix_new_exp (frag_now,
		     (where - frag_now->fr_literal),
		     2,
		     &arg,
		     0,
		     BFD_RELOC_16);
	return 0x1f;
      }

    }
  as_bad (_("bad operand i"));
  ignore_rest_of_line ();
  return -1;
}

/* This is the guts of the machine-dependent assembler.  STR points to
   a machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.  */

void
md_assemble (char *str)
{
  char *op_start;
  char *op_end;

  dcpu16_opc_info_t *opcode;
  char *p;
  char pend;

  unsigned short iword = 0;

  int nlen = 0;

  /* Drop leading whitespace.  */
  while (*str == ' ')
    str++;

  /* Find the op code end.  */
  op_start = str;
  for (op_end = str;
       *op_end && !is_end_of_line[*op_end & 0xff] && *op_end != ' ';
       op_end++)
    nlen++;

  pend = *op_end;
  *op_end = 0;

  if (nlen == 0)
    as_bad (_("can't find opcode "));
  opcode = (dcpu16_opc_info_t *) hash_find (opcode_hash_control, op_start);
  *op_end = pend;

  if (opcode == NULL)
    {
      as_bad (_("unknown opcode %s"), op_start);
      return;
    }

  p = frag_more (2);

  switch (opcode->itype)
    {
    case DCPU16_BASIC:
      iword = opcode->opcode;
      while (ISSPACE (*op_end))
	op_end++;
      {
	int op1, op2;
	op1 = parse_operand (&op_end);
	if (*op_end != ',')
	  as_warn (_("expecting comma delimited register operands"));
	op_end++;
	op2 = parse_operand (&op_end);
	iword += (op1 << 4) + (op2 << 10);
	while (ISSPACE (*op_end))
	  op_end++;
	if (*op_end != 0)
	  as_warn (_("extra stuff on line ignored"));
      }
      break;
    case DCPU16_NONBASIC:
      iword = opcode->opcode << 4;
      while (ISSPACE (*op_end))
	op_end++;
      {
	int op1;
	op1 = parse_operand (&op_end);
	iword += (op1 << 10);
	while (ISSPACE (*op_end))
	  op_end++;
	if (*op_end != 0)
	  as_warn (_("extra stuff on line ignored"));
      }
      break;
    default:
      abort ();
    }

  md_number_to_chars (p, iword, 2);

  while (ISSPACE (*op_end))
    op_end++;

  if (*op_end != 0)
    as_warn (_("extra stuff on line ignored"));

  if (pending_reloc)
    as_bad (_("Something forgot to clean up\n"));
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (int type, char *litP, int *sizeP)
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return _("bad call to md_atof");
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  for (i = prec - 1; i >= 0; i--)
    {
      md_number_to_chars (litP, (valueT) words[i], 2);
      litP += 2;
    }

  return NULL;
}

const char *md_shortopts = "";

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof (md_longopts);

/* We have no target specific options yet, so these next
   two functions are empty.  */
int
md_parse_option (int c ATTRIBUTE_UNUSED, char *arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE *stream ATTRIBUTE_UNUSED)
{
}

/* Apply a fixup to the object file.  */

void
md_apply_fix (fixS *fixP ATTRIBUTE_UNUSED, 
	      valueT * valP ATTRIBUTE_UNUSED, segT seg ATTRIBUTE_UNUSED)
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  long val = *valP;
  long max, min;

  max = min = 0;
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_16:
      *buf++ = val >> 8;
      *buf++ = val >> 0;
      break;

    default:
      abort ();
    }

  if (max != 0 && (val < min || val > max))
    as_bad_where (fixP->fx_file, fixP->fx_line, _("offset out of range"));

  if (fixP->fx_addsy == NULL && fixP->fx_pcrel == 0)
    fixP->fx_done = 1;
}

/* Put number into target byte order (big endian).  */

void
md_number_to_chars (char *ptr, valueT use, int nbytes)
{
  number_to_chars_bigendian (ptr, use, nbytes);
}

/* Generate a machine-dependent relocation.  */
arelent *
tc_gen_reloc (asection *section ATTRIBUTE_UNUSED, fixS *fixP)
{
  arelent *relP;
  bfd_reloc_code_real_type code;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_16:
      code = fixP->fx_r_type;
      break;
    default:
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("Semantics error.  This type of operand can not be relocated, it must be an assembly-time constant"));
      return 0;
    }

  relP = xmalloc (sizeof (arelent));
  gas_assert (relP != 0);
  relP->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
  *relP->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);
  relP->address = fixP->fx_frag->fr_address + fixP->fx_where;
  fprintf (stderr, "reloc 0x%x 0x%x\n", (unsigned)relP->address, (unsigned)relP->address/OCTETS_PER_BYTE);
  relP->address /= OCTETS_PER_BYTE; 
  relP->addend = fixP->fx_offset;

  /* This is the standard place for KLUDGEs to work around bugs in
     bfd_install_relocation (first such note in the documentation
     appears with binutils-2.8).

     That function bfd_install_relocation does the wrong thing with
     putting stuff into the addend of a reloc (it should stay out) for a
     weak symbol.  The really bad thing is that it adds the
     "segment-relative offset" of the symbol into the reloc.  In this
     case, the reloc should instead be relative to the symbol with no
     other offset than the assembly code shows; and since the symbol is
     weak, any local definition should be ignored until link time (or
     thereafter).
     To wit:  weaksym+42  should be weaksym+42 in the reloc,
     not weaksym+(offset_from_segment_of_local_weaksym_definition)

     To "work around" this, we subtract the segment-relative offset of
     "known" weak symbols.  This evens out the extra offset.

     That happens for a.out but not for ELF, since for ELF,
     bfd_install_relocation uses the "special function" field of the
     howto, and does not execute the code that needs to be undone.  */

  if (OUTPUT_FLAVOR == bfd_target_aout_flavour
      && fixP->fx_addsy && S_IS_WEAK (fixP->fx_addsy)
      && ! bfd_is_und_section (S_GET_SEGMENT (fixP->fx_addsy)))
    {
      relP->addend -= S_GET_VALUE (fixP->fx_addsy);
    }

  relP->howto = bfd_reloc_type_lookup (stdoutput, code);
  if (! relP->howto)
    {
      const char *name;

      name = S_GET_NAME (fixP->fx_addsy);
      if (name == NULL)
	name = _("<unknown>");
      as_fatal (_("Cannot generate relocation type for symbol %s, code %s"),
		name, bfd_get_reloc_code_name (code));
    }

  return relP;
}

/* Decide from what point a pc-relative relocation is relative to,
   relative to the pc-relative fixup.  Er, relatively speaking.  */
long
md_pcrel_from (fixS *fixP)
{
  valueT addr = fixP->fx_where + fixP->fx_frag->fr_address;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_16:
      return addr + 2;
    default:
      abort ();
      return addr;
    }
}
