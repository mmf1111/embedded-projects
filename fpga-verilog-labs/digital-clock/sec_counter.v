module sec_counter(
    input clk,
    input rst_n,
    output reg [5:0] sec,   // 0~59 ??6?
    output reg carry         // ??60???
);

always @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin
        sec <= 0;
        carry <= 0;
    end
    else begin
        if(sec == 59) begin
            sec <= 0;
            carry <= 1;
        end
        else begin
            sec <= sec + 1;
            carry <= 0;
        end
    end
end

endmodule
