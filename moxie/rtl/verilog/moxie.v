// moxie.v - Top level Moxie Core
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

module moxie (/*AUTOARG*/
  // Outputs
  wb_dat_o, wb_adr_o, wb_cyc_o, wb_stb_o,
  // Inputs
  rst_i, clk_i, wb_dat_i, wb_sel_i, wb_we_i, wb_ack_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  reg 	 rst;

  // --- Wishbone Interconnect ------------------------------------
  input [31:0]  wb_dat_i;
  output [31:0] wb_dat_o;
  output [31:0]  wb_adr_o;
  input [1:0]   wb_sel_i;
  input         wb_we_i;
  output        wb_cyc_o;
  output        wb_stb_o;
  input         wb_ack_i;

  /*AUTOREG*/
  // Beginning of automatic regs (for this module's undeclared outputs)
  reg [31:0]		wb_dat_o;
  reg			wb_stb_o;
  // End of automatics

  // --- Wires to connect the 5 pipeline stages -------------------
  //
  //  Prefix codes for the control signals
  //        fd - Fetch to Decode
  //        dx - Decode to Execute
  //        rx - Register File to Execute
  //        xr - Execute to Register File

  wire [15:0] fd_opcode;
  wire [31:0] fd_operand;
  wire [0:0]  fd_valid;
  wire [31:0] dx_operand;
  wire [0:0]  dx_register_write_enable;
  wire [5:0]  dx_op;
  wire [0:0]  xw_register_write_enable;
  wire [0:0]  wr_register_write_enable;
  wire [3:0]  dx_register_write_index;
  wire [3:0]  xw_register_write_index;
  wire [31:0] xw_result;
  wire [3:0]  wr_register_write_index;
  wire [31:0] wr_result;
  wire [3:0]  dx_regA;
  wire [3:0]  dx_regB;
  wire [3:0]  dx_regC;

  wire [0:0]  xf_branch_flag;
  wire [31:0] xf_branch_target;
 
  wire [31:0] rx_reg_value1;
  wire [31:0] rx_reg_value2;
  wire [3:0]  dx_reg_index1;
  wire [3:0]  dx_reg_index2;
  wire [3:0]  xr_reg_index1;
  wire [3:0]  xr_reg_index2;

  wire [0:0] hazard_war;

  // synthesis translate_off 
  initial
    begin
      $dumpvars(1,stage_fetch); 
      $dumpvars(1,stage_decode); 
      $dumpvars(1,stage_execute); 
      $dumpvars(1,stage_write);
      $dumpvars(1,regs);
      $display("-- BEGINNING --");
    end
  // synthesis translate_on 

  cpu_registerfile regs (// Outputs
			 .value1_o (rx_reg_value1), 
			 .value2_o (rx_reg_value2),
			 // Inputs
			 .rst_i			(rst_i),
			 .clk_i			(clk_i),
			 .write_enable_i (wr_register_write_enable), 
			 .reg_write_index_i (wr_register_write_index),
			 .reg_read_index1_i (xr_reg_index1), 
			 .reg_read_index2_i (xr_reg_index2), 
			 .value_i (wr_result));

  always @(posedge clk_i)
    if (rst_i) begin
      /* AUTORESET */
      // Beginning of autoreset for uninitialized flops
      wb_stb_o <= 1'h0;
      // End of automatics
    end else begin
      wb_stb_o <= #1 (wb_stb_o & !wb_ack_i) | (!wb_stb_o);
    end

  assign wb_cyc_o = wb_stb_o;
  
  cpu_fetch stage_fetch (// Outputs
			 .opcode		(fd_opcode[15:0]),
			 .valid		(fd_valid),
			 .operand		(fd_operand[31:0]),
			 .imem_address_o        (wb_adr_o[31:0]),
			 // Inputs
			 .rst_i			(rst_i),
			 .clk_i			(clk_i),
			 .branch_flag_i (xf_branch_flag),
			 .branch_target_i (xf_branch_target),
			 .stall_i               (hazard_war),
			 .imem_data_i           (wb_dat_i[31:0]));
    
  cpu_decode stage_decode (// Inputs
			   .rst_i			(rst_i),
			   .clk_i			(clk_i),
			   .opcode_i		(fd_opcode[15:0]),
			   .operand_i		(fd_operand[31:0]),
			   .valid_i		(fd_valid),
			   .stall_i             (hazard_war),
			   // Outputs
			   .register_write_enable_o (dx_register_write_enable),
			   .register_write_index_o (dx_register_write_index),
			   .operand_o (dx_operand),
			   .riA_o (dx_reg_index1),
			   .riB_o (dx_reg_index2),
			   .op_o (dx_op));
    
  cpu_execute stage_execute (// Inputs
			     .rst_i			(rst_i),
			     .clk_i			(clk_i),
			     .stall_i        (hazard_war),
			     .op_i           (dx_op),
			     .operand_i		(dx_operand[31:0]),
			     .riA_i (dx_reg_index1),
			     .riB_i (dx_reg_index2),
			     .riA_o (xr_reg_index1),
			     .riB_o (xr_reg_index2),
			     .regA_i (rx_reg_value1),
			     .regB_i (rx_reg_value2),
			     .branch_flag_o (xf_branch_flag),
			     .branch_target_o (xf_branch_target),
			     .register_write_index_i (dx_register_write_index),
			     // Outputs
			     .register_write_enable_o (xw_register_write_enable),
			     .register_write_index_o (xw_register_write_index),
			     .result_o (xw_result));

  cpu_write stage_write (  // Inputs
			   .rst_i (rst_i),
			   .clk_i (clk_i),
			   .register_write_index_i (xw_register_write_index),
			   .register_write_enable_i (xw_register_write_enable),
			   .result_i (xw_result),
			   // Outputs
			   .register_write_index_o (wr_register_write_index),
			   .register_write_enable_o (wr_register_write_enable),
			   .result_o (wr_result));

  assign hazard_war = 0;

  // (dr_A_read_enable & (xr_register_write_enable & (dr_reg_index1 == xr_register_write_index))) |  (dr_B_read_enable & (xr_register_write_enable & (dr_reg_index2 == xr_register_write_index)));
    
  always @ (posedge clk_i) begin
    if (!rst_i & hazard_war)
      begin

      end
  end

endmodule // moxie
