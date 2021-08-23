#ifndef __TEMP_H_
#define __TEMP_H_

#include<reg52.h>
//---重定义关键词---//
#ifndef u8
#define u8 unsigned char
#endif

#ifndef u16 
#define u16 unsigned int
#endif

//--定义使用的IO口--//
sbit DSPORT=P3^7;

//--声明全局函数--//
void Delay1ms(u16 );
void Delay_DS18B20(int);
u8 Ds18b20Init();
void Ds18b20WriteByte(u8 com);
u8 Ds18b20ReadByte();
void  Ds18b20ChangTemp();
void  Ds18b20ReadTempCom();
unsigned int Ds18b20ReadTemp();

#endif
