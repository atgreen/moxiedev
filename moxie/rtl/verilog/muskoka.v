// muskoka.v - Top level Muskoka SoC module.
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

module muskoka (/*AUTOARG*/
  // Inputs
  rst_i, clk_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  reg 	 rst;

  wire [31:0] moxie_dat_i;
  wire [31:0] moxie_dat_o;
  wire [31:1] moxie_adr_i;
  wire [1:0]  moxie_sel_i;
  wire 	      moxie_we_i;
  wire 	      moxie_cyc_i;
  wire        moxie_stb_i;
  wire 	      moxie_ack_o;
   
  wb_intercon intercon (.wbm_dat_i (moxie_dat_i),
			.wbm_dat_o (moxie_dat_o),
			.wbm_adr_i (moxie_adr_i),
			.wbm_sel_i (moxie_sel_i),
			.wbm_we_i (moxie_we_i),
			.wbm_cyc_i (moxie_cyc_i),
			.wbm_stb_i (moxie_stb_i),
			.wbm_ack_o (moxie_ack_o));
  
  moxie core (.rst_i (rst_i),
	      .clk_i (clk_i),
	      .wb_dat_i (moxie_dat_i),
	      .wb_dat_o (moxie_dat_o),
	      .wb_adr_i (moxie_adr_i),
	      .wb_sel_i (moxie_sel_i),
	      .wb_we_i (moxie_we_i),
	      .wb_cyc_i (moxie_cyc_i),
	      .wb_stb_i (moxie_stb_i),
	      .wb_ack_o (moxie_ack_o));
  
endmodule // muskoka
