module sequence_detector(
    input  wire clk,
    input  wire rst_n,
    input  wire din,
    output reg  dout
);

parameter S0 = 3'd0;
parameter S1 = 3'd1;
parameter S2 = 3'd2;
parameter S3 = 3'd3;
parameter S4 = 3'd4;
parameter S5 = 3'd5;

reg [2:0] state,next_state;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
        state <= S0;
    else
        state <= next_state;
end

always @(*)
begin
    case(state)

    S0:
        if(din) next_state = S1;
        else    next_state = S0;

    S1:
        if(din) next_state = S2;
        else    next_state = S0;

    S2:
        if(din) next_state = S2;
        else    next_state = S3;

    S3:
        if(din) next_state = S4;
        else    next_state = S0;

    S4:
        if(din) next_state = S5;
        else    next_state = S0;

    S5:
        if(din) next_state = S2;
        else    next_state = S3;

    default:
        next_state = S0;

    endcase
end

always @(*)
begin
    if(state == S5)
        dout = 1'b1;
    else
        dout = 1'b0;
end

endmodule
