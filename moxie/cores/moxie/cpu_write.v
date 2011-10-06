// cpu_write.v - The writeback unit
//
// Copyright (c) 2011 Anthony Green.  All Rights Reserved.
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

module cpu_write (/*AUTOARG*/
  // Outputs
  register_write_index_o, register_write_enable_o, result_o,
  memory_write_enable_o, memory_write_address_o, memory_write_value_o,
  // Inputs
  rst_i, clk_i, register_write_index_i, register_write_enable_i,
  memory_write_enable_i, memory_write_address_i, result_i
  );
  
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  input [3:0] register_write_index_i;
  input [0:0] register_write_enable_i;
  input [0:0] memory_write_enable_i;
  input [31:0] memory_write_address_i;
  input [31:0] result_i;

  output [3:0] register_write_index_o;
  output [0:0] register_write_enable_o;
  output [31:0] result_o;
  
  wire [3:0] register_write_index_o = register_write_index_i;
  wire [0:0] register_write_enable_o = register_write_enable_i;
  wire [31:0] result_o = result_i;

  output [0:0]   memory_write_enable_o;
  output [31:0]  memory_write_address_o;
  output [31:0]  memory_write_value_o;

  reg [0:0]   memory_write_enable_o;
  reg [31:0]  memory_write_address_o;
  reg [31:0]  memory_write_value_o;

  always @(posedge clk_i) begin
    if (! rst_i)
      begin
	memory_write_enable_o <= memory_write_enable_i;
	if (memory_write_enable_i)
	  begin
	    memory_write_value_o <= result_i;
	    memory_write_address_o <= memory_write_address_i;
	  end
      end
  end

endmodule // cpu_write
