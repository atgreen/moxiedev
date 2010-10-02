project_new muskoka -overwrite

# Assign family, device, and top-level file
set_global_assignment -name FAMILY CycloneII
set_global_assignment -name DEVICE EP2C35F672C6

# ---- Assign pins -------------------------------------

# The DE2 board has PIN_N2 as a 50MHz clock and PIN_D13 as a 27MHz
# clock.
set_location_assignment -to clk_i PIN_N2

# Assign pushbutton[0] to the reset signal.
set_location_assignment -to rst_i PIN_G26

# Set up an output pin
set_location_assignment -to hazout PIN_N18

# Create timing assignments
create_base_clock -fmax "50 MHz" -target PIN_N2 clk_i

set_global_assignment -name VERILOG_FILE muskoka.v
set_global_assignment -name VERILOG_FILE cpu_fetch.v
set_global_assignment -name VERILOG_FILE cpu_ififo.v
set_global_assignment -name VERILOG_FILE cpu_decode.v
set_global_assignment -name VERILOG_FILE cpu_execute.v
set_global_assignment -name VERILOG_FILE cpu_registerfile.v

project_close
