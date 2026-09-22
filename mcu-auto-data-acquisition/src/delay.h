#ifndef __DELAY_H_
#define __DELAY_H_
#include	"stc15.H"
#include    "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;
typedef signed char     int8;   //  8 bits 
typedef signed int      int16;  // 16 bits 
typedef signed long     int32;  // 32 bits 

extern void delay_ms(u16 x);
extern void delay_us(u8 us);

#endif