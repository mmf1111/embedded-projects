module mux_4to1(a,b,c,d,s1,s0,f);
input a,b,c,d;
input s0,s1;
output f;
reg f;
always @(a or b or c or d or s1 or s0)
 begin
  case({s1,s0})
    2'b00: f<=a;
	 2'b01: f<=b;
	 2'b10: f<=c;
	 2'b11: f<=d;
	 default: f<=a;
  endcase
 end
endmodule
