module muxtask(
    input  wire [15:0] in,
    input  wire [3:0]  sel,
    output reg         out
);
    reg [3:0] mid; // 中间信号

    // 定义4-1复用器任务
    task mux4_task;
        input [3:0] task_in;
        input [1:0] task_sel;
        output reg  task_out;
        begin
            case(task_sel)
                2'b00: task_out = task_in[0];
                2'b01: task_out = task_in[1];
                2'b10: task_out = task_in[2];
                2'b11: task_out = task_in[3];
            endcase
        end
    endtask

    // 第一级：4次调用任务，得到4个中间值
    always @(*) begin
        mux4_task(in[3:0],   sel[1:0], mid[0]);
        mux4_task(in[7:4],   sel[1:0], mid[1]);
        mux4_task(in[11:8],  sel[1:0], mid[2]);
        mux4_task(in[15:12], sel[1:0], mid[3]);
        // 第二级：调用任务得到最终输出
        mux4_task(mid,       sel[3:2], out);
    end
endmodule
