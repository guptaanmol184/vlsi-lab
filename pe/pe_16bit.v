module priority_encoder_16bit (in, out, valid);

    input [15:0] in;
    output [15:0] out;
    output valid;

    wire [3:0]pri[0:3];
    wire [3:0]vali;
    wire [3:0]temp;

    priority_encoder_4bit pe1(in[15:12], pri[3], vali[3]);
    priority_encoder_4bit pe2(in[11:8], pri[2], vali[2]);
    priority_encoder_4bit pe3(in[7:4], pri[1], vali[1]);
    priority_encoder_4bit pe4(in[3:0], pri[0], vali[0]);

    priority_encoder_4bit pe5(vali, temp, valid);

   assign out[0]= temp[0] & pri[0][0];
   assign out[1]= temp[0] & pri[0][1];
   assign out[2]= temp[0] & pri[0][2];
   assign out[3]= temp[0] & pri[0][3];
   assign out[4]= temp[1] & pri[1][0];
   assign out[5]= temp[1] & pri[1][1];
   assign out[6]= temp[1] & pri[1][2];
   assign out[7]= temp[1] & pri[1][3];
   assign out[8]= temp[2] & pri[2][0];
   assign out[9]= temp[2] & pri[2][1];
   assign out[10]= temp[2] & pri[2][2];
   assign out[11]= temp[2] & pri[2][3];
   assign out[12]= temp[3] & pri[3][0];
   assign out[13]= temp[3] & pri[3][1];
   assign out[14]= temp[3] & pri[3][2];
   assign out[15]= temp[3] & pri[3][3];

endmodule

