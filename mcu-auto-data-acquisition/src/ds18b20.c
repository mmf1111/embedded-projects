#include  "ds18b20.h" 
volatile u8 ReadTempFlag;

/**************************************
 * ?  ? : ??DS18B20,?????????
 * ?  ? : ?
 * ??? : ?
**************************************/
void DS18B20_Reset(void)
{
    CY = 1;
    while (CY)
    {
        DATA_PIN = 0;               //?????????
        delay_us(240);              //????480us
        delay_us(240);
        DATA_PIN = 1;               //?????
        delay_us(60);               //??60us
        CY = DATA_PIN;              //??????
        delay_us(240);              //?????????
        delay_us(180);
    }
}

/**************************************
 * ?  ? : ?DS18B20?1????
 * ?  ? : ?
 * ??? : ???1????
**************************************/
u8 DS18B20_ReadByte(void)
{
    u8 i;
    u8 dat = 0;
    for (i=0; i<8; i++)            
    {
        dat >>= 1;
        DATA_PIN = 0;               //?????
        delay_us(1);                //????
        DATA_PIN = 1;               //????
        delay_us(1);                //????
        if (DATA_PIN) dat |= 0x80;  //????
        delay_us(60);               //???????
    }
    return dat;
}

/**************************************
 * ?  ? : ?DS18B20?1????
 * ?  ? : ?
 * ??? : ?
**************************************/
void DS18B20_WriteByte(u8 dat)
{
    u8 i;
    for (i=0; i<8; i++)             
    {
        DATA_PIN = 0;               //?????
        delay_us(1);                //????
        dat >>= 1;                  //????
        DATA_PIN = CY;
        delay_us(60);               //???????
        DATA_PIN = 1;               //?????
        delay_us(1);                //????
    }
}

/******************************************************************************
 * ?  ? : ??????
 * ?  ? : ?
 * ??? : ???
 *****************************************************************************/
float floatReadDs18B20(void) 
{ 
      u8 TempL,TempH;              //????8? ??
	  short s_tem;
      float fValue;
	
	  DS18B20_Reset();                //????
      DS18B20_WriteByte(0xCC);        //??ROM??
      DS18B20_WriteByte(0x44);        //??????
      while (!DATA_PIN);              //??????
      DS18B20_Reset();                //????
      DS18B20_WriteByte(0xCC);        //??ROM??
      DS18B20_WriteByte(0xBE);        //????????(???9????) ???????
      TempL = DS18B20_ReadByte();     //??????
      TempH = DS18B20_ReadByte();     //??????
	
	  s_tem = TempH<<8;
	  s_tem = s_tem | TempL;
	
	  if( s_tem < 0 )	   //?????????,???????
	  {
			ReadTempFlag=1;    //???,?ReadTempFlag??????1
			fValue = (~s_tem+1) * 0.0625;
	  }
	  else
	  {
			ReadTempFlag=0;    //????,?ReadTempFlag??????0
			fValue = s_tem * 0.0625;
	  }
    
      return fValue;          //?????
}