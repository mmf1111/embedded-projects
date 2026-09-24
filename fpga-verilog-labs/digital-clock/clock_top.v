module clock_top(
    input clk,
    input rst_n,

    output [4:0] hour,
    output [5:0] min,
    output [5:0] sec
);

wire carry_sec;
wire carry_min;

sec_counter u_sec(
    .clk(clk),
    .rst_n(rst_n),
    .sec(sec),
    .carry(carry_sec)
);

min_counter u_min(
    .carry_sec(carry_sec),
    .rst_n(rst_n),
    .min(min),
    .carry(carry_min)
);

hour_counter u_hour(
    .carry_min(carry_min),
    .rst_n(rst_n),
    .hour(hour)
);

endmodule
