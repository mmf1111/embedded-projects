module seq_detector_moore(
    input clk,
    input rst_n,
    input din,
    output reg dout
);

// 状态定义
parameter S0 = 5'b00001;
parameter S1 = 5'b00010;
parameter S2 = 5'b00100;
parameter S3 = 5'b01000;
parameter S4 = 5'b10000;

reg [4:0] current_state;
reg [4:0] next_state;

// 时序逻辑：状态更新
always @(posedge clk or negedge rst_n) begin
    if(!rst_n)
        current_state <= S0;
    else
        current_state <= next_state;
end

// 组合逻辑：状态转移
always @(*) begin
    case(current_state)
        S0: next_state = din ? S1 : S0;
        S1: next_state = din ? S2 : S0;
        S2: next_state = din ? S2 : S3;
        S3: next_state = din ? S4 : S0;
        S4: next_state = din ? S2 : S0; // 允许重叠
        default: next_state = S0;
    endcase
end

// Moore输出：只与状态有关
always @(*) begin
    if(current_state == S4 && din == 1)
        dout = 1'b1;
    else
        dout = 1'b0;
end

endmodule
