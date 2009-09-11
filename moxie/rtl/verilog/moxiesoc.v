// moxiesoc.v - Top level Moxie SoC module.
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

module moxiesoc (/*AUTOARG*/
   // Inputs
   rst_i, clk_i
   );
   
   // --- Clock and Reset ------------------------------------------
   input  rst_i, clk_i;

   reg rst;

   wire idif_used_insn_o;
   wire idif_used_data_o;

   wire [15:0] ifid_insn_i;
   wire [31:0] ifid_insn_data_i;
   wire [0:0] ifid_insn_valid_i;
   wire [0:0] ifid_insn_data_valid_i;

   cpu_ifetch cif (ifid_insn_i, ifid_insn_data_i, ifid_insn_valid_i,
		   ifid_insn_data_valid_i, rst_i, clk_i, idif_used_insn_o,
		   idif_used_data_o);

   cpu_idecode cid (idif_used_insn_o, idif_used_data_o,
		    rst_i, clk_i, ifid_insn_i,
		    ifid_insn_data_i, ifid_insn_valid_i, ifid_insn_data_valid_i);
   
   always @ (posedge clk_i) begin
      
      if (rst_i == 1) begin
	 
	 $display("** RESET **");

      end
	
   end
	 
endmodule // moxiesoc

