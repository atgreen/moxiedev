// bootrom.v - ROM module
//
// Copyright (c) 2009, 2010, 2011  Anthony Green.  All Rights Reserved.
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

module rom (/*AUTOARG*/
    // Wishbone slave interface
    input  [31:0] wb_dat_i,
    output [31:0] wb_dat_o,
    input  [31:0] wb_adr_i,
    input         wb_we_i,
    input         wb_tga_i,
    input         wb_stb_i,
    input         wb_cyc_i,
    input  [ 1:0] wb_sel_i,
    output        wb_ack_o
  );
<
  // Net declarations
  reg  [15:0] rom[0:127];  // Instantiate the ROM

  wire [ 6:0] rom_addr;
  wire        stb;

  // Combinatorial logic
  assign rom_addr = wb_adr_i[7:1];
  assign stb      = wb_stb_i & wb_cyc_i;
  assign wb_ack_o = stb;
  assign wb_dat_o = rom[rom_addr];

  initial $readmemh("bootrom.dat", rom);

endmodule
