module sram_tcam(addr,out);

    input [143:0]addr;
    output [255:0]out;

    wire [255:0]w[0:71];

    sram a1(addr[1:0],w[0]);
    sram a2(addr[3:2],w[1]);
    sram a3(addr[5:4],w[2]);
    sram a4(addr[7:6],w[3]);
    sram a5(addr[9:8],w[4]);
    sram a6(addr[11:10],w[5]);
    sram a7(addr[13:12],w[6]);
    sram a8(addr[15:14],w[7]);
    sram a9(addr[17:16],w[8]);
    sram a10(addr[19:18],w[9]);
    sram a11(addr[21:20],w[10]);
    sram a12(addr[23:22],w[11]);
    sram a13(addr[25:24],w[12]);
    sram a14(addr[27:26],w[13]);
    sram a15(addr[29:28],w[14]);
    sram a16(addr[31:30],w[15]);
    sram a17(addr[33:32],w[16]);
    sram a18(addr[35:34],w[17]);
    sram a19(addr[37:36],w[18]);
    sram a20(addr[39:38],w[19]);
    sram a21(addr[41:40],w[20]);
    sram a22(addr[43:42],w[21]);
    sram a23(addr[45:44],w[22]);
    sram a24(addr[47:46],w[23]);
    sram a25(addr[49:48],w[24]);
    sram a26(addr[51:50],w[25]);
    sram a27(addr[53:52],w[26]);
    sram a28(addr[55:54],w[27]);
    sram a29(addr[57:56],w[28]);
    sram a30(addr[59:58],w[29]);
    sram a31(addr[61:60],w[30]);
    sram a32(addr[63:62],w[31]);
    sram a33(addr[65:64],w[32]);
    sram a34(addr[67:66],w[33]);
    sram a35(addr[69:68],w[34]);
    sram a36(addr[71:70],w[35]);
    sram a37(addr[73:72],w[36]);
    sram a38(addr[75:74],w[37]);
    sram a39(addr[77:76],w[38]);
    sram a40(addr[79:78],w[39]);
    sram a41(addr[81:80],w[40]);
    sram a42(addr[83:82],w[41]);
    sram a43(addr[85:84],w[42]);
    sram a44(addr[87:86],w[43]);
    sram a45(addr[89:88],w[44]);
    sram a46(addr[91:90],w[45]);
    sram a47(addr[93:92],w[46]);
    sram a48(addr[95:94],w[47]);
    sram a49(addr[97:96],w[48]);
    sram a50(addr[99:98],w[49]);
    sram a51(addr[101:100],w[50]);
    sram a52(addr[103:102],w[51]);
    sram a53(addr[105:104],w[52]);
    sram a54(addr[107:106],w[53]);
    sram a55(addr[109:108],w[54]);
    sram a56(addr[111:110],w[55]);
    sram a57(addr[113:112],w[56]);
    sram a58(addr[115:114],w[57]);
    sram a59(addr[117:116],w[58]);
    sram a60(addr[119:118],w[59]);
    sram a61(addr[121:120],w[60]);
    sram a62(addr[123:122],w[61]);
    sram a63(addr[125:124],w[62]);
    sram a64(addr[127:126],w[63]);
    sram a65(addr[129:128],w[64]);
    sram a66(addr[131:130],w[65]);
    sram a67(addr[133:132],w[66]);
    sram a68(addr[135:134],w[67]);
    sram a69(addr[137:136],w[68]);
    sram a70(addr[139:138],w[69]);
    sram a71(addr[141:140],w[70]);
    sram a72(addr[143:142],w[71]);

    assign out= w[0] & w[1] & w[2] & w[3] & w[4] & w[5] & w[6] & w[7] & w[8] & w[9] & w[10] & w[11] & w[12] & w[13] & w[14] & w[15] & w[16] & w[17] & w[18] & w[19] & w[20] & w[21] & w[22] & w[23] & w[24] & w[25] & w[26] & w[27] & w[28] & w[29] & w[30] & w[31] & w[32] & w[33] & w[34] & w[35] & w[36] & w[37] & w[38] & w[39] & w[40] & w[41] & w[42] & w[43] & w[44] & w[45] & w[46] & w[47] & w[48] & w[49] & w[50] & w[51] & w[52] & w[53] & w[54] & w[55] & w[56] & w[57] & w[58] & w[59] & w[60] & w[61] & w[62] & w[63] & w[64] & w[65] & w[66] & w[67] & w[68] & w[69] & w[70] & w[71];

endmodule    

module sram(addr,out);

    input [1:0]addr;
    output [255:0]out;

    wire [255:0]mem[0:3];
    reg [255:0]arr;

    assign mem[0][255:0]=144'd12333333;
    assign mem[1][255:0]=144'd7382917389;
    assign mem[2][255:0]=144'd3480924377;
    assign mem[3][255:0]=144'd7273621678;

    always @ (addr)
    case (addr)
        4'd0 : arr =mem[0];    
        4'd1 : arr =mem[1];    
        4'd2 : arr =mem[2];    
        4'd3 : arr =mem[3];    
    endcase

    assign out=arr;

endmodule    

