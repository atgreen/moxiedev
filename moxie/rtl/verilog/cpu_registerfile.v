// cpu_registerfile.v - moxie register file
//
// Copyright (c) 2010  Anthony Green.  All Rights Reserved.
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
  rst_i, clk_i, write_enable_i, read_enable_i, reg_write_index_i,
  reg_read_index1_i, reg_read_index2_i, value_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  input [0:0] write_enable_i;
  input [0:0] read_enable_i;
  input [3:0] reg_write_index_i;
  input [3:0] reg_read_index1_i;
  input [3:0] reg_read_index2_i;
  input [31:0] value_i;
  output [31:0] value1_o;
  output [31:0] value2_o;
  
  reg [31:0] value1_o;
  reg [31:0] value2_o;

  reg [31:0] register_fp;
  reg [31:0] register_sp;
  reg [31:0] register_r0;
  reg [31:0] register_r1;
  reg [31:0] register_r2;
  reg [31:0] register_r3;
  reg [31:0] register_r4;
  reg [31:0] register_r5;
  reg [31:0] register_r6;
  reg [31:0] register_r7;
  reg [31:0] register_r8;
  reg [31:0] register_r9;
  reg [31:0] register_r10;
  reg [31:0] register_r11;
  reg [31:0] register_r12;
  reg [31:0] register_r13;

  
  // always @ (posedge rst_i) begin
  //   $display ("REGRESET");
  //   register_fp <= 32'b00000000000000000000000000000000;
  //   register_sp <= 32'b00000000000000000000000000000000;
  //   register_r0 <= 32'b00000000000000000000000000000000;
  //   register_r1 <= 32'b00000000000000000000000000000000;
  //   register_r2 <= 32'b00000000000000000000000000000000;
  //   register_r3 <= 32'b00000000000000000000000000000000;
  //   register_r4 <= 32'b00000000000000000000000000000000;
  //   register_r5 <= 32'b00000000000000000000000000000000;
  //   register_r6 <= 32'b00000000000000000000000000000000;
  //   register_r7 <= 32'b00000000000000000000000000000000;
  //   register_r8 <= 32'b00000000000000000000000000000000;
  //   register_r9 <= 32'b00000000000000000000000000000000;
  //   register_r10 <= 32'b00000000000000000000000000000000;
  //   register_r11 <= 32'b00000000000000000000000000000000;
  //   register_r12 <= 32'b00000000000000000000000000000000;
  //   register_r13 <= 32'b00000000000000000000000000000000;
  // end

  always @ (posedge clk_i) begin
    if (write_enable_i)
    if (!rst_i) begin
      $display("WRITING 0x%x to register 0x%x", value_i, reg_write_index_i);
      case (reg_write_index_i)
	4'b0000:
	  register_fp <= value_i;
	4'b0001:
	  register_sp <= value_i;
	4'b0010:
	  register_r0 <= value_i;
	4'b0011:
	  register_r1 <= value_i;
	4'b0100:
	  register_r2 <= value_i;
	4'b0101:
	  register_r3 <= value_i;
	4'b0110:
	  register_r4 <= value_i;
	4'b0111:
	  register_r5 <= value_i;
	4'b1000:
	  register_r6 <= value_i;
	4'b1001:
	  register_r7 <= value_i;
	4'b1010:
	  register_r8 <= value_i;
	4'b1011:
	  register_r9 <= value_i;
	4'b1100:
	  register_r10 <= value_i;
	4'b1101:
	  register_r11 <= value_i;
	4'b1110:
	  register_r12 <= value_i;
	4'b1111:
	  register_r13 <= value_i;
      endcase
    end
  end

  always @ (posedge read_enable_i) begin
    if (!rst_i)
      begin
	case (reg_read_index1_i)
	  4'b0000:
	    value1_o <= register_fp;
	  4'b0001:
	    value1_o <= register_sp;
	  4'b0010:
	    value1_o <= register_r0;
	  4'b0011:
	    value1_o <= register_r1;
	  4'b0100:
	    value1_o <= register_r2;
	  4'b0101:
	    value1_o <= register_r3;
	  4'b0110:
	    value1_o <= register_r4;
	  4'b0111:
	    value1_o <= register_r5;
	  4'b1000:
	    value1_o <= register_r6;
	  4'b1001:
	    value1_o <= register_r7;
	  4'b1010:
	    value1_o <= register_r8;
	  4'b1011:
	    value1_o <= register_r9;
	  4'b1100:
	    value1_o <= register_r10;
	  4'b1101:
	    value1_o <= register_r11;
	  4'b1110:
	    value1_o <= register_r12;
	  4'b1111:
	    value1_o <= register_r13;
	endcase
	case (reg_read_index2_i)
	  4'b0000:
	    value2_o <= register_fp;
	  4'b0001:
	    value2_o <= register_sp;
	  4'b0010:
	    value2_o <= register_r0;
	  4'b0011:
	    value2_o <= register_r1;
	  4'b0100:
	    value2_o <= register_r2;
	  4'b0101:
	    value2_o <= register_r3;
	  4'b0110:
	    value2_o <= register_r4;
	  4'b0111:
	    value2_o <= register_r5;
	  4'b1000:
	    value2_o <= register_r6;
	  4'b1001:
	    value2_o <= register_r7;
	  4'b1010:
	    value2_o <= register_r8;
	  4'b1011:
	    value2_o <= register_r9;
	  4'b1100:
	    value2_o <= register_r10;
	  4'b1101:
	    value2_o <= register_r11;
	  4'b1110:
	    value2_o <= register_r12;
	  4'b1111:
	    value2_o <= register_r13;
	endcase
	#1 $display("READING 0x%x from register 0x%x", value1_o, reg_read_index1_i);
	#1 $display("READING 0x%x from register 0x%x", value2_o, reg_read_index2_i);
      end
  end

endmodule // cpu_registerfile
