/* Instruction scanner for The Incredible Moxielyzer 
   (C) 2009 Anthony Green

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

void
moxielyze (bfd_size_type size, bfd_byte *code, bfd_vma *addr, moxie_analyzer *analyzer)
{				     
  size_t offset = 0;
  unsigned long i, icount;
  size_t opc, pc = *addr;	     
  unsigned short insn;

  /* Save some state in the analyzer.  */
  analyzer->code = code;
  
  /* Allocate an array large enough to hold offsets for the total
     number of instructions, so we can easily traverse the
     instructions backwards and forwards during analysis.  Also be
     sure to track some bits of info we care about.  */
  
  analyzer->ioffsets = malloc (size * sizeof (size_t));
  analyzer->iflags   = malloc (size * sizeof (unsigned long));

  /* Populate this array by scanning all instructions.  All
     unrecognized instructions are assumed to be 16-bits long.  */

  for (icount = 0; offset < size; icount++)
    {				     
      /* Record the start of this instruction.  */
      analyzer->ioffsets[icount] = offset;
						   
      /* Fetch the instruction at pc.  */	 
      insn = code[offset] << 8 + code[offset+1]; 
      						 
      /* Decode instruction.  */		 
      if (insn & (1 << 15))			 
	{					 
	  if (insn & (1 << 14))			   
	    {					   
	      /* This is a Form 3 instruction.  */ 
	      int opcode = (insn >> 10 & 0xf);	   

	      switch (opcode)			   
		{				   
		case 0x00: /* beq */		   
		case 0x01: /* bne */			       
		case 0x02: /* blt */			       
		case 0x03: /* bgt */			       
		case 0x04: /* bltu */			       
		case 0x05: /* bgtu */				
		case 0x06: /* bge */				
		case 0x07: /* ble */				
		case 0x08: /* bgeu */				
		case 0x09: /* bleu */
		  analyzer->iflags[icount] |= IS_BRANCH;
		default:
		  /* Undefined.  Assume 16-bits.  */
		  offset += 2;
		  break;					
		}						
	    }							
	  else							
	    {							
	      /* This is a Form 2 instruction.  */		
	      int opcode = (insn >> 12 & 0x3);			
								
	      switch (opcode)					
		{						
		case 0x02: /* gsr */				
		case 0x03: /* ssr */				
		  offset += 6;				
		  break;					
		case 0x00: /* inc */				
		case 0x01: /* dec */				
		default:
		  /* Undefined.  Assume 16-bits.  */
		  offset += 2;
		  break;					
		}						
	    }							
	}							
      else							
	{							
	  /* This is a Form 1 instruction.  */			
	  int opcode = insn >> 8;				
	  switch (opcode)					
	    {							
 	    case 0x03: /* jsra */
	    case 0x1a: /* jmpa */				
	      analyzer->iflags[icount] |= IS_JUMP;
	      offset += 6;					
	      break;

	    case 0x0d: /* sto.l */
	    case 0x37: /* sto.b */				
	    case 0x39: /* sto.s */				
	      analyzer->iflags[icount] |= IS_ADDRESSING_OFFSET;
	      analyzer->iflags[icount] |= IS_INDIRECT_A;
	      offset += 6;					
	      break;						

	    case 0x0c: /* ldo.l */
	    case 0x36: /* ldo.b */				
	    case 0x38: /* ldo.s */				
	      analyzer->iflags[icount] |= IS_ADDRESSING_OFFSET;
	      analyzer->iflags[icount] |= IS_INDIRECT_B;
	      offset += 6;					
	      break;						

	    case 0x01: /* ldi.l (immediate) */			
	    case 0x08: /* lda.l */				
	    case 0x09: /* sta.l */				
	    case 0x1b: /* ldi.b (immediate) */			
	    case 0x1d: /* lda.b */				
	    case 0x1f: /* sta.b */				
	    case 0x20: /* ldi.s (immediate) */			
	    case 0x22: /* lda.s */				
	    case 0x24: /* sta.s */				
	    case 0x30: /* swi */				
	      offset += 6;					
	      break;						

	    case 0x25: /* jmp */				
	    case 0x19: /* jsr */				
	      analyzer->iflags[icount] |= IS_JUMP;
	      offset += 2;
	      break;

	    case 0x00: /* bad */				
	    case 0x02: /* mov (register-to-register) */		
 	    case 0x04: /* ret */				
	    case 0x05: /* add.l */				
	    case 0x06: /* push */				
	    case 0x07: /* pop */				
	    case 0x0a: /* ld.l (register indirect) */		
	    case 0x0b: /* st.l */				
	    case 0x0e: /* cmp */				
	    case 0x0f: /* nop */				
	    case 0x1c: /* ld.b (register indirect) */		
	    case 0x1e: /* st.b */				
	    case 0x21: /* ld.s (register indirect) */		
	    case 0x23: /* st.s */				
	    case 0x26: /* and */				
	    case 0x27: /* lshr */				
	    case 0x28: /* ashl */				
	    case 0x29: /* sub.l */				
	    case 0x2a: /* neg */				
	    case 0x2b: /* or */					
	    case 0x2c: /* not */				
	    case 0x2d: /* ashr */				
	    case 0x2e: /* xor */				
	    case 0x2f: /* mul.l */				
	    case 0x31: /* div.l */				
	    case 0x32: /* udiv.l */				
	    case 0x33: /* mod.l */				
	    case 0x34: /* umod.l */				
	    case 0x35: /* brk */				
	    default:						
	      offset += 2;					
	      break;						
	    }							
	}
      icount++;
    }
  
  offset = 0;

  for (i = 0; i < icount; i++)
    {				     
      opc = pc;				  
      					  
      /* Fetch the instruction at pc.  */	 
      insn = code[analyzer->ioffsets[i]] << 8 
	+ code[analyzer->ioffsets[i]+1]; 
      						 
#define CALLBACK(INSN) if (analyzer->callback[INSN]) (analyzer->callback[INSN])(analyzer, i, INSN)

      CALLBACK(moxie_i);

      /* Decode instruction.  */		 
      if (insn & (1 << 15))			 
	{					 
	  if (insn & (1 << 14))			   
	    {					   
	      /* This is a Form 3 instruction.  */ 
	      int opcode = (insn >> 10 & 0xf);	   
						   
	      switch (opcode)			   
		{				   
		case 0x00: /* beq */		   
		  {
		    CALLBACK(moxie_i_beq);
		  }					       
		  break;				       
		case 0x01: /* bne */			       
		  {					       
		    CALLBACK(moxie_i_bne);
		  }					       
		  break;				       
		case 0x02: /* blt */			       
		  {					       
		    CALLBACK(moxie_i_blt);
		  }					       
		  break;				       
		case 0x03: /* bgt */			       
		  {					       
		    CALLBACK(moxie_i_bgt);
		  }					       
		  break;				       
		case 0x04: /* bltu */			       
		  {					       
		    CALLBACK(moxie_i_bltu);
		  }						
		  break;					
		case 0x05: /* bgtu */				
		  {						
		    CALLBACK(moxie_i_bgtu);
		  }						
		  break;					
		case 0x06: /* bge */				
		  {						
		    CALLBACK(moxie_i_bge);
		  }						
		  break;					
		case 0x07: /* ble */				
		  {						
		    CALLBACK(moxie_i_ble);
		  }						
		  break;					
		case 0x08: /* bgeu */				
		  {						
		    CALLBACK(moxie_i_bgeu);
		  }						
		  break;					
		case 0x09: /* bleu */				
		  {						
		    CALLBACK(moxie_i_bleu);
		  }						
		  break;					
		default:					
		  {						
		    CALLBACK(moxie_i_bad);
		  }						
		  break;					
		}						
	    }							
	  else							
	    {							
	      /* This is a Form 2 instruction.  */		
	      int opcode = (insn >> 12 & 0x3);			
								
	      switch (opcode)					
		{						
		case 0x00: /* inc */				
		  {						
		    CALLBACK(moxie_i_inc);
		  }						
		  break;					
		case 0x01: /* dec */				
		  {						
		    CALLBACK(moxie_i_dec);
		  }						
		  break;					
		case 0x02: /* gsr */				
		  {						
		    CALLBACK(moxie_i_gsr);
		  }						
		  break;					
		case 0x03: /* ssr */				
		  {						
		    CALLBACK(moxie_i_ssr);
		  }						
		  break;					
		default:					
		  {						
		    CALLBACK(moxie_i_bad);
		  }						
		  break;					
		}						
	    }							
	}							
      else							
	{							
	  /* This is a Form 1 instruction.  */			
	  int opcode = insn >> 8;				
	  switch (opcode)					
	    {							
	    case 0x00: /* bad */				
	      {							
		CALLBACK(moxie_i_bad);
	      }							
	      break;						
	    case 0x01: /* ldi.l (immediate) */			
	      {							
		CALLBACK(moxie_i_ldi_l);
	      }							
	      break;						
	    case 0x02: /* mov (register-to-register) */		
	      {							
		CALLBACK(moxie_i_mov);
	      }							
	      break;						
 	    case 0x03: /* jsra */				
 	      {							
		CALLBACK(moxie_i_jsra);
 	      }							
 	      break;						
 	    case 0x04: /* ret */				
 	      {							
		CALLBACK(moxie_i_ret);
  	      }							
  	      break;						
	    case 0x05: /* add.l */				
	      {							
		CALLBACK(moxie_i_add_l);
	      }							
	      break;						
	    case 0x06: /* push */				
	      {							
		CALLBACK(moxie_i_push);
	      }							
	      break;						
	    case 0x07: /* pop */				
	      {							
		CALLBACK(moxie_i_pop);
	      }							
	      break;						
	    case 0x08: /* lda.l */				
	      {							
		CALLBACK(moxie_i_lda_l);
	      }							
	      break;						
	    case 0x09: /* sta.l */				
	      {							
		CALLBACK(moxie_i_sta_l);
	      }							
	      break;						
	    case 0x0a: /* ld.l (register indirect) */		
	      {							
		CALLBACK(moxie_i_ld_l);
	      }							
	      break;						
	    case 0x0b: /* st.l */				
	      {							
		CALLBACK(moxie_i_st_l);
	      }							
	      break;						
	    case 0x0c: /* ldo.l */				
	      {							
		CALLBACK(moxie_i_ldo_l);
	      }							
	      break;						
	    case 0x0d: /* sto.l */				
	      {							
		CALLBACK(moxie_i_sto_l);
	      }							
	      break;						
	    case 0x0e: /* cmp */				
	      {							
		CALLBACK(moxie_i_cmp);
	      }							
	      break;						
	    case 0x0f: /* nop */				
	      {							
		CALLBACK(moxie_i_nop);
	      }							
	      break;						
	    case 0x10:						
	    case 0x11:						
	    case 0x12:						
	    case 0x13:						
	    case 0x14:						
	    case 0x15:						
	    case 0x16:						
	    case 0x17:						
	    case 0x18:						
	      {							
		CALLBACK(moxie_i_bad);
	      }							
	      break;						
	    case 0x19: /* jsr */				
	      {							
		CALLBACK(moxie_i_jsr);
	      }							
	      break;						
	    case 0x1a: /* jmpa */				
	      {							
		CALLBACK(moxie_i_jmpa);
	      }							
	      break;						
	    case 0x1b: /* ldi.b (immediate) */			
	      {							
		CALLBACK(moxie_i_ldi_b);
	      }							
	      break;						
	    case 0x1c: /* ld.b (register indirect) */		
	      {							
		CALLBACK(moxie_i_ld_b);
	      }							
	      break;						
	    case 0x1d: /* lda.b */				
	      {							
		CALLBACK(moxie_i_lda_b);
	      }							
	      break;						
	    case 0x1e: /* st.b */				
	      {							
		CALLBACK(moxie_i_st_b);
	      }							
	      break;						
	    case 0x1f: /* sta.b */				
	      {							
		CALLBACK(moxie_i_sta_b);
	      }							
	      break;						
	    case 0x20: /* ldi.s (immediate) */			
	      {							
		CALLBACK(moxie_i_ldi_s);
	      }							
	      break;						
	    case 0x21: /* ld.s (register indirect) */		
	      {							
		CALLBACK(moxie_i_ld_s);
	      }							
	      break;						
	    case 0x22: /* lda.s */				
	      {							
		CALLBACK(moxie_i_lda_s);
	      }							
	      break;						
	    case 0x23: /* st.s */				
	      {							
		CALLBACK(moxie_i_st_s);
	      }							
	      break;						
	    case 0x24: /* sta.s */				
	      {							
		CALLBACK(moxie_i_sta_s);
	      }							
	      break;						
	    case 0x25: /* jmp */				
	      {							
		CALLBACK(moxie_i_jmp);
	      }							
	      break;						
	    case 0x26: /* and */				
	      {							
		CALLBACK(moxie_i_and);
	      }							
	      break;						
	    case 0x27: /* lshr */				
	      {							
		CALLBACK(moxie_i_lshr);
	      }							
	      break;						
	    case 0x28: /* ashl */				
	      {							
		CALLBACK(moxie_i_ashl);
	      }							
	      break;						
	    case 0x29: /* sub.l */				
	      {							
		CALLBACK(moxie_i_sub_l);
	      }							
	      break;						
	    case 0x2a: /* neg */				
	      {							
		CALLBACK(moxie_i_neg);
	      }							
	      break;						
	    case 0x2b: /* or */					
	      {							
		CALLBACK(moxie_i_or);
	      }							
	      break;						
	    case 0x2c: /* not */				
	      {							
		CALLBACK(moxie_i_not);
	      }							
	      break;						
	    case 0x2d: /* ashr */				
	      {							
		CALLBACK(moxie_i_ashr);
	      }							
	      break;						
	    case 0x2e: /* xor */				
	      {							
		CALLBACK(moxie_i_xor);
	      }							
	      break;						
	    case 0x2f: /* mul.l */				
	      {							
		CALLBACK(moxie_i_mul_l);
	      }							
	      break;						
	    case 0x30: /* swi */				
	      {							
		CALLBACK(moxie_i_swi);
	      }							
	      break;						
	    case 0x31: /* div.l */				
	      {							
		CALLBACK(moxie_i_div_l);
	      }							
	      break;						
	    case 0x32: /* udiv.l */				
	      {							
		CALLBACK(moxie_i_udiv_l);
	      }							
	      break;						
	    case 0x33: /* mod.l */				
	      {							
		CALLBACK(moxie_i_mod_l);
	      }							
	      break;						
	    case 0x34: /* umod.l */				
	      {							
		CALLBACK(moxie_i_umod_l);
	      }							
	      break;						
	    case 0x35: /* brk */				
	      {							
		CALLBACK(moxie_i_brk);
	      }							
	      break;						
	    case 0x36: /* ldo.b */				
	      {							
		CALLBACK(moxie_i_ldo_b);
	      }							
	      break;						
	    case 0x37: /* sto.b */				
	      {							
		CALLBACK(moxie_i_sto_b);
	      }							
	      break;						
	    case 0x38: /* ldo.s */				
	      {							
		CALLBACK(moxie_i_ldo_s);
	      }							
	      break;						
	    case 0x39: /* sto.s */				
	      {							
		CALLBACK(moxie_i_sto_s);
	      }							
	      break;						
	    default:						
	      {							
		CALLBACK(moxie_i_bad);
	      }							
	      break;						
	    }							
	}							
								
      pc += 2;							
    }
}
 
  
