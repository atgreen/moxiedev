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

  // Moxie/Wishbone interface
  wire [31:0] wb2mx_dat;
  wire [31:0] mx2wb_dat;
  wire [31:0] mx2wb_adr;
  wire [1:0]  wb2mx_sel;
  wire 	      wb2mx_we;
  wire 	      mx2wb_cyc;
  wire        mx2wb_stb;
  wire 	      wb2mx_ack;
   
  // Bootrom/Wishbone interface
  wire [31:0] wb2br_dat;
  wire [31:0] br2wb_dat;
  wire [31:0] wb2br_adr;
  wire [1:0]  wb2br_sel;
  wire 	      wb2br_we;
  wire 	      wb2br_cyc;
  wire        wb2br_stb;
  wire 	      br2wb_ack;

  // synthesis translate_off 
  initial
    begin
      $dumpvars(1,intercon);
      $dumpvars(1,rom);
    end

  // slave 0 - bootrom @ 0x1000 for 512 bytes
  // slave 1 - unused
  // slave 2 - unused
  // slave 3 - unused
  
  wb_intercon #(.slave_0_mask (32'b1111_1111_1111_1111_1111_0000_0000_0000),
	        .slave_0_addr (32'b0000_0000_0000_0000_0001_0000_0000_0000),
		.slave_1_mask (32'b0000_0000_0000_0000_0000_0000_0000_0000),
	        .slave_1_addr (32'b1111_1111_1111_1111_1111_1111_1111_1111),
		.slave_2_mask (32'b0000_0000_0000_0000_0000_0000_0000_0000),
	        .slave_2_addr (32'b1111_1111_1111_1111_1111_1111_1111_1111),
		.slave_3_mask (32'b0000_0000_0000_0000_0000_0000_0000_0000),
	        .slave_3_addr (32'b1111_1111_1111_1111_1111_1111_1111_1111))

  intercon (.wbm_dat_o (wb2mx_dat),
	    .wbm_adr_i (mx2wb_adr),
	    .wbm_sel_i (wb2mx_sel),
	    .wbm_we_i (wb2mx_we),
	    .wbm_cyc_i (mx2wb_cyc),
	    .wbm_stb_i (mx2wb_stb),
	    .wbm_ack_o (wb2mx_ack),
            .wbs_0_dat_o (br2wb_dat),
	    .wbs_0_adr_i (wb2br_adr),
	    .wbs_0_sel_o (wb2br_sel),
	    .wbs_0_we_o (wb2br_we),
	    .wbs_0_cyc_o (wb2br_cyc),
	    .wbs_0_stb_o (wb2br_stb),
	    .wbs_0_ack_i (bootrom_ack_o));
  
  bootrom rom (.wb_dat_i (wb2br_dat),
	       .wb_dat_o (br2wb_dat),
	       .wb_adr_i (wb2br_adr),
	       .wb_sel_i (wb2br_sel),
	       .wb_we_i (wb2br_we),
	       .wb_cyc_i (wb2br_cyc),
	       .wb_stb_i (wb2br_stb),
	       .wb_ack_o (br2wb_ack));
  
  moxie core (.rst_i (rst_i),
	      .clk_i (clk_i),
	      .wb_dat_i (wb2mx_dat),
	      .wb_dat_o (mx2wb_dat),
	      .wb_adr_o (mx2wb_adr),
	      .wb_sel_i (wb2mx_sel),
	      .wb_we_i (wb2mx_we),
	      .wb_cyc_o (mx2wb_cyc),
	      .wb_stb_o (mx2wb_stb),
	      .wb_ack_i (wb2mx_ack));

endmodule // muskoka
