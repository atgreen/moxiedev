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

  // All _i _o suffixes are relative to the moxie core.
  wire [31:0] moxie_dat_i;
  wire [31:0] moxie_dat_o;
  wire [31:0] moxie_adr_o;
  wire [1:0]  moxie_sel_i;
  wire 	      moxie_we_i;
  wire 	      moxie_cyc_i;
  wire        moxie_stb_i;
  wire 	      moxie_ack_o;
   
  // All _i _o suffixes are relative to the bootrom core.
  wire [31:0] bootrom_dat_i;
  wire [31:0] bootrom_dat_o;
  wire [31:0] bootrom_adr_o;
  wire [1:0]  bootrom_sel_i;
  wire 	      bootrom_we_i;
  wire 	      bootrom_cyc_i;
  wire        bootrom_stb_i;
  wire 	      bootrom_ack_o;

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

  intercon (.wbm_dat_o (moxie_dat_o),
	    .wbm_adr_i (moxie_adr_o),
	    .wbm_sel_i (moxie_sel_i),
	    .wbm_we_i (moxie_we_i),
	    .wbm_cyc_i (moxie_cyc_i),
	    .wbm_stb_i (moxie_stb_i),
	    .wbm_ack_o (moxie_ack_o),
            .wbs_0_dat_o (bootrom_dat_o),
	    .wbs_0_adr_i (bootrom_adr_o),
	    .wbs_0_sel_o (bootrom_sel_i),
	    .wbs_0_we_o (bootrom_we_i),
	    .wbs_0_cyc_o (bootrom_cyc_i),
	    .wbs_0_stb_o (bootrom_stb_i),
	    .wbs_0_ack_i (bootrom_ack_o));
  
  bootrom rom (.wb_dat_i (bootrom_dat_i),
	       .wb_dat_o (bootrom_dat_o),
	       .wb_adr_i (bootrom_adr_o),
	       .wb_sel_i (bootrom_sel_i),
	       .wb_we_i (bootrom_we_i),
	       .wb_cyc_i (bootrom_cyc_i),
	       .wb_stb_i (bootrom_stb_i),
	       .wb_ack_o (bootrom_ack_o));
  
  moxie core (.rst_i (rst_i),
	      .clk_i (clk_i),
	      .wb_dat_i (moxie_dat_i),
	      .wb_dat_o (moxie_dat_o),
	      .wb_adr_o (moxie_adr_o),
	      .wb_sel_i (moxie_sel_i),
	      .wb_we_i (moxie_we_i),
	      .wb_cyc_i (moxie_cyc_i),
	      .wb_stb_i (moxie_stb_i),
	      .wb_ack_o (moxie_ack_o));

endmodule // muskoka
