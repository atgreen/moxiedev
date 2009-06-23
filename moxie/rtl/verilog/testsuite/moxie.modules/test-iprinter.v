////////////////
//Clock module//
////////////////
module cl(clk);
        parameter TIME_LIMIT = 110000;
        output  clk;
        reg clk;

        initial
                clk=0;
        always  
                #50 clk = ~clk;
        always @(posedge clk)
                if ($time > TIME_LIMIT) #70 $stop;
endmodule

module test_iprinter (/*AUTOARG*/);
 
   wire sysclk;
   cl clock(sysclk);  //instantiate clock

   reg [15:0] insn;
   reg [31:0] data;
   reg [16:0] ptr;
   
   reg [7:0] mem [0:128000];
   wire [2:0] length;

   iprinter ip (length, sysclk, insn, data);

   initial
     begin
	$readmemh("hello.vh", mem);
	ptr = 16'h1000;
	insn = 0;
	data = 0;
     end

   always @ (posedge sysclk) begin

      ptr = ptr + length;
      insn[15:8]  = mem[ptr];
      insn[7:0]   = mem[ptr+1];
      data[31:24] = mem[ptr+2];
      data[23:16] = mem[ptr+3];
      data[15:8]  = mem[ptr+4];
      data[7:0]   = mem[ptr+5];
      
      if (ptr > 8000)
	begin
	   #1000 $stop;               // Halt simulator
	   $finish;             // Stop all simulation
	end
   end
endmodule // moxiesoc
