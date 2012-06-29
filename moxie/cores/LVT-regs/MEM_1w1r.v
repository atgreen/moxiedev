
`include "parameters.v"

module MEM_1w1r(
    input  wire           clock,
    input  wire we,
    input  wire `MEM_ADDR write_addr,
    input  wire `WORD     write_data,
    input  wire `MEM_ADDR read_addr,
    output reg  `WORD     read_data
); 

reg `WORD memory `MEM;

always @(negedge clock) begin
    if (we)  
      memory[write_addr] <= write_data;
end

always @(posedge clock) begin
    read_data <= memory[read_addr];
end

endmodule
