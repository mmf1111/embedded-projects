# 电子信息工程项目集

中国计量大学电子信息工程项目作品集。收录可以查看的源码、仿真工程、PCB 文件及项目说明；各项目的验证范围在对应 README 中标注。

| 项目 | 主要内容 | 验证方式 |
|---|---|---|
| [智能小车](smart-car/) | Arduino 控制、黑线循迹、红外避障、蓝牙串口、双层 PCB | 实车制作与联调 |
| [STC15 数据采集](mcu-auto-data-acquisition/) | PCF8591 电压采集、DS18B20 温度读取、按键与数码管显示 | Keil 工程及 HEX |
| [FPGA/Verilog 课程实验](fpga-verilog-labs/) | 数据选择器、24 小时时钟、序列检测、ADC0809 控制、交通灯 | **功能仿真，未上板** |
| [汽车尾灯电路](analog-circuit-design/) | NE555、74LS 逻辑、仿真、PCB 与实物 | 仿真及实物调试 |
| [Android BLE 无感开门](android-ble-door/) | BLE 扫描、RSSI 判断、GATT 写入、前台服务 | Android 工程 |
| [竞赛数学训练网页](math-competition-trainer/) | 原生 HTML/CSS/JavaScript、MathJax、随机练习和进度保存 | 浏览器运行 |

## 阅读顺序

- 投嵌入式软件：先看 [智能小车](smart-car/) 和 [STC15 数据采集](mcu-auto-data-acquisition/)。
- 投硬件或电子测试：先看 [汽车尾灯电路](analog-circuit-design/) 和智能小车的 PCB 工程。
- 投 FPGA：先看 [Verilog 课程实验](fpga-verilog-labs/)。
- 投物联网应用：先看 [Android BLE 无感开门](android-ble-door/) 与 STC15 数据采集。

除已保留的 STC15 示例 HEX 和项目接线说明外，软件安装包、编译缓存、通用课程任务书与参考报告未收录。正在进行的挑战杯项目未收录于此仓库。
