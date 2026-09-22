#include  <stdio.h>
#include  "delay.h"
#include  "led.h"
#include  "keyboard.h"
#include  "ledseg.h" 	
#include  "timer.h"
#include  "pcf8591.h"	
#include  "i2c_hw.h"
#include  "ds18b20.h" 

void ULN2003_off(void)
{
	//控制Y5C输出高电平，即打开控制ULN2003的锁存器（U11:74HC573）
    P25=1;	    
	P26=0;	       
	P27=1;	       
	
	P0=0x00;      
}

void SEG_off(void)
{
	//控制Y6C输出高电平，即打开控制U13的锁存器（U13:74HC573）
    P25=0;	      
	P26=1;	       
	P27=1;	       
	P0=0x00;      
	
	//控制Y7C输出高电平，即打开控制U14的锁存器（U14:74HC573）
    P25=1;	    
	P26=1;	      
	P27=1;	      
	P0=0x00;      
}

int main(void)
{
    u8 btn_val;
	u16 temp=0;
	u16 adc_value=0;
	static u8 adc_channel=4;  // 默认ADC通道3
	static u8 adcbuff[5]; 
	static u8 temp_buff[6];   // 温度字符串缓冲区
	float fTemp;              // 温度浮点值
	static u8 display_mode=0; // 显示模式：0-ADC电压，1-温度
	u8 uTemp;               
	
	// GPIO初始化
	P2M1 &= 0x1F;	P2M0 |= 0xE0;	  // P2.5、P2.6、P2.7推挽输出
	P0M1 &= 0x00;	P0M0 |= 0xFF;	  // P0.0~P0.7推挽输出
	P3M1 &= 0xF3;	P3M0 &= 0xF3;	  // P3.2~P3.3准双向口
	P4M1 &= 0xEB;	P4M0 &= 0xEB;	  // P4.2、P4.4准双向口
	P1M1 &= 0xEF;	P1M0 &= 0xEF;	  // P1.4准双向口（DS18B20引脚）
			
	SEG_off();                    // 关闭数码管显示
	leds_off();	                  // 熄灭所有LED
	ULN2003_off();                // 关闭蜂鸣器、继电器等
	delay_ms(10);                 // 初始化延时
	
	I2C_init();                     // I2C总线初始化
	timer2_init();                  // 定时器2初始化（数码管扫描）
	timer2_start();                 // 启动定时器2
	EA = 1;                         // 使能总中断
	delay_ms(10);                   // 初始化后延时
	
  while(1)
  {
    btn_val=keyboard_scan();            // 扫描矩阵按键
    
    // 按键S9：切换到ADC通道3电压显示
    if(btn_val == BUTTON6_PRESSED)        
    {
			display_mode = 0;	// 设置为ADC显示模式
			adc_channel = 4;	// 选择ADC通道3                 
    }
		// 按键S5：切换到DS18B20温度显示
    else if(btn_val == BUTTON2_PRESSED)   
    {
			display_mode = 1;	// 设置为温度显示模式       
    } 
    
    // 根据显示模式更新数码管
		if(display_mode == 0)     // ADC电压显示模式
	  {	 
            // 两次读取软件滤波
            temp =(u16)PCF8591_read_byte(adc_channel-1);
			temp =(u16)PCF8591_read_byte(adc_channel-1);
			// 8位ADC转换为mV：5000mV * 采样值 / 255 ≈ 19.61mV/单位
			adc_value = 19.61*temp;
				
            adcbuff[0]=adc_value%10000/1000;      
            adcbuff[1]=adc_value%10000%1000/100;
            adcbuff[2]=adc_value%10000%1000%100/10;
            adcbuff[3]=adc_value%10000%1000%100%10;
                            
            // 更新数码管（第5-8位显示电压）
            LEDseg_DispUpdata(LEDSEG_5,adcbuff[0],LEDSEG_DP_ON);
			LEDseg_DispUpdata(LEDSEG_6,adcbuff[1],LEDSEG_DP_OFF);
            LEDseg_DispUpdata(LEDSEG_7,adcbuff[2],LEDSEG_DP_OFF);
            LEDseg_DispUpdata(LEDSEG_8,adcbuff[3],LEDSEG_DP_OFF);
            // 熄灭前4位数码管
            LEDseg_DispUpdata(LEDSEG_1,17,LEDSEG_DP_OFF);
            LEDseg_DispUpdata(LEDSEG_2,17,LEDSEG_DP_OFF);
            LEDseg_DispUpdata(LEDSEG_3,17,LEDSEG_DP_OFF);
            LEDseg_DispUpdata(LEDSEG_4,17,LEDSEG_DP_OFF);
	  }
    else if(display_mode == 1) // 温度显示模式
    {
        memset(temp_buff, 0, sizeof(temp_buff));
        fTemp = floatReadDs18B20();  // 读取温度值（保留1位小数）
        sprintf(temp_buff, "%.01f", fTemp);  // 浮点数转字符串
        uTemp = (u8)fTemp;
		
		// 非负温显示
		if((ReadTempFlag==0)&&(uTemp<10))        
		{  
             LEDseg_DispUpdata(LEDSEG_5,17,LEDSEG_DP_OFF);
			 LEDseg_DispUpdata(LEDSEG_6,17,LEDSEG_DP_OFF);
             LEDseg_DispUpdata(LEDSEG_7,temp_buff[0]-0x30,LEDSEG_DP_ON);
             LEDseg_DispUpdata(LEDSEG_8,temp_buff[2]-0x30,LEDSEG_DP_OFF);
		}
		else if((ReadTempFlag==0)&&(uTemp>9))      
		{  
             LEDseg_DispUpdata(LEDSEG_5,17,LEDSEG_DP_OFF);
			 LEDseg_DispUpdata(LEDSEG_6,temp_buff[0]-0x30,LEDSEG_DP_OFF);
             LEDseg_DispUpdata(LEDSEG_7,temp_buff[1]-0x30,LEDSEG_DP_ON);
             LEDseg_DispUpdata(LEDSEG_8,temp_buff[3]-0x30,LEDSEG_DP_OFF);
		}
		// 负温显示（带负号）
		if((ReadTempFlag==1)&&(uTemp<10)) 
		{
			 LEDseg_DispUpdata(LEDSEG_5,17,LEDSEG_DP_OFF);
			 LEDseg_DispUpdata(LEDSEG_6,16,LEDSEG_DP_OFF);  // 显示负号
             LEDseg_DispUpdata(LEDSEG_7,temp_buff[0]-0x30,LEDSEG_DP_ON);
             LEDseg_DispUpdata(LEDSEG_8,temp_buff[2]-0x30,LEDSEG_DP_OFF);
		}
		else if((ReadTempFlag==1)&&(uTemp>9))  
		{  
             LEDseg_DispUpdata(LEDSEG_5,16,LEDSEG_DP_OFF);  // 显示负号
			 LEDseg_DispUpdata(LEDSEG_6,temp_buff[0]-0x30,LEDSEG_DP_OFF);
             LEDseg_DispUpdata(LEDSEG_7,temp_buff[1]-0x30,LEDSEG_DP_ON);
             LEDseg_DispUpdata(LEDSEG_8,temp_buff[3]-0x30,LEDSEG_DP_OFF);
		}
        // 熄灭前4位数码管
        LEDseg_DispUpdata(LEDSEG_1,17,LEDSEG_DP_OFF);
        LEDseg_DispUpdata(LEDSEG_2,17,LEDSEG_DP_OFF);
        LEDseg_DispUpdata(LEDSEG_3,17,LEDSEG_DP_OFF);
        LEDseg_DispUpdata(LEDSEG_4,17,LEDSEG_DP_OFF);
    }
	}
}