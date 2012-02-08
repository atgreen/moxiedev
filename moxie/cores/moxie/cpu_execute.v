// cpu_execute.v - The moxie execute stage
//
// Copyright (c) 2010, 2011, 2012 Anthony Green.
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
  memory_read_enable_o, memory_write_enable_o, memory_address_o,
  reg_result_o, mem_result_o, riA_o, riB_o, stall_o, branch_flag_o,
  branch_target_o,
  // Inputs
  rst_i, clk_i, stall_i, riA_i, riB_i, regA_i, regB_i,
  register_write_index_i, operand_i, op_i, sp_i, fp_i, PC_i
  );

  parameter [1:0] STATE_READY = 2'b00,
    STATE_JSR1 = 2'b01,
    STATE_RET1 = 2'b10;

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
  input [31:0] sp_i;
  input [31:0] fp_i;
  input [31:0] PC_i;
  
  // --- Outputs --------------------------------------------------
  output [3:0] register_write_index_o;
  output [0:0] register_write_enable_o;
  output [0:0] memory_read_enable_o;
  output [0:0] memory_write_enable_o;
  output [31:0] memory_address_o;  
  output [31:0] reg_result_o;
  output [31:0] mem_result_o;
  output [3:0] riA_o;
  output [3:0] riB_o;

  output [0:0] stall_o;
  reg [0:0]    stall_o;
  
  output       branch_flag_o;
  output [31:0] branch_target_o;

  reg [0:0] 	branch_flag_o;
  reg [31:0] 	branch_target_o;

  reg [3:0]    register_write_index_o;
  reg [0:0] 	register_write_enable_o;
  reg [0:0] 	memory_read_enable_o;
  reg [0:0] 	memory_write_enable_o;
  reg [31:0] 	memory_address_o;
  reg [31:0] 	reg_result_o;
  reg [31:0] 	mem_result_o;

  reg [1:0] 	current_state, next_state;
  
  assign riA_o = riA_i;
  assign riB_o = riB_i;

  always @(posedge rst_i or posedge clk_i)
    if (rst_i == 1) begin
      branch_flag_o <= 0;
      current_state <= STATE_READY;
    end else begin
      branch_flag_o <= (op_i == `OP_JMPA) || (current_state == STATE_JSR1);
      current_state <= next_state;
    end
    
  always @(posedge rst_i or posedge clk_i)
    if (rst_i) begin
      register_write_enable_o <= 0;
      memory_read_enable_o <= 0;
      memory_write_enable_o <= 0;
      stall_o <= 0;
    end else
      if (stall_i)
        begin
	  $display ("EXECUTE STALL");
	  memory_read_enable_o <= 0;
	  memory_write_enable_o <= 0;
	end
      else begin
	case (current_state)
	  STATE_READY:
	    begin
	      case (op_i)
		`OP_ADD_L:
		  begin
		    reg_result_o <= regA_i + regB_i;
		    register_write_enable_o <= 1;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_AND:
		  begin
		    reg_result_o <= regA_i & regB_i;
		    register_write_enable_o <= 1;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_ASHL:
		  begin
		    $display ("Executing OP_ASHL");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_ASHR:
		  begin
		    $display ("Executing OP_ASHR");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BAD:
		  begin
		    $display ("Executing OP_BAD");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BEQ:
		  begin
		    $display ("Executing OP_BEQ");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BGE:
		  begin
		    $display ("Executing OP_BGE");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BGEU:
		  begin
		    $display ("Executing OP_BGEU");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BGT:
		  begin
		    $display ("Executing OP_BGT");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BGTU:
		  begin
		    $display ("Executing OP_BGTU");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BLE:
		  begin
		    $display ("Executing OP_BLE");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BLEU:
		  begin
		    $display ("Executing OP_BLEU");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BLT:
		  begin
		    $display ("Executing OP_BLT");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BLTU:
		  begin
		    $display ("Executing OP_BLTU");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BNE:
		  begin
		    $display ("Executing OP_BNE");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_BRK:
		  begin
		    $display ("Executing OP_BRK");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_CMP:
		  begin
		    $display ("Executing OP_CMP");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_DEC:
		  begin
		    $display ("EXECUTE OP_DEC: 0x%x", operand_i);
		    reg_result_o <= regA_i - operand_i;
		    register_write_enable_o <= 1;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_DIV_L:
		  begin
		    $display ("Executing OP_DIV_L");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_GSR:
		  begin
		    $display ("Executing OP_GSR");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_INC:
		  begin
		    $display ("EXECUTE OP_INC: 0x%x", operand_i);
		    reg_result_o <= regA_i + operand_i;
		    register_write_enable_o <= 1;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_JMP:
		  begin
		    branch_target_o <= regA_i;
		    register_write_enable_o <= 0;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_JMPA:
		  begin
		    branch_target_o <= operand_i;
		    register_write_enable_o <= 0;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_JSR:
		  begin
		    // Decrement $sp by 8 bytes and store the return address.
		    reg_result_o <= sp_i - 8;
		    memory_address_o <= sp_i - 8;
		    mem_result_o <= PC_i+6;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 1;
		    register_write_enable_o <= 1;
		    register_write_index_o <= 1; // $sp
		    next_state <= STATE_JSR1;
		    stall_o <= 1;
		  end
		`OP_JSRA:
		  begin
		    $display ("Executing OP_JSRA");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_JSR1;
		    stall_o <= 1;
		  end
		`OP_LDA_B:
		  begin
		    $display ("Executing OP_LDA_B");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDA_L: 
		  begin
		    memory_address_o <= operand_i;
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDA_S:
		  begin
		    $display ("Executing OP_LDA_S");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LD_B:
		  begin
		    $display ("Executing OP_LD_B");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDI_B:
		  begin
		    $display ("Executing OP_LDI_B");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDI_L:
		  begin
		    $display ("EXECUTE OP_LDI_L: 0x%x", operand_i);
		    reg_result_o <= operand_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDI_S:
		  begin
		    $display ("Executing OP_LDI_S");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LD_L:
		  begin
		    $display ("Executing OP_LD_L");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDO_B:
		  begin
		    $display ("Executing OP_LDO_B");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDO_L:
		  begin
		    $display ("Executing OP_LDO_L");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LDO_S:
		  begin
		    $display ("Executing OP_LDO_S");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LD_S:
		  begin
		    $display ("Executing OP_LD_S");
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_LSHR:
		  begin
		    $display ("Executing OP_LSHR");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_MOD_L:
		  begin
		    $display ("Executing OP_MOD_L");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_MOV:
		  begin
		    reg_result_o <= regB_i;
		    memory_write_enable_o <= 0;
		    memory_read_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_MUL_L:
		  begin
		    $display ("Executing OP_MUL_L");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_NEG:
		  begin
		    $display ("Executing OP_NEG");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_NOP:
		  begin
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_NOT:
		  begin
		    $display ("Executing OP_NOT");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_OR:
		  begin
		    reg_result_o <= regA_i | regB_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_POP:
		  begin
		    // Decrement pointer register by 4 bytes.
		    memory_address_o <= regA_i;
		    mem_result_o <= regA_i - 4;
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_PUSH:
		  begin
		    // Decrement pointer register by 4 bytes.
		    reg_result_o <= regA_i - 4;
		    memory_address_o <= regA_i - 4;
		    mem_result_o <= regB_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 1;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_RET:
		  begin
		    // Increment $sp by 8
		    memory_address_o <= sp_i;
		    reg_result_o <= sp_i + 8;
		    memory_read_enable_o <= 1;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= 1; // $sp
		    next_state <= STATE_RET1;
		    stall_o <= 0;
		  end
		`OP_SSR:
		  begin
		    $display ("Executing OP_SSR");
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STA_B:
		  begin
		    $display ("Executing OP_STA_B");
		    mem_result_o <= regA_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 1;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STA_L:
		  begin
		    mem_result_o <= regA_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 1;
		    memory_address_o <= operand_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STA_S:
		  begin
		    $display ("Executing OP_STA_S");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_ST_B:
		  begin
		    $display ("Executing OP_ST_B");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_ST_L:
		  begin
		    $display ("Executing OP_ST_L");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STO_B:
		  begin
		    $display ("Executing OP_STO_B");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STO_L:
		  begin
		    $display ("Executing OP_STO_L");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_STO_S:
		  begin
		    $display ("Executing OP_STO_S");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_ST_S:
		  begin
		    $display ("Executing OP_ST_S");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_SUB_L:
		  begin
		    reg_result_o <= regA_i - regB_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_SWI:
		  begin
		    $display ("Executing OP_SWI");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_UDIV_L:
		  begin
		    $display ("Executing OP_UDIV_L");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_UMOD_L:
		  begin
		    $display ("Executing OP_UMOD_L");
		    next_state <= STATE_READY;
		    stall_o <= 0;
		  end
		`OP_XOR:
		  begin
		    reg_result_o <= regA_i ^ regB_i;
		    memory_read_enable_o <= 0;
		    memory_write_enable_o <= 0;
		    register_write_enable_o <= 1;
		    register_write_index_o <= register_write_index_i;
		    stall_o <= 0;
		  end
	      endcase // case (op_i)
	    end // case: STATE_READY
	  STATE_JSR1:
	    begin
	      // Decrement $sp by 4 bytes.
	      reg_result_o <= sp_i - 4;
	      memory_address_o <= sp_i - 4;
	      mem_result_o <= fp_i;
	      memory_read_enable_o <= 0;
	      memory_write_enable_o <= 1;
	      register_write_enable_o <= 1;
	      register_write_index_o <= 1; // $sp
	      branch_target_o <= operand_i;
	      next_state <= STATE_READY;
	      stall_o <= 0;
	    end
	  STATE_RET1:
	    begin
	      // Increment $sp by 4 bytes.
	      reg_result_o <= sp_i + 4;
	      memory_address_o <= sp_i + 4;
	      memory_read_enable_o <= 1;
	      memory_write_enable_o <= 0;
	      register_write_enable_o <= 1;
	      // This is all wrong
	      register_write_index_o <= 1; // $sp
	      branch_target_o <= operand_i;
	      next_state <= STATE_READY;
	      stall_o <= 0;
	    end
	endcase
      end
endmodule // cpu_execute;
