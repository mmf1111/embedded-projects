`timescale 1ns/1ps

module tb_adc0809_ctrl;

reg clk;
reg rst_n;
reg start_sample;
reg eoc;

wire ale;
wire start;
wire oe;
wire data_valid;

adc0809_ctrl uut(
    .clk(clk),
    .rst_n(rst_n),
    .start_sample(start_sample),
    .eoc(eoc),
    .ale(ale),
    .start(start),
    .oe(oe),
    .data_valid(data_valid)
);

initial
begin
    clk = 0;
    forever #10 clk = ~clk;
end

initial
begin
    rst_n = 0;
    start_sample = 0;
    eoc = 0;

    #50;
    rst_n = 1;

    //??????
    #40;
    start_sample = 1;
    #20;
    start_sample = 0;

    //ADC????
    #200;
    eoc = 1;

    #20;
    eoc = 0;

    #200;
    $stop;
end

endmodule
