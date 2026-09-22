#ifndef __I2C_HW_H_
#define __I2C_HW_H_
#include	"delay.h"

sbit	SDA	= P2^1;	   //??SDA  
sbit	SCL	= P2^0;	   //??SCL

extern void I2C_init(void);
extern void I2C_Start(void);
extern void I2C_SendData(u8 dat);
extern bit  I2C_WaitACK(void);
extern void I2C_SendACK(bit ackbit);
extern void I2C_SendNAK(void);
extern u8 I2C_RecvData(void);
extern void I2C_Stop(void);

#endif