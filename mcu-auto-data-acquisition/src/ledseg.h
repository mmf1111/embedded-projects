#ifndef __LEDSEG_H_
#define __LEDSEG_H_
#include "delay.h"

#define  LEDSEG_DP_ON   ~0x80
#define  LEDSEG_DP_OFF  ~0x00
#define  LEDSEG_1       0
#define  LEDSEG_2       1
#define  LEDSEG_3       2
#define  LEDSEG_4       3
#define  LEDSEG_5       4
#define  LEDSEG_6       5
#define  LEDSEG_7       6
#define  LEDSEG_8       7

extern void LEDseg_nodeSelect(u8 nod);
extern void LEDseg_write_data(u8 nod);
extern void LEDseg_DispUpdata(u8 num,u8 dat,u8 dp);
extern void LEDseg_off(void);

#endif