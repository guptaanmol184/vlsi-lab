module priority_encoder_4bit (in, out, valid);

    input [3:0] in;
    output [3:0] out;
    output valid;

    assign out[3] = in[3];
    assign out[2] = ~in[3] & in[2];
    assign out[1] = ~in[3] & ~in[2] & in[1];
    assign out[0] = ~in[3] & ~in[2] & ~in[1] & in[0];

    assign valid = in[3] | in[2] | in[1] | in[0];

endmodule
