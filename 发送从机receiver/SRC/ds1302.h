#ifndef __DS1302_H_
#define __DS1302_H_

//---包含头文件---//
#include<reg52.h>
#include<intrins.h>

//---重定义关键词---//
#ifndef u8
#define u8 unsigned char
#endif

#ifndef u16 
#define u16 unsigned int
#endif

//---定义ds1302使用的IO口---//
sbit DSIO=P3^4;
sbit RST=P3^5;
sbit SCLK=P3^6;

//---定义全局函数---//
void Ds1302Write(u8 addr, u8 dat);
u8 Ds1302Read(u8 addr);
void Ds1302Init();
void Ds1302ReadTime();

//---加入全局变量--//
extern u8 TIME[7];	//加入全局变量
extern u8 code WRITE_RTC_ADDR[7];

#endif