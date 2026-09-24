`timescale 1ns/1ps

module tb_sequence_detector;

reg clk;
reg rst_n;
reg din;
wire dout;

sequence_detector uut(
    .clk(clk),
    .rst_n(rst_n),
    .din(din),
    .dout(dout)
);

always #10 clk = ~clk;

initial
begin
    clk = 0;
    rst_n = 0;
    din = 0;

    #30;
    rst_n = 1;

    // ?????
    // 11011011
    send_bit(1);
    send_bit(1);
    send_bit(0);
    send_bit(1);
    send_bit(1);

    send_bit(0);

    send_bit(1);
    send_bit(1);

    #100;
    $stop;
end

task send_bit;
input data;
begin
    @(negedge clk);
    din = data;
end
endtask

endmodule
