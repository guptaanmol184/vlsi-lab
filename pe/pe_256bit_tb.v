module priority_encoder_256bit_tb();
    reg [255:0] in;
    wire [255:0] out;
    wire valid;

    priority_encoder_256bit pe256(in, out, valid);

    initial begin
        in = 0;
#10
        in = 4;
#10
        in = 9;
#10
        in = 23443;
#10
        in = 7546843688436;
#10
        in = 5854455465457487468658;
#10
        in = 575875674558;
    end

    initial begin
        $monitor("%d\nin:%b\nou:%b\nva:valid", $time, in, out, valid);
    end

endmodule
