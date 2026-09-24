
module even_divider
#(
    parameter N = 10
)
(
    input clk,
    input rst_n,
    output reg clk_div
);

reg [31:0] cnt;

always @(posedge clk or negedge rst_n)
begin
    if(!rst_n)
    begin
        cnt <= 0;
        clk_div <= 0;
    end
    else
    begin
        if(cnt == N/2-1)
        begin
            cnt <= 0;
            clk_div <= ~clk_div;
        end
        else
            cnt <= cnt + 1;
    end
end

endmodule
