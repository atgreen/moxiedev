// cpu_decode.v - The instruction decode unit
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

module cpu_decode (/*AUTOARG*/
  // Outputs
  register_write_enable_o, register_write_index_o,
  register_read_enable_o, operand_o, regA_o, regB_o, op_ldi_o,
  op_dec_o, op_nop_o,
  // Inputs
  rst_i, clk_i, stall_i, opcode_i, operand_i, valid_i
  );

  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;

  // --- Pipeline interlock ---------------------------------------
  input stall_i;
  
  // --- Instructions ---------------------------------------------
  input [15:0] opcode_i;	
  input [31:0] operand_i;	
  input valid_i;
  
  // --- Outputs --------------------------------------------------
  output [0:0] register_write_enable_o;
  output [3:0] register_write_index_o;
  output [0:0] register_read_enable_o;
  output [31:0] operand_o;
  output [3:0] 	regA_o;
  output [3:0] 	regB_o;
  output [0:0] 	op_ldi_o;
  output [0:0] 	op_dec_o;
  output [0:0] 	op_nop_o;
    
  reg [31:0] 	operand_o;
  reg [0:0] 	op_ldi_o;
  reg [0:0] 	op_dec_o;
  reg [0:0] 	op_nop_o;
  reg [0:0] 	register_write_enable_o;
  reg [3:0] 	register_write_index_o;

  assign op_ldi = valid_i & opcode_i[15:8] == 8'b00000001;
  assign op_dec = valid_i & opcode_i[15:12] == 4'b1001;
  assign op_nop = (!valid_i) | opcode_i[15:8] == 8'b00000000;
  assign register_read_enable_o = op_dec;
  assign regA_o = (opcode_i[15:12] == 4'b1001 ? opcode_i[11:8] : 0);
  
  always @(posedge clk_i)
    if (! rst_i && ! stall_i)
      begin
	fork
	  operand_o <= (op_dec ? opcode_i[7:0] : operand_i);
	  register_write_index_o <= opcode_i[7:4];
	join
	fork
	  op_ldi_o <= op_ldi;
	  op_dec_o <= op_dec;
	  op_nop_o <= op_nop;
	join
	register_write_enable_o <= (!op_nop) & (op_ldi|op_dec);
      end
    else begin
      $display ("STALL");
      register_write_enable_o <= 0;
    end

endmodule // cpu_decode;
