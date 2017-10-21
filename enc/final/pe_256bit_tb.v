module priority_encoder_256bit_tb();
    reg [255:0] inp;
    wire [255:0] outp;
    wire [7:0] oute;
    wire validp;
    wire valide;

    priority_encoder_256bit pe256(inp, outp, validp);
    encoder256bit enc256(outp, oute, valide);

    initial begin
        inp = 0;
#10
        inp = 1;
#10
        inp = 2;
#10
        inp = 4;
#10
        inp = 8;
#10
        inp = 16;
#10
        inp = 32;
    end

    initial begin
        $monitor("%d\ninp:%b\noup:%b\nvap:%b\noue:%b\nvae:%b", $time, inp, outp, validp, oute, valide);
    end

endmodule
