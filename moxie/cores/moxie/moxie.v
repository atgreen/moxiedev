// moxie.v - Top level Moxie Core
//
// Copyright (c) 2009, 2010, 2011, 2012  Anthony Green.
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
  wb_I_dat_o, wb_I_adr_o, wb_I_we_o, wb_I_cyc_o, wb_I_stb_o,
  wb_D_dat_o, wb_D_adr_o, wb_D_we_o, wb_D_cyc_o, wb_D_stb_o,
  // Inputs
  rst_i, clk_i, wb_I_dat_i, wb_I_sel_i, wb_I_ack_i, wb_D_dat_i,
  wb_D_sel_i, wb_D_ack_i
  );
   
  // --- Clock and Reset ------------------------------------------
  input  rst_i, clk_i;
  reg 	 rst;

  // --- Wishbone Interconnect for INSTRUCTION Memory -------------
  input [31:0]  wb_I_dat_i;
  output [31:0] wb_I_dat_o;
  output [31:0] wb_I_adr_o;
  input [1:0]   wb_I_sel_i;
  output        wb_I_we_o;
  output        wb_I_cyc_o;
  output        wb_I_stb_o;
  input         wb_I_ack_i;

  // --- Wishbone Interconnect for DATA Memory --------------------
  input [31:0]  wb_D_dat_i;
  output [31:0] wb_D_dat_o;
  output [31:0] wb_D_adr_o;
  input [1:0]   wb_D_sel_i;
  output        wb_D_we_o;
  output        wb_D_cyc_o;
  output        wb_D_stb_o;
  input         wb_D_ack_i;

  // --- Wires to connect the 5 pipeline stages -------------------
  //
  //  Prefix codes for the control signals
  //        fd - Fetch to Decode
  //        dx - Decode to Execute
  //        rx - Register File to Execute
  //        xr - Execute to Register File

  wire [15:0] fd_opcode;
  wire [31:0] fd_operand;
  wire [31:0] fd_PC;
  wire [0:0]  fd_valid;
  wire [31:0] dx_operand;
  wire [31:0] dx_PC;
  wire [0:0]  dx_register_write_enable;
  wire [5:0]  dx_op;
  wire [0:0]  xw_register_write_enable;
  wire [0:0]  xw_loadp;
  wire [0:0]  xw_memory_we;
  wire [0:0]  wr_register_write_enable;
  wire [3:0]  dx_register_write_index;
  wire [3:0]  xw_register_write_index;
  wire [31:0] xw_memory_address;
  wire [31:0] xw_reg_result;
  wire [31:0] xw_mem_result;
  wire [3:0]  wr_register_write_index;
  wire [31:0] wr_reg_result;
  wire [3:0]  dx_regA;
  wire [3:0]  dx_regB;
  wire [3:0]  dx_regC;

  // Stack and frame pointers
  wire [31:0] rx_sp;
  wire [31:0] rx_fp;

  wire [0:0]  xf_branch_flag;
  wire [31:0] xf_branch_target;
 
  wire [31:0] rx_reg_value1;
  wire [31:0] rx_reg_value2;
  wire [3:0]  dx_reg_index1;
  wire [3:0]  dx_reg_index2;
  wire [3:0]  xr_reg_index1;
  wire [3:0]  xr_reg_index2;

  wire [0:0]  stall_x;

  wire [0:0] hazard_war;

  reg [0:0]  wb_I_stb_o;

  // synthesis translate_off 
  initial
    begin
      $dumpvars(1,stage_fetch); 
      $dumpvars(1,stage_fetch.ififo); 
      $dumpvars(1,stage_decode); 
      $dumpvars(1,stage_execute); 
      $dumpvars(1,stage_write);
      $dumpvars(1,stage_write.cache);
      $dumpvars(1,regs);
      $display("-- BEGINNING --");
    end
  // synthesis translate_on 

  cpu_registerfile regs (// Outputs
			 .value0_o (rx_reg_value1), 
			 .value1_o (rx_reg_value2),
			 // Inputs
			 .rst_i			(rst_i),
			 .clk_i			(clk_i),
			 .write_enable0_i (wr_register_write_enable), 
			 .write_enable1_i (0), 
			 .reg_write_index0_i (wr_register_write_index),
			 .reg_read_index0_i (xr_reg_index1), 
			 .reg_read_index1_i (xr_reg_index2),
			 .sp_o (rx_sp),
			 .fp_o (rx_fp),
			 .value0_i (wr_reg_result),
			 .value1_i (0));

  always @(posedge clk_i)
    if (rst_i) begin
      /* AUTORESET */
      // Beginning of autoreset for uninitialized flops
      wb_I_stb_o <= 1'h0;
      // End of automatics
    end else begin
      wb_I_stb_o <= (wb_I_stb_o & !wb_I_ack_i) | (!wb_I_stb_o);
    end

  assign wb_I_cyc_o = wb_I_stb_o;
  
  cpu_fetch stage_fetch (// Outputs
			 .opcode		(fd_opcode[15:0]),
			 .valid		        (fd_valid),
			 .operand		(fd_operand[31:0]),
			 .imem_address_o        (wb_I_adr_o[31:0]),
			 .PC_o                  (fd_PC[31:0]),
			 // Inputs
			 .rst_i			(rst_i),
			 .clk_i			(clk_i),
			 .branch_flag_i (xf_branch_flag),
			 .branch_target_i (xf_branch_target),
			 .stall_i               (hazard_war | stall_x),
			 .imem_data_i           (wb_I_dat_i[31:0]));
    
  cpu_decode stage_decode (// Inputs
			   .rst_i			(rst_i),
			   .clk_i			(clk_i),
			   .opcode_i		(fd_opcode[15:0]),
			   .operand_i		(fd_operand[31:0]),
			   .PC_i                (fd_PC[31:0]),
			   .valid_i		(fd_valid),
			   .stall_i             (hazard_war | stall_x),
			   // Outputs
			   .register_write_enable_o (dx_register_write_enable),
			   .register_write_index_o (dx_register_write_index),
			   .operand_o (dx_operand),
			   .PC_o (dx_PC),
			   .riA_o (dx_reg_index1),
			   .riB_o (dx_reg_index2),
			   .op_o (dx_op));
    
  cpu_execute stage_execute (// Inputs
			     .rst_i			(rst_i),
			     .clk_i			(clk_i),
			     .stall_i        (hazard_war),
			     .stall_o        (stall_x),
			     .op_i           (dx_op),
			     .PC_i           (dx_PC),
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
			     .reg_result_o (xw_reg_result),
			     .mem_result_o (xw_mem_result),
			     .memory_address_o (xw_memory_address),
			     .memory_read_enable_o (xw_loadp),
			     .memory_write_enable_o (xw_memory_we),
			     .sp_i (rx_sp),
			     .fp_i (rx_fp));
  
  cpu_write stage_write (  // Inputs
			   .rst_i (rst_i),
			   .clk_i (clk_i),
			   .register_write_index_i (xw_register_write_index),
			   .register_we_i (xw_register_write_enable),
			   .loadp_i (xw_loadp),
			   .memory_we_i (xw_memory_we),
			   .memory_address_i (xw_memory_address),
			   .reg_result_i (xw_reg_result),
			   .mem_result_i (xw_mem_result),
			   // Outputs
			   .register_write_index_o (wr_register_write_index),
			   .register_we_o (wr_register_write_enable),
			   .reg_result_o (wr_reg_result));

  assign hazard_war = xw_register_write_enable
		      & ((xw_register_write_index == dx_reg_index1)
			 | (xw_register_write_index == dx_reg_index2));

endmodule // moxie
