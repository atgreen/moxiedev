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

   moxie mx (rst_i, clk_i);
   
   always @ (posedge clk_i) begin
      
      if (rst_i == 1) begin
	 
	 $display("** RESET **");

      end
	
   end
	 
endmodule // moxiesoc

