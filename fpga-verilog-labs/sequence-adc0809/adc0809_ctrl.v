module adc0809_ctrl(
    input clk,
    input rst_n,
    input start_sample,
    input eoc,

    output reg ale,
    output reg start,
    output reg oe,
    output reg data_valid
);

parameter IDLE     = 2'd0;
parameter START_S  = 2'd1;
parameter WAIT_EOC = 2'd2;
parameter READ_S   = 2'd3;

reg [1:0] state,next_state;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
        state <= IDLE;
    else
        state <= next_state;
end

always @(*)
begin
    case(state)

    IDLE:
        if(start_sample)
            next_state = START_S;
        else
            next_state = IDLE;

    START_S:
        next_state = WAIT_EOC;

    WAIT_EOC:
        if(eoc)
            next_state = READ_S;
        else
            next_state = WAIT_EOC;

    READ_S:
        next_state = IDLE;

    default:
        next_state = IDLE;

    endcase
end

always @(*)
begin
    ale        = 0;
    start      = 0;
    oe         = 0;
    data_valid = 0;

    case(state)

    START_S:
    begin
        ale   = 1;
        start = 1;
    end

    READ_S:
    begin
        oe         = 1;
        data_valid = 1;
    end

    endcase
end

endmodule
