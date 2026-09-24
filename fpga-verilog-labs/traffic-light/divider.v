module divider(
    input clk,
    input rst,
    output reg clk_1hz
);

reg [3:0] cnt;

always @(posedge clk or posedge rst)
begin
    if(rst)
    begin
        cnt <= 0;
        clk_1hz <= 0;
    end
    else if(cnt == 4)
    begin
        cnt <= 0;
        clk_1hz <= ~clk_1hz;
    end
    else
        cnt <= cnt + 1;
end

endmodule
