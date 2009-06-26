// iprinter.v - test module for printing decoded instructions
//
// Copyright (c) 2009 Anthony Green.  All Rights Reserved.
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

module iprinter (/*AUTOARG*/
   // Outputs
   length_o,
   // Inputs
   clk_i, insn_i, data_i
   );
   
   // --- Clock and Reset ------------------------------------------
   input clk_i;

   // --- Opcode, operands and data --------------------------------
   input [15:0] insn_i;
   input [31:0] data_i;
   output length_o;
   reg [2:0] length_o;

   initial length_o = 2'b00;

   function [3*8:0] get_regname;
      input [4:0] regnum;
      case (regnum)
 	4'b0000: 
	  get_regname = "fp ";
 	4'b0001:
	  get_regname = "sp ";
        4'b0010:
	  get_regname = "r0 ";
        4'b0011:
	  get_regname = "r1 ";
        4'b0100:
	  get_regname = "r2 ";
        4'b0101:
	  get_regname = "r3 ";
        4'b0110:
	  get_regname = "r4 ";
        4'b0111:
	  get_regname = "r5 ";
        4'b1000:
	  get_regname = "r6 ";
        4'b1001:
	  get_regname = "r7 ";
        4'b1010:
	  get_regname = "r8 ";
        4'b1011:
	  get_regname = "r9 ";
        4'b1100:
	  get_regname = "r10";
        4'b1101:
	  get_regname = "r11";
        4'b1110:
	  get_regname = "r12";
        4'b1111:
	  get_regname = "r13";
      endcase // case (regnum)
   endfunction

   always @ (posedge clk_i) begin

      # 1 length_o = 2;
      
      casex (insn_i)
	16'b00000000????????:
	  $display ("\tnop");
	16'b00000001????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldi.l\t$%0s,\t0x%x", 
		       get_regname(insn_i[7:4]),
		       data_i);
	  end
	16'b00000010????????:
	  $display ("\tmov\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00000011????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tjsra\t0x%x", data_i);
	  end
	16'b00000100????????:
	  $display ("\tret");
	16'b00000101????????:
	  $display ("\tadd.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00000110????????:
	  $display ("\tpush\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00000111????????:
	  $display ("\tpop\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00001000????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tlda.l\t0x%x", data_i);
	  end
	16'b00001001????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsta.l\t0x%x", data_i);
	  end
	16'b00001010????????:
	  $display ("\tld.l\t$%0s,\t($%0s)",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00001011????????:
	  $display ("\tst.l");
	16'b00001100????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldo.l\t0x%x", data_i);
	  end
	16'b00001101????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsto.l\t0x%x", data_i);
	  end
	16'b00001110????????:
	  $display ("\tcmp\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00001111????????:
	  $display ("\tbad");
	16'b00010000????????:
	  $display ("\tbad");
	16'b00010001????????:
	  $display ("\tbad");
	16'b00010010????????:
	  $display ("\tbad");
	16'b00010011????????:
	  $display ("\tbad");
	16'b00010100????????:
	  $display ("\tbad");
	16'b00010101????????:
	  $display ("\tbad");
	16'b00010110????????:
	  $display ("\tbad");
	16'b00010111????????:
	  $display ("\tbad");
	16'b00011000????????:
	  $display ("\tbad");
	16'b00011001????????:
	  $display ("\tjsr\t$%0s",
		    get_regname(insn_i[7:4]));
	16'b00011010????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tjmpa\t0x%x", data_i);
	  end
	16'b00011011????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldi.b\t0x%x", data_i);
	  end
	16'b00011100????????:
	  $display ("\tld.b");
	16'b00011101????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tlda.b\t0x%x", data_i);
	  end
	16'b00011110????????:
	  $display ("\tst.b");
	16'b00011111????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsta.b\t0x%x", data_i);
	  end
	16'b00100000????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldi.s\t0x%x", data_i);
	  end
	16'b00100001????????:
	  $display ("\tld.s");
	16'b00100010????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tlda.s\t0x%x", data_i);
	  end
	16'b00100011????????:
	  $display ("\tst.s");
	16'b00100100????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsta.s\t0x%x", data_i);
	  end
	16'b00100101????????:
	  $display ("\tjmp");
	16'b00100110????????:
	  $display ("\tand\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00100111????????:
	  begin
	     $display ("\tlshr\t$%0s,\t%0s",
		       get_regname(insn_i[7:4]),
		       get_regname(insn_i[3:0]));
	  end
	16'b00101000????????:
	  begin
	     $display ("\tashl\t$%0s,\t%0s",
		       get_regname(insn_i[7:4]),
		       get_regname(insn_i[3:0]));
	  end
	16'b00101001????????:
	  $display ("\tsub.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00101010????????:
	  $display ("\tneg");
	16'b00101011????????:
	  $display ("\tor\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00101100????????:
	  $display ("\tnot");
	16'b00101101????????:
	  begin
	     $display ("\tashr\t$%0s,\t%0s",
		       get_regname(insn_i[7:4]),
		       get_regname(insn_i[3:0]));
	  end
	16'b00101110????????:
	  $display ("\txor\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00101111????????:
	  $display ("\tmul.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00110000????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tswi\t0x%x", data_i);
	  end
	16'b00110001????????:
	  $display ("\tdiv.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00110010????????:
	  $display ("\tudiv.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00110011????????:
	  $display ("\tmod.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00110100????????:
	  $display ("\tumod.l\t$%0s,\t%0s",
		    get_regname(insn_i[7:4]),
		    get_regname(insn_i[3:0]));
	16'b00110101????????:
	  $display ("\tbrk");
	16'b00110110????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldo.b\t0x%x", data_i);
	  end
	16'b00110111????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsto.b\t0x%x", data_i);
	  end
	16'b00111000????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tldo.s\t0x%x", data_i);
	  end
	16'b00111001????????:
	  begin
	     # 1 length_o = 6;
	     $display ("\tsto.s\t0x%x", data_i);
	  end
	16'b00111010????????:
	  $display ("\tbad");
	16'b00111011????????:
	  $display ("\tbad");
	16'b00111100????????:
	  $display ("\tbad");
	16'b00111101????????:
	  $display ("\tbad");
	16'b00111110????????:
	  $display ("\tbad");
	16'b00111111????????:
	  $display ("\tbad");
	16'b1000????????????:
	  $display ("\tinc\t$%0s,\t%d", 
		    get_regname(insn_i[11:8]),
		    insn_i[7:0]);
	16'b1001????????????:
	  $display ("\tdec\t$%0s,\t%d", 
		    get_regname(insn_i[11:8]),
		    insn_i[7:0]);
	16'b1010????????????:
	  $display ("\tgsr");
	16'b1011????????????:
	  $display ("\tssr");
	16'b110000??????????:
	  $display ("\tbeq");
	16'b110001??????????:
	  $display ("\tbne");
	16'b110010??????????:
	  $display ("\tblt");
	16'b110011??????????:
	  $display ("\tbgt");
	16'b110100??????????:
	  $display ("\tbltu");
	16'b110101??????????:
	  $display ("\tbgtu");
	16'b110110??????????:
	  $display ("\tbge");
	16'b110111??????????:
	  $display ("\tble");
	16'b111000??????????:
	  $display ("\tbgeu");
	16'b111001??????????:
	  $display ("\tbleu");
	16'b111010??????????:
	  $display ("\tbad");
	16'b111011??????????:
	  $display ("\tbad");
	16'b111100??????????:
	  $display ("\tbad");
	16'b111101??????????:
	  $display ("\tbad");
	16'b111110??????????:
	  $display ("\tbad");
	16'b111111??????????:
	  $display ("\tbad");
      endcase // casex (insn_i)

   end
	 
endmodule // iprinter
