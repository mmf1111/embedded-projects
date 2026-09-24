module hour_counter(
    input carry_min,
    input rst_n,
    output reg [4:0] hour
);

always @(posedge carry_min or negedge rst_n)
begin
    if(!rst_n)
        hour <= 0;
    else if(hour == 23)
        hour <= 0;
    else
        hour <= hour + 1;
end

endmodule
