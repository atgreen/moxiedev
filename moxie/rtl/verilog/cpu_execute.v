// cpu_execute.v - The moxie execute stage
//
// Copyright (c) 2010 Anthony Green.  All Rights Reserved.
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

module cpu_execute (/*AUTOARG*/
  // Outputs
  register_write_index_o, register_write_enable_o, result_o,
  // Inputs
  rst_i, clk_i, regA_i, regB_i, register_write_index_i, operand_i,
  op_ldi_i, op_dec_i, op_nop_i
  );

  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  // --- Instructions ---------------------------------------------
  input [31:0] regA_i;
  input [31:0] regB_i;
  input [3:0]  register_write_index_i;
  input [31:0] operand_i;
  input [0:0]  op_ldi_i;
  input [0:0]  op_dec_i;
  input [0:0]  op_nop_i;
  
  // --- Outputs --------------------------------------------------
  output [3:0] register_write_index_o;
  output [0:0] register_write_enable_o;
  output [31:0] result_o;

  reg [3:0] 	register_write_index_o;
  reg [0:0] 	register_write_enable_o;
  reg [31:0] 	result_o;

  always @(posedge clk_i)
    if (! rst_i)
      begin
	if (op_ldi_i)
	  begin
//	    $display ("EXECUTE ldi: 0x%x", operand_i);
	    result_o <= operand_i;
	    register_write_enable_o <= 1;
	    register_write_index_o <= register_write_index_i;
	  end
	if (op_dec_i)
	  begin
	    $display ("EXECUTE dec: 0x%x", operand_i);
	    result_o <= regA_i - operand_i;
	    register_write_enable_o <= 1;
	    register_write_index_o <= register_write_index_i;
	  end
	if (op_nop_i)
	  begin
//	    $display ("EXECUTE stall");
	    register_write_enable_o <= 0;
	  end
      end
  
endmodule // cpu_execute;
