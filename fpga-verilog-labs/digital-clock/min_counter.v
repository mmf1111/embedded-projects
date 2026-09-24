module min_counter(
    input carry_sec,
    input rst_n,
    output reg [5:0] min,
    output reg carry
);

always @(posedge carry_sec or negedge rst_n)
begin
    if(!rst_n)
    begin
        min <= 0;
        carry <= 0;
    end
    else if(min == 59)
    begin
        min <= 0;
        carry <= 1;
    end
    else
    begin
        min <= min + 1;
        carry <= 0;
    end
end

endmodule
