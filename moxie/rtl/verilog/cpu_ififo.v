// cpu_ififo.v - The instruction FIFO unit
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

module cpu_ififo (/*AUTOARG*/
  // Outputs
  opcode_o, operand_o, valid_o, empty_o, full_o,
  // Inputs
  rst_i, clk_i, write_en_i, read_en_i, data_i
  );
  
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;

  input write_en_i ; // assert high to write data
  input read_en_i ; // assert high to read data

  input [31:0] data_i ; // data to write
  
  output [15:0] opcode_o; // opcode_o for newly read instruction
  output [31:0] operand_o; // data operand_o for newly read instruction
  output 	valid_o;
  output 	empty_o ;
  output 	full_o ;
  
  reg [1:0]    read_ptr, write_ptr; // write and read pointers
  reg [2:0]    ptr_gap; // gap between the pointers
  reg [15:0]   opcode_o;
  reg [31:0]   operand_o;
  reg [0:0]    valid_o;
  reg [15:0]   buffer[3:0]; // instruction buffer

  assign can_write_32 = ((ptr_gap == 0) || (ptr_gap == 1) || (ptr_gap == 2));
  assign can_read_16 = (ptr_gap != 0);
  assign can_read_48 =  ((ptr_gap != 0) && (ptr_gap != 1) && (ptr_gap != 2));
  assign buffer_full = ptr_gap == 4;
  assign buffer_empty = ptr_gap == 0;

  assign full_o = ! (can_write_32);
  
  assign empty_o = buffer_empty;
    
  // This is a rediculous bit of logic.  We should try to recode the
  // moxie instructions so that we can determine the length with less
  // logic.
  function [0:0] is_long_insn;
    input [7:0] op;
    is_long_insn = ((op == 8'b00001111)    // beq
		    || (op == 8'b00010101) // bge
		    || (op == 8'b00010111) // bgeu
		    || (op == 8'b00010010) // bgt
		    || (op == 8'b00010100) // bgtu
		    || (op == 8'b00010110) // ble
		    || (op == 8'b00011000) // bleu
		    || (op == 8'b00010001) // blt
		    || (op == 8'b00010011) // bltu
		    || (op == 8'b00010000) // bne
		    || (op == 8'b00011010) // jmpa
		    || (op == 8'b00000011) // jsra
		    || (op == 8'b00011101) // lda.b
		    || (op == 8'b00001000) // lda.l
		    || (op == 8'b00010011) // lda.s
		    || (op == 8'b00000001) // ldi.l
		    || (op == 8'b00011011) // ldi.b
		    || (op == 8'b00100000) // ldi.s
		    || (op == 8'b00110110) // ldo.b
		    || (op == 8'b00001100) // ldo.l
		    || (op == 8'b00111000) // ldo.s
		    || (op == 8'b00001001) // sta.b
		    || (op == 8'b00100100) // sta.s
		    || (op == 8'b00110111) // sto.b
		    || (op == 8'b00001101) // sto.l
		    || (op == 8'b00111001)); // sto.s
  endfunction

  always @(posedge clk_i or posedge rst_i)
    if (rst_i == 1) begin
      opcode_o <= 0;
      operand_o <= 0;
      read_ptr <= 0;
      write_ptr <= 0;
      ptr_gap <= 0;
    end else begin
      $display ("A %x buffer[read_ptr] = 0x%x", read_ptr, buffer[read_ptr][15:8]);
      $display ("A BUFFER = 0x%x%x%x%x", buffer[0], buffer[1], buffer[2], buffer[3]);
      if (!is_long_insn(buffer[read_ptr][15:8])) begin
	$display ("B");
	if (write_en_i && (!read_en_i) && (can_write_32)) begin
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  ptr_gap <= ptr_gap + 2;
	end
	else if ((!write_en_i) && read_en_i && (can_read_16)) begin
	  opcode_o <= buffer[read_ptr];
	  valid_o <= 1;
	  read_ptr <= read_ptr + 1;
	  ptr_gap <= ptr_gap - 1;
	end
	else if (write_en_i && read_en_i && buffer_empty) begin
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  valid_o <= 0;
	end
	else if (write_en_i && read_en_i && buffer_full) begin
	  opcode_o <= buffer[read_ptr];
	  valid_o <= 1;
	  read_ptr <= read_ptr + 1;
	  ptr_gap <= ptr_gap - 1;
	end
	else if (write_en_i && read_en_i && (can_write_32) && (can_read_16)) begin
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  opcode_o <= buffer[read_ptr];
	  valid_o <= 1;
	  ptr_gap <= ptr_gap + 1;
	end
      end else begin
	$display ("C w %x r %x w32 %x", write_en_i, read_en_i, can_write_32);
	if (write_en_i && (!read_en_i) && (can_write_32)) begin
	  $display ("D");
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  ptr_gap <= ptr_gap + 2;
	end
	else if ((!write_en_i) && read_en_i && (can_read_48)) begin
	  $display ("E");
	  opcode_o <= buffer[read_ptr];
	  operand_o[31:16] <= buffer[read_ptr+1];
	  operand_o[15:0] <= buffer[read_ptr+2];
	  valid_o <= 1;
	  read_ptr <= read_ptr + 3;
	  ptr_gap <= ptr_gap - 3;
	end
	else if (write_en_i && read_en_i && buffer_empty) begin
	  $display ("F data = 0x%x", data_i);
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  ptr_gap <= ptr_gap + 2;
	  valid_o <= 0;
	  #1 $display ("f... buffer = 0x%x%x%x%x", buffer[read_ptr], buffer[read_ptr+1], buffer[read_ptr+2], buffer[read_ptr+3]);
	end
	else if (write_en_i && read_en_i && buffer_full) begin
	  $display ("G");
	  opcode_o <= buffer[read_ptr];
	  operand_o[31:16] <= buffer[read_ptr+1];
	  operand_o[15:0] <= buffer[read_ptr+2];
	  valid_o <= 1;
	  read_ptr <= read_ptr + 3;
	  ptr_gap <= ptr_gap - 3;
	end
	else if (write_en_i && read_en_i && (can_write_32) && (can_read_48)) begin
	  $display ("H");
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  opcode_o <= buffer[read_ptr];
	  operand_o[31:16] <= buffer[read_ptr+1];
	  operand_o[15:0] <= buffer[read_ptr+2];
	  read_ptr <= read_ptr + 3;
	  valid_o <= 1;
	  ptr_gap <= ptr_gap - 1;
	end
	else if (write_en_i && read_en_i && (ptr_gap == 2)) begin
	  $display ("I");
	  buffer[(write_ptr)%4] <= data_i[31:16];
	  buffer[(write_ptr+1)%4] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  opcode_o <= buffer[read_ptr];
	  operand_o[31:16] <= buffer[(read_ptr+1)%4];
	  operand_o[15:0] <= data_i[31:16];
	  read_ptr <= read_ptr + 3;
	  valid_o <= 1;
	  ptr_gap <= 1;
	  #1 $display ("opcode = 0x%x, operand = 0x%x, valid = %x", opcode_o, operand_o, valid_o );
	  #1 $display ("i... rbuffer[0x%x] = 0x%x%x%x%x", read_ptr, buffer[read_ptr], buffer[(read_ptr+1)%4], buffer[(read_ptr+2)%4], buffer[(read_ptr+3)%4]);
	  #1 $display ("i... wbuffer[0x%x] = 0x%x%x%x%x", write_ptr, buffer[write_ptr], buffer[(write_ptr+1)%4], buffer[(write_ptr+2)%4], buffer[(write_ptr+3)%4]);
	  #1 $display ("0x%x 0x%x 0x%x 0x%x", write_ptr, write_ptr+1, write_ptr+2, write_ptr+3);
	end
	else if (write_en_i && read_en_i && (ptr_gap == 1)) begin
	  $display ("J data = 0x%x", data_i);
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  ptr_gap <= ptr_gap + 2;
	  valid_o <= 0;
	  #1 $display ("j... buffer[0x%x] = 0x%x%x%x%x", read_ptr, buffer[read_ptr], buffer[(read_ptr+1)%4], buffer[(read_ptr+2)%4], buffer[(read_ptr+3)%4]);
	end 
	else if (write_en_i && read_en_i && (ptr_gap == 3)) begin
	  $display ("K data = 0x%x", data_i);
	  buffer[write_ptr] <= data_i[31:16];
	  buffer[write_ptr+1] <= data_i[15:0];
	  write_ptr <= write_ptr + 2;
	  opcode_o <= buffer[read_ptr];
	  operand_o[31:16] <= buffer[(read_ptr+1)%4];
	  operand_o[15:0] <= buffer[(read_ptr+2)%4];
	  read_ptr <= read_ptr + 3;
	  valid_o <= 1;
	  ptr_gap <= ptr_gap - 1;
	  #1 $display ("k... buffer[0x%x] = 0x%x%x%x%x", read_ptr, buffer[read_ptr], buffer[(read_ptr+1)%4], buffer[(read_ptr+2)%4], buffer[(read_ptr+3)%4]);
	end else
	  #1 $display ("L ERROR ptr_gap = 0x%x", ptr_gap);
      end  
    end
endmodule

 
