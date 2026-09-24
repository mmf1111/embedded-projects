`timescale 1ns/1ps

module tb_clock;

reg clk;
reg rst_n;

wire [4:0] hour;
wire [5:0] min;
wire [5:0] sec;

clock_top uut(
    .clk(clk),
    .rst_n(rst_n),
    .hour(hour),
    .min(min),
    .sec(sec)
);

initial
begin
    clk = 0;
    forever #10 clk = ~clk;
end

initial
begin
    rst_n = 0;
    #20;
    rst_n = 1;

    #5000;
    $stop;
end

endmodule
