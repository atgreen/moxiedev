// cpu_execute.v - The moxie execute stage
//
// Copyright (c) 2010, 2011 Anthony Green.  All Rights Reserved.
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

`include "defines.v"

module cpu_execute (/*AUTOARG*/
  // Outputs
  register_write_index_o, register_write_enable_o,
  memory_write_enable_o, memory_write_address_o, result_o, riA_o,
  riB_o, branch_flag_o, branch_target_o,
  // Inputs
  rst_i, clk_i, stall_i, riA_i, riB_i, regA_i, regB_i,
  register_write_index_i, operand_i, op_i
  );

  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;

  // --- Pipeline interlocking ------------------------------------
  input  stall_i;
    
  // --- Inputs ---------------------------------------------------
  input [3:0] riA_i;
  input [3:0] riB_i;
  input [31:0] regA_i;
  input [31:0] regB_i;
  input [3:0]  register_write_index_i;
  input [31:0] operand_i;
  input [5:0]  op_i;
  
  // --- Outputs --------------------------------------------------
  output [3:0] register_write_index_o;
  output [0:0] register_write_enable_o;
  output [0:0] memory_write_enable_o;
  output [31:0] memory_write_address_o;
  output [31:0] result_o;
  output [3:0] riA_o;
  output [3:0] riB_o;

  output       branch_flag_o;
  output [31:0] branch_target_o;

  reg [0:0] 	branch_flag_o;
  reg [31:0] 	branch_target_o;

  reg [3:0]    register_write_index_o;
  reg [0:0] 	register_write_enable_o;
  reg [0:0] 	memory_write_enable_o;
  reg [31:0] 	memory_write_address_o;
  reg [31:0] 	result_o;

  assign riA_o = riA_i;
  assign riB_o = riB_o;

  always @(posedge rst_i or posedge clk_i)
    if (rst_i == 1)
      branch_flag_o <= 0;
    else 
      branch_flag_o <= (op_i == `OP_JMPA);
    
  always @(posedge rst_i or posedge clk_i)
    if (rst_i) begin
      register_write_enable_o <= 0;
      memory_write_enable_o <= 0;
    end else
      if (stall_i)
        begin
	  $display ("EXECUTE STALL");
	  memory_write_enable_o <= 0;
	end
      else begin
	case (op_i)
	  `OP_ADD_L:
	    begin
	      result_o <= regA_i + regB_i;
	      register_write_enable_o <= 1;
	      memory_write_enable_o <= 0;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_AND:
	    begin
	      result_o <= regA_i & regB_i;
	      register_write_enable_o <= 1;
	      memory_write_enable_o <= 0;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_ASHL:
	    begin
	      $display ("Executing OP_ASHL");
	    end
	  `OP_ASHR:
	    begin
	      $display ("Executing OP_ASHR");
	    end
	  `OP_BAD:
	    begin
	      $display ("Executing OP_BAD");
	    end
	  `OP_BEQ:
	    begin
	      $display ("Executing OP_BEQ");
	    end
	  `OP_BGE:
	    begin
	      $display ("Executing OP_BGE");
	    end
	  `OP_BGEU:
	    begin
	      $display ("Executing OP_BGEU");
	    end
	  `OP_BGT:
	    begin
	      $display ("Executing OP_BGT");
	    end
	  `OP_BGTU:
	    begin
	      $display ("Executing OP_BGTU");
	    end
	  `OP_BLE:
	    begin
	      $display ("Executing OP_BLE");
	    end
	  `OP_BLEU:
	    begin
	      $display ("Executing OP_BLEU");
	    end
	  `OP_BLT:
	    begin
	      $display ("Executing OP_BLT");
	    end
	  `OP_BLTU:
	    begin
	      $display ("Executing OP_BLTU");
	    end
	  `OP_BNE:
	    begin
	      $display ("Executing OP_BNE");
	    end
	  `OP_BRK:
	    begin
	      $display ("Executing OP_BRK");
	    end
	  `OP_CMP:
	    begin
	      $display ("Executing OP_CMP");
	    end
	  `OP_DEC:
	    begin
	      $display ("EXECUTE OP_DEC: 0x%x", operand_i);
	      result_o <= regA_i - operand_i;
	      register_write_enable_o <= 1;
	      memory_write_enable_o <= 0;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_DIV_L:
	    begin
	      $display ("Executing OP_DIV_L");
	    end
	  `OP_GSR:
	    begin
	      $display ("Executing OP_GSR");
	    end
	  `OP_INC:
	    begin
	      $display ("EXECUTE OP_INC: 0x%x", operand_i);
	      result_o <= regA_i + operand_i;
	      register_write_enable_o <= 1;
	      memory_write_enable_o <= 0;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_JMP:
	    begin
	      branch_target_o <= regA_i;
	      register_write_enable_o <= 0;
	      memory_write_enable_o <= 0;
	    end
	  `OP_JMPA:
	    begin
	      branch_target_o <= operand_i;
	      register_write_enable_o <= 0;
	      memory_write_enable_o <= 0;
	    end
	  `OP_JSR:
	    begin
	      $display ("Executing OP_JSR");
	    end
	  `OP_JSRA:
	    begin
	      $display ("Executing OP_JSRA");
	    end
	  `OP_LDA_B:
	    begin
	      $display ("Executing OP_LDA_B");
	    end
	  `OP_LDA_L: 
	    begin
	      memory_address_o <= operand_i;
	    end
	  `OP_LDA_S:
	    begin
	      $display ("Executing OP_LDA_S");
	    end
	  `OP_LD_B:
	    begin
	      $display ("Executing OP_LD_B");
	    end
	  `OP_LDI_B:
	    begin
	      $display ("Executing OP_LDI_B");
	    end
	  `OP_LDI_L:
	    begin
	      $display ("EXECUTE OP_LDI_L: 0x%x", operand_i);
	      result_o <= operand_i;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_LDI_S:
	    begin
	      $display ("Executing OP_LDI_S");
	    end
	  `OP_LD_L:
	    begin
	      $display ("Executing OP_LD_L");
	    end
	  `OP_LDO_B:
	    begin
	      $display ("Executing OP_LDO_B");
	    end
	  `OP_LDO_L:
	    begin
	      $display ("Executing OP_LDO_L");
	    end
	  `OP_LDO_S:
	    begin
	      $display ("Executing OP_LDO_S");
	    end
	  `OP_LD_S:
	    begin
	      $display ("Executing OP_LD_S");
	    end
	  `OP_LSHR:
	    begin
	      $display ("Executing OP_LSHR");
	    end
	  `OP_MOD_L:
	    begin
	      $display ("Executing OP_MOD_L");
	    end
	  `OP_MOV:
	    begin
	      result_o <= regB_i;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_MUL_L:
	    begin
	      $display ("Executing OP_MUL_L");
	    end
	  `OP_NEG:
	    begin
	      $display ("Executing OP_NEG");
	    end
	  `OP_NOP:
	    begin
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 0;
	    end
	  `OP_NOT:
	    begin
	      $display ("Executing OP_NOT");
	    end
	  `OP_OR:
	    begin
	      result_o <= regA_i | regB_i;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_POP:
	    begin
	      $display ("Executing OP_POP");
	    end
	  `OP_PUSH:
	    begin
	      $display ("Executing OP_PUSH");
	    end
	  `OP_RET:
	    begin
	      $display ("Executing OP_RET");
	    end
	  `OP_SSR:
	    begin
	      $display ("Executing OP_SSR");
	    end
	  `OP_STA_B:
	    begin
	      $display ("Executing OP_STA_B");
	    end
	  `OP_STA_L:
	    begin
	      result_o <= regA_i;
	      memory_write_enable_o <= 1;
	      memory_write_address_o <= operand_i;
	    end
	  `OP_STA_S:
	    begin
	      $display ("Executing OP_STA_S");
	    end
	  `OP_ST_B:
	    begin
	      $display ("Executing OP_ST_B");
	    end
	  `OP_ST_L:
	    begin
	      $display ("Executing OP_ST_L");
	    end
	  `OP_STO_B:
	    begin
	      $display ("Executing OP_STO_B");
	    end
	  `OP_STO_L:
	    begin
	      $display ("Executing OP_STO_L");
	    end
	  `OP_STO_S:
	    begin
	      $display ("Executing OP_STO_S");
	    end
	  `OP_ST_S:
	    begin
	      $display ("Executing OP_ST_S");
	    end
	  `OP_SUB_L:
	    begin
	      result_o <= regA_i - regB_i;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      register_write_index_o <= register_write_index_i;
	    end
	  `OP_SWI:
	    begin
	      $display ("Executing OP_SWI");
	    end
	  `OP_UDIV_L:
	    begin
	      $display ("Executing OP_UDIV_L");
	    end
	  `OP_UMOD_L:
	    begin
	      $display ("Executing OP_UMOD_L");
	    end
	  `OP_XOR:
	    begin
	      result_o <= regA_i ^ regB_i;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      register_write_index_o <= register_write_index_i;
	    end
	endcase
      end
endmodule // cpu_execute;
