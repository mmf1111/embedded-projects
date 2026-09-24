# FPGA / Verilog 课程实验

现代逻辑设计课程中的四组 Verilog 工程。**这些项目完成了源码和功能仿真，未在 FPGA 开发板上验证。** 源文件已按功能整理，Quartus/ModelSim 可分别建立工程并运行相应 testbench。

| 目录 | 模块与验证重点 |
|---|---|
| [`mux/`](mux/) | 4 选 1 组合逻辑，以及由两级 4 选 1 组成的 16 选 1；练习 `function` 和 `task` 两种建模方式 |
| [`digital-clock/`](digital-clock/) | 参数化偶数分频器、秒/分/时计数模块及 24 小时制顶层；`tb_even_divider.v` 和 `tb_clock.v` 提供时钟、复位激励 |
| [`sequence-adc0809/`](sequence-adc0809/) | “11011”重叠序列检测；ADC0809 的启动、等待 EOC 和读取控制状态机，包含对应 testbench |
| [`traffic-light/`](traffic-light/) | 四状态双向交通灯及倒计时；`tb.v` 驱动时钟、复位并观察状态和灯色 |

## 使用

1. 在 Quartus Prime 或 ModelSim 中新建工程，加入某一子目录下的 `.v` 文件。
2. 将设计模块设为顶层，功能仿真时将该目录中的 `tb_*.v` 或 `tb.v` 设为仿真顶层。
3. 检查复位、计数进位、状态跳转和控制信号。数字时钟的测试激励只覆盖短时计数；不能据此声称完整验证了 24 小时边界。

本仓库未收录 Quartus、ModelSim 安装包和编译缓存。
