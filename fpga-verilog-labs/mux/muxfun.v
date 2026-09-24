module muxfun(
    input  wire [15:0] in,
    input  wire [3:0]  sel,
    output wire        out
);
    wire [3:0] mid;
    function muxfun;
        input [3:0] func_in;
        input [1:0] func_sel;
        begin
            case(func_sel)
                2'b00: muxfun = func_in[0];
                2'b01: muxfun = func_in[1];
                2'b10: muxfun = func_in[2];
                2'b11: muxfun = func_in[3];
            endcase
        end
    endfunction

    // 第一级：调用函数计算中间值
    assign mid[0] = muxfun(in[3:0],   sel[1:0]);
    assign mid[1] = muxfun(in[7:4],   sel[1:0]);
    assign mid[2] = muxfun(in[11:8],  sel[1:0]);
    assign mid[3] = muxfun(in[15:12], sel[1:0]);
    // 第二级：调用函数计算最终输出
    assign out    = muxfun(mid,       sel[3:2]);
endmodule
