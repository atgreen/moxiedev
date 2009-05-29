// moxie.v - The Moxie Core
//
// Copyright (c) 2009 Anthony Green.  All Rights Reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES.
// 
// The above named program is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
// 
// The above named program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this work; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
// 02110-1301, USA.

// Set the boot address upon system reset
`define BOOT_ADDRESS 32'h00001000 

module MoxieCore (/*AUTOARG*/
   // Inputs
   reset, clk
   );
   
   // --- Clock and Reset ------------------------------------------
   input  reset, clk;

   // --- Register file --------------------------------------------
   reg[31:0] GPRS[15:0];    // General Purpose Registers
   reg[31:0] SPRS[255:0];   // Special Purpose Registers
   reg[31:0] PC;            // Program Counter

   reg[63:0] IB;            // Instruction Buffer
   
   // Incremented Program Counter
   wire[31:0] PCnext = PC + 2;
   
   // Index for register initialization
   integer i;

   // Instruction Fetch state
   reg  [2:0] if_state;
   wire [2:0] if_next_state;
   reg  [2:0] if_IB_index;
   reg  [1:0] idif_read_word_0;
   reg  [1:0] idif_read_word_1;

   reg  [32:0] MEM[99:0];

   always @ (posedge clk) begin
      
      // Initialize all the registers
      if (reset == 1) begin
	 
	 // Initialize gprs and sprs registers
	 for (i=0; i<=16; i=i+1) GPRS[i] <= 32'h00000000;
	 for (i=0; i<=256; i=i+1) SPRS[i] <= 32'h00000000;
	 
	 // Initialize program counter
	 PC <= `BOOT_ADDRESS;
	 
	 // Initialize the Instruction Fetch state
	 if_state <= 0;
	 if_IB_index <= 0;
	 idif_read_word_0 <= 0;
	 idif_read_word_1 <= 0;
	 
	 $display("** RESET **");
	 
      end else begin
	 
	 $display("================> Time %t <================", $time);
	 $display("================> PC %d <================", PC);
	 $display("idif_read_word_0 %d", idif_read_word_0);
	 $display("idif_read_word_1 %d", idif_read_word_1);
	 PC <= PCnext;

      end
   end
	 
   /* ----------------------------------------------------------------
    * The Moxie core has a four stage pipeline
    * 
    * 1. INSTRUCTION FETCH
    * 2. INSTRUCTION DECODE
    * 3. INSTRUCTION EXECUTE
    * 4. WRITE RESULT
    * ---------------------------------------------------------------- */
   
   /* ================================================================
    * STAGE 1: INSTRUCTION FETCH
    * ================================================================
    *
    * Instructions may be as long as 48-bits, however we only
    * have a 32-bit path to instruction memory.  So, let's maintain a
    * circular 64-bit instruction buffer (IB) and a state
    * machine to control reads into that buffer. 
    * 
    * Our state machine has 16 states.  The top row in the
    * following table is a bitmap, where 0 represents an empty 32-bit
    * word in the IB, and 1 is a valid 32-bit word.  The column
    * on the left hand side indicates our current index into the
    * IB in bits.  For each cell in this table, 0 means fetch
    * the lower 32-bit word and 1 means fetch the upper 32-bit word.
    * x is an impossible state.
    * 
    *       00 01 10 11
    *    00  0  0  1  x
    *    16  0  0  1  x
    *    32  1  0  1  x
    *    48  1  0  1  x
    * 
    * During the INSTRUCTION DECODE stage, we will insert
    * bubbles into the pipeline if we don't have enough valid
    * bits in the IB.  */
   
   
   assign if_next_state = if_get_next_state(if_state, if_IB_index);
   
   function [2:0] if_get_next_state;
      input [2:0] state;
      input [2:0] index;
      case (state)
 	2'b00: 
 	  if (index < 2) begin
 	     if_get_next_state = 2'b10;
	  end else begin
 	     if_get_next_state = 2'b01;
 	  end
 	2'b01:
 	  // Read 0.
 	  if_get_next_state = 2'b11;
 	2'b10:
 	  // Read 1
 	  if_get_next_state = 2'b11;
 	2'b11:
 	  if_get_next_state = 2'b11;
      endcase // case (state)
   endfunction
   
   always @ (posedge clk) begin
      if (if_state == 2'b00 && if_next_state == 2'b10)
	IB[31:0] <= MEM[PC];
      else if (if_state == 2'b00 && if_next_state == 2'b01)
	IB[63:32] <= MEM[PC];
      else if (if_state == 2'b01)
	IB[31:0] <= MEM[PC];
      else if (if_state == 2'b10)
	IB[63:32] <= MEM[PC];

      if (idif_read_word_0 == 1 && if_next_state == 2'b11)
	if_state = 2'b01;
      else if (idif_read_word_1 == 1 && if_next_state == 2'b11)
	if_state = 2'b10;
      else
	if_state = if_next_state;
      $display("=== STATE = %b", if_state);
   end
   
   /* ================================================================
    * STAGE 2: INSTRUCTION DECODE
    * ================================================================
    *
    * Look at the first 16 bits of the instruction.  */

   reg [15:0] insn;
   
   always @ (posedge clk) begin
      case (if_IB_index)
	0:
	  begin
	     insn <= IB[15:0];
	     idif_read_word_0 = #1 0;
	     idif_read_word_1 = #1 0;
	  end
	1:
	  begin
	     insn = #1 IB[31:16];
	     idif_read_word_0 = #1 1;
	     idif_read_word_1 = #1 0;
	  end
	2:
	  begin
	     insn = #1 IB[47:32];
	     idif_read_word_0 = #1 0;
	     idif_read_word_1 = #1 0;
	  end
	3:
	  begin
	     insn = #1 IB[63:48];
	     idif_read_word_0 = #1 0;
	     idif_read_word_1 = #1 1;
	  end
      endcase // case (if_IB_index)

      // This is a rediculous switch.  Eventually we should recode
      // the opcodes so we can easily determine the length of the
      // instruction.
      if ((insn[15:8] == 8'b00001111)    // beq
	  || (insn[15:8] == 8'b00010101) // bge
	  || (insn[15:8] == 8'b00010111) // bgeu
	  || (insn[15:8] == 8'b00010010) // bgt
	  || (insn[15:8] == 8'b00010100) // bgtu
	  || (insn[15:8] == 8'b00010110) // ble
	  || (insn[15:8] == 8'b00011000) // bleu
	  || (insn[15:8] == 8'b00010001) // blt
	  || (insn[15:8] == 8'b00010011) // bltu
	  || (insn[15:8] == 8'b00010000) // bne
	  || (insn[15:8] == 8'b00011010) // jmpa
	  || (insn[15:8] == 8'b00000011) // jsra
	  || (insn[15:8] == 8'b00011101) // lda.b
	  || (insn[15:8] == 8'b00001000) // lda.l
	  || (insn[15:8] == 8'b00010011) // lda.s
	  || (insn[15:8] == 8'b00000001) // ldi.l
	  || (insn[15:8] == 8'b00011011) // ldi.b
	  || (insn[15:8] == 8'b00100000) // ldi.s
	  || (insn[15:8] == 8'b00110110) // ldo.b
	  || (insn[15:8] == 8'b00001100) // ldo.l
	  || (insn[15:8] == 8'b00111000) // ldo.s
	  || (insn[15:8] == 8'b00001001) // sta.b
	  || (insn[15:8] == 8'b00100100) // sta.s
	  || (insn[15:8] == 8'b00110111) // sto.b
	  || (insn[15:8] == 8'b00001101) // sto.l
	  || (insn[15:8] == 8'b00111001)) // sto.s
	/* This is a 48-bit instruction.  */

      idex_insn = insn;
	
   end

   always @ (negedge clk) begin
      $display ("if_IB_index = %d", if_IB_index);
      if_IB_index <= (if_IB_index + 1) % 4;
   end


endmodule // MoxieCore

