/* Target Definitions for dcpu16.
   Copyright (C) 2008, 2009, 2010, 2011, 2012  Free Software Foundation, Inc.
   Contributed by Anthony Green.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_DCPU16_H
#define GCC_DCPU16_H

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "crt0%O%s crti.o%s crtbegin.o%s"

/* Provide an ENDFILE_SPEC appropriate for svr4.  Here we tack on our own
   magical crtend.o file (see crtstuff.c) which provides part of the
   support for getting C++ file-scope static object constructed before
   entering `main', followed by the normal svr3/svr4 "finalizer" file,
   which is either `gcrtn.o' or `crtn.o'.  */

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* Provide a LIB_SPEC appropriate for svr4.  Here we tack on the default
   standard C library (unless we are building a shared library) and
   the simulator BSP code.  */

#undef LIB_SPEC
#define LIB_SPEC "%{!shared:%{!symbolic:-lc}}"

#undef  LINK_SPEC
#define LINK_SPEC "%{h*} %{v:-V} \
		   %{static:-Bstatic} %{shared:-shared} %{symbolic:-Bsymbolic}"

/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 16
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 1

#undef  SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef  WCHAR_TYPE
#define WCHAR_TYPE "long int"

#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

#define REGISTER_NAMES {	\
  "A", "B", "C", "X",   \
  "Y", "Z", "I", "J",   \
  "?fp", "?ap", "$pc", "?cc" }

#define DCPU16_A     0
#define DCPU16_B     1
#define DCPU16_C     2
#define DCPU16_X     3 
#define DCPU16_Y     4
#define DCPU16_Z     5
#define DCPU16_I     6
#define DCPU16_J     7
#define DCPU16_QFP    8
#define DCPU16_QAP    9
#define DCPU16_PC     10
#define DCPU16_CC     11

#define FIRST_PSEUDO_REGISTER 12

enum reg_class
{
  NO_REGS,
  GENERAL_REGS,
  SPECIAL_REGS,
  CC_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};


#define REG_CLASS_CONTENTS \
{ { 0x00000000 }, /* Empty */			   \
  { 0x000000FF }, /* $fp, $sp, $r0 to $r13, ?fp */ \
  { 0x00000F00 }, /* $pc */	                   \
  { 0x00000800 }, /* ?cc */                        \
  { 0x00000FFF }  /* All registers */              \
}

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "SPECIAL_REGS", \
    "CC_REGS", \
    "ALL_REGS" }

#define FIXED_REGISTERS     { 0, 0, 0, 0, \
			      0, 0, 0, 0, \
                              0, 0, 0, 0 }

#define CALL_USED_REGISTERS { 1, 1, 1, 0, \
			      0, 0, 0, 0, \
                              0, 0, 0, 0 }

/* We can't copy to or from our CC register. */
#define AVOID_CCMODE_COPIES 1

/* A C expression that is nonzero if it is permissible to store a
   value of mode MODE in hard register number REGNO (or in several
   registers starting with that one).  All gstore registers are 
   equivalent, so we can set this to 1.  */
#define HARD_REGNO_MODE_OK(R,M) 1

/* A C expression whose value is a register class containing hard
   register REGNO.  */
#define REGNO_REG_CLASS(R) ((R < DCPU16_PC) ? GENERAL_REGS :		\
                            (R == DCPU16_CC ? CC_REGS : SPECIAL_REGS))

/* A C expression for the number of consecutive hard registers,
   starting at register number REGNO, required to hold a value of mode
   MODE.  */
#define HARD_REGNO_NREGS(REGNO, MODE)			   \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1)		   \
   / UNITS_PER_WORD)

/* A C expression that is nonzero if a value of mode MODE1 is
   accessible in mode MODE2 without copying.  */
#define MODES_TIEABLE_P(MODE1, MODE2) 1

/* The Overall Framework of an Assembler File */

#undef  ASM_SPEC
#define ASM_COMMENT_START "#"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define FILE_ASM_OP     "\t.file\n"

/* Switch to the text or data segment.  */
#define TEXT_SECTION_ASM_OP  "\t.text"
#define DATA_SECTION_ASM_OP  "\t.data"

/* Assembler Commands for Alignment */

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, "\t.p2align\t%d\n", POWER);

/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand X.  */
#define PRINT_OPERAND(STREAM, X, CODE) dcpu16_print_operand (STREAM, X, CODE)

#define PRINT_OPERAND_ADDRESS(STREAM ,X) dcpu16_print_operand_address (STREAM, X)

/* Output and Generation of Labels */

#define GLOBAL_ASM_OP "\t.global\t"

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS unsigned int

/* If defined, the maximum amount of space required for outgoing arguments
   will be computed and placed into the variable
   `current_function_outgoing_args_size'.  No space will be pushed
   onto the stack for each call; instead, the function prologue should
   increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* A C statement (sans semicolon) for initializing the variable CUM
   for the state at the beginning of the argument list.  
   For dcpu16, the first arg is passed in register 2 (aka $r0).  */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = DCPU16_A)

/* How Scalar Function Values Are Returned */

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
   pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH) (DEPTH) = 0

/* Offset from the frame pointer to the first local variable slot to
   be allocated.  */
#define STARTING_FRAME_OFFSET 0

/* Define this if the above stack space is to be considered part of the
   space allocated by the caller.  */
#define OUTGOING_REG_PARM_STACK_SPACE(FNTYPE) 1
#define STACK_PARMS_IN_REG_PARM_AREA

/* Define this if it is the responsibility of the caller to allocate
   the area reserved for arguments passed in registers.  */
#define REG_PARM_STACK_SPACE(FNDECL) (6 * UNITS_PER_WORD)

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  */
#define FIRST_PARM_OFFSET(F) 12

/* Define this macro to nonzero value if the addresses of local variable slots
   are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 1

/* Define this macro as a C expression that is nonzero for registers that are
   used by the epilogue or the return pattern.  The stack and frame
   pointer registers are already assumed to be used as needed.  */
#define EPILOGUE_USES(R) 0

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX					\
  gen_frame_mem (Pmode,							\
		 plus_constant (stack_pointer_rtx, UNITS_PER_WORD))

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N)	((N) < 4 ? (N+2) : INVALID_REGNUM)

/* Store the return handler into the call frame.  */
#define EH_RETURN_HANDLER_RTX						\
  gen_frame_mem (Pmode,							\
		 plus_constant (frame_pointer_rtx, UNITS_PER_WORD))

/* Storage Layout */

/* 16-bits is the smallest addressable unit on this machine.  */
#define BITS_PER_UNIT    16

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 1
#define WORDS_BIG_ENDIAN 1

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 16

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a `char' or a `short') is no
   faster than accessing a word of memory.  */
#define SLOW_BYTE_ACCESS 0

/* Number of storage units in a word; normally the size of a
   general-purpose register, a power of two from 1 or 8.  */
#define UNITS_PER_WORD 1

/* Define this macro to the minimum alignment enforced by hardware
   for the stack pointer on this machine.  The definition is a C
   expression for the desired alignment (measured in bits).  */
#define STACK_BOUNDARY 16

/* Normal alignment required for function parameters on the stack, in
   bits.  All stack parameters receive at least this much alignment
   regardless of data type.  */
#define PARM_BOUNDARY 16

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY  16

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 16

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 16

/* Every structures size must be a multiple of 8 bits.  */
#define STRUCTURE_SIZE_BOUNDARY 16

/* Look at the fundamental type that is used for a bit-field and use 
   that to impose alignment on the enclosing structure.
   struct s {int a:8}; should have same alignment as "int", not "char".  */
#define	PCC_BITFIELD_TYPE_MATTERS	1

/* Largest integer machine mode for structures.  If undefined, the default
   is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 16

/* Make strings word-aligned so strcpy from constants will be faster.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN)  \
  ((TREE_CODE (EXP) == STRING_CST	\
    && (ALIGN) < FASTEST_ALIGNMENT)	\
   ? FASTEST_ALIGNMENT : (ALIGN))

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)		\
  (TREE_CODE (TYPE) == ARRAY_TYPE		\
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode	\
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))
     
/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* Trampolines for Nested Functions.  */
#define TRAMPOLINE_SIZE (2 + 6 + 6 + 2 + 2 + 6)

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 16

/* An alias for the machine mode for pointers.  */
#define Pmode         QImode

/* An alias for the machine mode used for memory references to
   functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE QImode

/* The register number of the stack pointer register, which must also
   be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM DCPU16_X

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM DCPU16_QFP

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM DCPU16_QAP

#define HARD_FRAME_POINTER_REGNUM DCPU16_X

#define ELIMINABLE_REGS							\
{{ FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM },			\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM }}			

/* This macro is similar to `INITIAL_FRAME_POINTER_OFFSET'.  It
   specifies the initial difference between the specified pair of
   registers.  This macro must be defined if `ELIMINABLE_REGS' is
   defined.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = dcpu16_initial_elimination_offset ((FROM), (TO));		\
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(r) (r >= DCPU16_A && r <= DCPU16_C)

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
  ((unsigned) (NUM) < FIRST_PSEUDO_REGISTER \
   && (REGNO_REG_CLASS(NUM) == GENERAL_REGS \
       || (NUM) == HARD_FRAME_POINTER_REGNUM))

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  (HARD_REGNO_OK_FOR_BASE_P(NUM) 		 \
   || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) DCPU16_I

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 1

/* All load operations zero extend.  */
#define LOAD_EXTEND_OP(MEM) ZERO_EXTEND

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE QImode

/* A C compound statement with a conditional `goto LABEL;' executed
   if X (an RTX) is a legitimate memory address on the target machine
   for a memory operand of mode MODE.  */
#define GO_IF_LEGITIMATE_ADDRESS(MODE,X,LABEL)		\
  do {                                                  \
    if (GET_CODE(X) == PLUS)				\
      {							\
	rtx op1,op2;					\
	op1 = XEXP(X,0);				\
	op2 = XEXP(X,1);				\
	if (GET_CODE(op1) == REG			\
	    && CONSTANT_ADDRESS_P(op2)			\
	    && REGNO_OK_FOR_BASE_P(REGNO(op1)))		\
	  goto LABEL;					\
      }							\
    if (REG_P (X) && REGNO_OK_FOR_BASE_P (REGNO (X)))	\
      goto LABEL;					\
    if (GET_CODE (X) == SYMBOL_REF			\
	|| GET_CODE (X) == LABEL_REF			\
	|| GET_CODE (X) == CONST)			\
      goto LABEL;					\
  } while (0)

/* Run-time Target Specification */

#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define_std ("dcpu16");		\
    builtin_define_std ("DCPU16");		\
  }

#define HAS_LONG_UNCOND_BRANCH true

#define WORD_REGISTER_OPERATIONS 1

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)	\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < 4)		\
    (MODE) = QImode;

#endif /* GCC_DCPU16_H */
