module priority_encoder_4bit_tb ();

    reg [3:0] in;
    wire [3:0] out;
    wire valid;

    priority_encoder_4bit pe1(in, out, valid);

    initial begin
        in=4'b1111;
        #10
        in=4'b1010;
        #10
        in=4'b0011;
        #10
        in=4'b0000;
        #10
        in=4'b0001;
    end

    initial begin
        $monitor("%d\t%b\t%b\t%b",$time, in, out, valid);
    end 

endmodule
