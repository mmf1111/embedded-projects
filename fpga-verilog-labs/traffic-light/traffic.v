module traffic(
    input clk_1hz,
    input rst,

    output reg [1:0] state,
    output reg [2:0] lightA,
    output reg [2:0] lightB,
    output reg [5:0] timer
);

parameter S0 = 2'b00;
parameter S1 = 2'b01;
parameter S2 = 2'b10;
parameter S3 = 2'b11;

always @(posedge clk_1hz or posedge rst)
begin
    if(rst)
    begin
        state <= S0;
        timer <= 5;
    end
    else
    begin
        if(timer == 0)
        begin
            case(state)

                S0:
                begin
                    state <= S1;
                    timer <= 2;
                end

                S1:
                begin
                    state <= S2;
                    timer <= 5;
                end

                S2:
                begin
                    state <= S3;
                    timer <= 2;
                end

                S3:
                begin
                    state <= S0;
                    timer <= 5;
                end

            endcase
        end
        else
            timer <= timer - 1;
    end
end

always @(*)
begin
    case(state)

        S0:
        begin
            lightA = 3'b001;
            lightB = 3'b100;
        end

        S1:
        begin
            lightA = 3'b010;
            lightB = 3'b100;
        end

        S2:
        begin
            lightA = 3'b100;
            lightB = 3'b001;
        end

        S3:
        begin
            lightA = 3'b100;
            lightB = 3'b010;
        end

        default:
        begin
            lightA = 3'b100;
            lightB = 3'b100;
        end

    endcase
end

endmodule
