module sram_tcam_tb();
    reg [143:0] addr;
    wire [255:0] out;

    sram_tcam s(addr, out);

    initial begin
        addr = 8'b00000000;
#10
        addr = 8'b01010101;
#10
        addr = 8'b11111111;
#10
        addr = 8'b01000001;

    end

    initial begin
        $monitor("addr:%b\nout:%b\n", addr, out);
    end

endmodule