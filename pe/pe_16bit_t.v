module pe_16bit_tb();
    reg [15:0] in;
    wire[15:0] out;
    wire valid;


    priority_encoder_16bit pee(in, out, valid);

    initial begin
        in=16'b0000000000000000;
    end

    initial begin
        $monitor("%d\t%b\t%b\t%b", $time, in, out, valid);
    end

endmodule
