// muskoka.v - Top level Muskoka SoC module.
//
// Copyright (c) 2009, 2010  Anthony Green.  All Rights Reserved.
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

module muskoka (/*AUTOARG*/
  // Inputs
  rst_i, clk_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  reg 	 rst;

  /*AUTOWIRE*/
  wire 	 [15:0] opcode;
  wire   [31:0] operand;
  wire   [0:0] valid;
  
  cpu_fetch s1 (// Outputs
		.opcode		(opcode[15:0]),
		.valid		(valid),
		.operand		(operand[31:0]),
		// Inputs
		.rst_i			(rst_i),
		.clk_i			(clk_i));
  
  cpu_decode s2 (// Inputs
		 .rst_i			(rst_i),
		 .clk_i			(clk_i),
		 .opcode_i		(opcode[15:0]),
		 .operand_i		(operand[31:0]),
		 .valid_i		(valid));
       
   always @ (posedge clk_i) begin
      
      if (rst_i == 1) begin
	 
	 $display("** RESET **");

      end
	
   end
	 
endmodule // muskoka
