// cpu_fetch.v - The instruction fetch unit
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

// Set the boot address upon system reset
`define BOOT_ADDRESS 32'h00001000 

module cpu_fetch (/*AUTOARG*/
  // Outputs
  imem_address_o, opcode, valid, operand,
  // Inputs
  rst_i, clk_i, imem_data_i, stall_i
  );
  
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  
  // --- Instruction memory ---------------------------------------
  output [31:0] imem_address_o;
  input  [31:0] imem_data_i;

  // --- Pipeline interlock ---------------------------------------
  input stall_i;
  
  // --- Instruction Buffer ---------------------------------------
  output [15:0] opcode;
  output [0:0] 	valid;
  output [31:0] operand;
 
  // --- Test memory.  Let's just read from an internal array.  */
  //  reg [7:0] 	MEM [0:128000];
  reg [7:0] 	MEM [0:16675];
  reg [31:0] 	PC; /* For testing only.  */

  wire [0:0] 	valid, empty, full;
  reg 		wren, rden;
  reg  [31:0] 	wrdata;
  wire [15:0] opcode;
  wire [31:0] operand;

  assign imem_address_o = PC;
  
  // The instruction FIFO
  cpu_ififo ififo (
		   // Outputs
		   .opcode_o		(opcode[15:0]),
		   .operand_o		(operand[31:0]),
		   .valid_o		(valid),
		   .empty_o		(empty),
		   .full_o		(full),
		   // Inputs
		   .rst_i		(rst_i),
		   .clk_i		(clk_i),
		   .write_en_i		(wren),
		   .read_en_i		(rden),
		   .data_i	(wrdata[31:0]));
  
  // synthesis translate_off 
  initial
    begin
      $readmemh("hello.vh", MEM);
      $dumpvars(1,ififo); 
    end
  // synthesis translate_on

  // --- Test fetch -----------------------------------------------
  always @(posedge clk_i) begin
    if (rst_i) begin
       PC <= #1 `BOOT_ADDRESS;
    end else begin 
       if (! stall_i) begin
	  if (! full) begin
	     wren <= 1;
//	     wrdata <= { MEM[PC], MEM[PC+1], MEM[PC+2], MEM[PC+3] };
	     wrdata <= imem_data_i;
	     PC <= PC+4;
	  end else begin
	     wren <= 0;
	  end
	  rden <= 1;
       end else begin
	  wren <= 0;
	  rden <= 0;
       end      
    end // else: !if(rst_i)
  end

endmodule // cpu_fetch
