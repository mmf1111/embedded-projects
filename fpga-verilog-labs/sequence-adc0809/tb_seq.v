module tb_seq;

reg clk;
reg rst_n;
reg din;
wire dout;

seq_detector_moore uut(
    .clk(clk),
    .rst_n(rst_n),
    .din(din),
    .dout(dout)
);

// 时钟
initial begin
    clk = 0;
    forever #10 clk = ~clk;
end

// 激励
initial begin
    rst_n = 0;
    din = 0;
    #50;
    rst_n = 1;

    #20; din = 1;
    #20; din = 1;
    #20; din = 0;
    #20; din = 1;
    #20; din = 1;

    #20; din = 0;
    #20; din = 0;

    #100;
    $stop;
end

endmodule
