// cpu_ifetch.v - The instruction fetch unit.
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

module cpu_ifetch (/*AUTOARG*/
  // Outputs
  ifid_insn_o, ifid_insn_data_o, ifid_insn_valid_o,
  ifid_insn_data_valid_o,
  // Inputs
  rst_i, clk_i, idif_used_insn_i, idif_used_data_i
  );

   // --- Clock and Reset ------------------------------------------
   input  rst_i, clk_i;
   
   // --- Instruction Buffer ---------------------------------------
   reg[63:0] IB;            // Instruction Buffer

   // --- Inputs from Instruction Decoder --------------------------
   input  idif_used_insn_i, idif_used_data_i;
   
   // --- Instruction Buffer ---------------------------------------
   output [15:0] ifid_insn_o;
   output [31:0] ifid_insn_data_o;
   output ifid_insn_valid_o, ifid_insn_data_valid_o;
   reg [15:0] ifid_insn_o;
   reg [31:0] ifid_insn_data_o;
   reg [0:0]  ifid_insn_valid_o;
   reg [0:0]  ifid_insn_data_valid_o;
   
   // --- Instruction Buffer ---------------------------------------
   reg [63:0] ibuf_o;
   wire [3:0]  pc_index;
   reg  [3:0]  pc_index_reg;
   
   // --- Instruction Fetch state ----------------------------------
   reg [1:0]  if_state;
   wire [1:0] if_next_state;
   reg [2:0]  if_IB_index;
   reg [1:0]  idif_read_word_0;
   reg [1:0]  idif_read_word_1;
   
   // --- Test memory.  Let's just read from an internal array.  */
   reg [7:0]  MEM [0:128000];
   reg [31:0] PC; /* For testing only.  */
   
   initial
     begin
	$readmemh("hello.vh", MEM);
     end

   // --- Combinational Logic --------------------------------------
   assign pc_index = (rst_i == 0 ? (pc_index_reg 
				    + (idif_used_insn_i ? 1 : 0)
				    + (idif_used_data_i ? 2 : 0)) % 4
		      : 0);

   // --- Reset Logic ----------------------------------------------
   always @ (posedge clk_i) begin
      // Initialize all the registers
      if (rst_i == 1) begin
	 
	 // Initialize the Instruction Fetch state
	 if_state <= 0;
	 if_IB_index <= 0;
	 idif_read_word_0 <= 0;
	 idif_read_word_1 <= 0;

	 PC <= `BOOT_ADDRESS;
	 pc_index_reg <= 0;
      end
   end
   
   /* Instructions may be as long as 48-bits, however we only
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
   
   function [1:0] if_get_next_state;
      input [1:0] state;
      input [1:0] index;
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

   always @ (posedge clk_i) begin
      $display("pc_index = 0x%x\t: 0x%x\t0x%x\t0x%x\t0x%x", pc_index,
	       IB[15:0], IB[31:16], IB[47:32], IB[63:48]);
      
      case (pc_index)
        2'b00:
	  begin
	     $display("A");
             ifid_insn_o = IB[63:48];
	     ifid_insn_data_o = IB[47:16];
	     ifid_insn_valid_o = if_state[1] == 1;
	     ifid_insn_data_valid_o = if_state[0] == 1;
	  end
        2'b01:
	  begin
	     $display("B");
             ifid_insn_o = IB[47:32];
	     ifid_insn_data_o = IB[31:0];
	     ifid_insn_valid_o = if_state[1] == 1;
	     ifid_insn_data_valid_o = if_state[0] == 1;
	  end
        2'b10:
          begin
	     $display("C");
	     ifid_insn_o = IB[31:16];
	     ifid_insn_data_o[15:0] = IB[15:0];
	     ifid_insn_data_o[31:16] = IB[63:48];
	     ifid_insn_valid_o = if_state[0] == 1;
	     ifid_insn_data_valid_o = if_state[1] == 1;
	  end
	2'b11:
	  begin
	     $display("D");
	     ifid_insn_o = IB[15:0];
	     ifid_insn_data_o = IB[63:32];
	     ifid_insn_valid_o = if_state[0] == 1;
	     ifid_insn_data_valid_o = if_state[1] == 1;
	  end
      endcase // case (pc_index)
      pc_index_reg = pc_index;
      $display("insn = 0x%x", ifid_insn_o);
      $display("data = 0x%x", ifid_insn_data_o);
   end   
   
   always @ (posedge clk_i) begin
      $display("MEM[PC] = 0x%x", MEM[PC]);
      $display("if_state = 0x%x", if_state);
      if (if_state == 2'b00 && if_next_state == 2'b10)
	begin
	   IB[63:56] = MEM[PC];
	   IB[55:48] = MEM[PC+1];
	   IB[47:40] = MEM[PC+2];
	   IB[39:32] = MEM[PC+3];
	   PC = PC+4;
	end
      else if (if_state == 2'b00 && if_next_state == 2'b01)
	begin
	   IB[31:24] = MEM[PC];
	   IB[23:16] = MEM[PC+1];
	   IB[15:8] = MEM[PC+2];
	   IB[7:0] = MEM[PC+3];
	   PC = PC+4;
	end
      else if (if_state == 2'b01)
	begin
	   IB[63:56] = MEM[PC];
	   IB[55:48] = MEM[PC+1];
	   IB[47:40] = MEM[PC+2];
	   IB[39:32] = MEM[PC+3];
	   PC = PC+4;
	end
      else if (if_state == 2'b10)
	begin
	   IB[31:24] = MEM[PC];
	   IB[23:16] = MEM[PC+1];
	   IB[15:8] = MEM[PC+2];
	   IB[7:0] = MEM[PC+3];
	   PC = PC+4;
	end
      $display("IB = 0x%x", IB);

      if (idif_read_word_0 == 1 && if_next_state == 2'b11)
	if_state = 2'b01;
      else if (idif_read_word_1 == 1 && if_next_state == 2'b11)
	if_state = 2'b10;
      else
	if_state = if_next_state;
   end

endmodule // cpu_ifetch
