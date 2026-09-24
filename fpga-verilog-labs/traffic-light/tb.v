`timescale 1ns/1ps

module tb;

reg clk;
reg rst;

wire clk_1hz;

wire [1:0] state;
wire [2:0] lightA;
wire [2:0] lightB;
wire [5:0] timer;

divider u1(
    .clk(clk),
    .rst(rst),
    .clk_1hz(clk_1hz)
);

traffic u2(
    .clk_1hz(clk_1hz),
    .rst(rst),
    .state(state),
    .lightA(lightA),
    .lightB(lightB),
    .timer(timer)
);

always #10 clk = ~clk;

initial
begin
    clk = 0;
    rst = 1;

    #30;
    rst = 0;

    #100000;
    $stop;
end

endmodule
