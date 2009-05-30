// test module
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

module test;

   reg reset = 0;
   initial begin
      $dumpfile("test.vcd");
      $dumpvars(1,mc);
      # 10 reset = 1;
      # 20 reset = 0;
      # 100 $stop;
   end

   reg clk = 0;

   always #5 clk = !clk;

   MoxieCore mc (reset, clk);

endmodule
   
 