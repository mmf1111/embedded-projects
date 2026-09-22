#ifndef __LED_H_
#define __LED_H_
#include "delay.h"

/*********************************************
??????
**********************************************/	
//???????D1?D2?D3?D4?D5?D6?D7?D8?GPIO
#define LED1_P00     P00
#define LED2_P01     P01 
#define LED3_P02     P02 
#define LED4_P03     P03  
#define LED5_P04     P04
#define LED6_P05     P05 
#define LED7_P06     P06 
#define LED8_P07     P07  

//????????
#define LED_1          1
#define LED_2          2
#define LED_3          3
#define LED_4          4
#define LED_5          5
#define LED_6          6
#define LED_7          7
#define LED_8          8

extern void led_on(u8 led_idx);
extern void led_off(u8 led_idx);
extern void led_toggle(u8 led_idx);
extern void leds_on(void);
extern void leds_off(void);

#endif