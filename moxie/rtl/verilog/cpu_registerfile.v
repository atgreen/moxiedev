// cpu_registerfile.v - moxie register file
//
// Copyright (c) 2010, 2011  Anthony Green.  All Rights Reserved.
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

module cpu_registerfile (/*AUTOARG*/
  // Outputs
  value1_o, value2_o,
  // Inputs
  rst_i, clk_i, write_enable_i, reg_write_index_i,
  reg_read_index1_i, reg_read_index2_i, value_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  input [0:0] write_enable_i;
  input [3:0] reg_write_index_i;
  input [3:0] reg_read_index1_i;
  input [3:0] reg_read_index2_i;
  input [31:0] value_i;
  output [31:0] value1_o;
  output [31:0] value2_o;
  
  wire [31:0] value1_o = registers[reg_read_index1_i];
  wire [31:0] value2_o = registers[reg_read_index2_i];

  reg [31:0] registers[0:15];

  always @ (posedge rst_i or posedge clk_i) begin
    if (rst_i) begin
      registers[0] <= 32'b00000000000000000000000000000000;
      registers[1] <= 32'b00000000000000000000000000000000;
      registers[2] <= 32'b00000000000000000000000000000000;
      registers[3] <= 32'b00000000000000000000000000000000;
      registers[4] <= 32'b00000000000000000000000000000000;
      registers[5] <= 32'b00000000000000000000000000000000;
      registers[6] <= 32'b00000000000000000000000000000000;
      registers[7] <= 32'b00000000000000000000000000000000;
      registers[8] <= 32'b00000000000000000000000000000000;
      registers[9] <= 32'b00000000000000000000000000000000;
      registers[10] <= 32'b00000000000000000000000000000000;
      registers[11] <= 32'b00000000000000000000000000000000;
      registers[12] <= 32'b00000000000000000000000000000000;
      registers[13] <= 32'b00000000000000000000000000000000;
    end else
      if (write_enable_i)
        registers[reg_write_index_i] <= value_i;
  end

endmodule // cpu_registerfile
