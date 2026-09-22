#ifndef __PCF8591_H_
#define __PCF8591_H_
#include	"delay.h"

#define PCF8591_ADDR_W	0x90               //I2C?????
#define PCF8591_ADDR_R	0x91               //I2C?????
#define PCF8591_DAC	0x40                   //DAC??????

extern u8 PCF8591_read_byte(u8 Channel);
extern void PCF8591_write_byte(u8 dat);
   	
#endif