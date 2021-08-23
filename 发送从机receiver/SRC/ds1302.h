#ifndef __DS1302_H_
#define __DS1302_H_

//---����ͷ�ļ�---//
#include<reg52.h>
#include<intrins.h>

//---�ض���ؼ���---//
#ifndef u8
#define u8 unsigned char
#endif

#ifndef u16 
#define u16 unsigned int
#endif

//---����ds1302ʹ�õ�IO��---//
sbit DSIO=P3^4;
sbit RST=P3^5;
sbit SCLK=P3^6;

//---����ȫ�ֺ���---//
void Ds1302Write(u8 addr, u8 dat);
u8 Ds1302Read(u8 addr);
void Ds1302Init();
void Ds1302ReadTime();

//---����ȫ�ֱ���--//
extern u8 TIME[7];	//����ȫ�ֱ���
extern u8 code WRITE_RTC_ADDR[7];

#endif