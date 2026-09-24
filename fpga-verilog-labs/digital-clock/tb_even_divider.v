`timescale 1ns/1ps

module tb_even_divider;

reg clk;
reg rst_n;

wire clk_div;

even_divider
#(
    .N(10)
)
u1(
    .clk(clk),
    .rst_n(rst_n),
    .clk_div(clk_div)
);

initial clk = 0;

always #10 clk = ~clk;   // 50MHz??

initial
begin
    rst_n = 0;
    #50;
    rst_n = 1;

    #2000;
    $stop;
end

endmodule
