/**************************************************************************
   
     					SOUND PLAY FOR 51MCU
  
               		COPYRIGHT (c)   2005 BY JJJ.
                       		--  ALL RIGHTS RESERVED  --
  
   File Name:       SoundPlay.h
   Author:          Jiang Jian Jun
   Created:         2005/5/16
   Modified:		NO
   Revision: 		1.0
  
*******************************************************************************/

/*说明**************************************************************************
 曲谱存贮格式 unsigned char code MusicName{音高，音长，音高，音长...., 0,0};	末尾:0,0 表示结束(Important)

 音高由三位数字组成：
 		个位是表示 1~7 这七个音符 
 		十位是表示音符所在的音区:1-低音，2-中音，3-高音;
 		百位表示这个音符是否要升半音: 0-不升，1-升半音。
 
 音长最多由三位数字组成： 
 		个位表示音符的时值，其对应关系是： 
 			|数值(n):  |0 |1 |2 |3 | 4 | 5 | 6 
 			|几分音符: |1 |2 |4 |8 |16 |32 |64 		音符=2^n
 		十位表示音符的演奏效果(0-2):  0-普通，1-连音，2-顿音
 		百位是符点位: 0-无符点，1-有符点

 调用演奏子程序的格式
 		Play(乐曲名,调号,升降八度,演奏速度);
	|乐曲名           : 要播放的乐曲指针,结尾以(0,0)结束;
	|调号(0-11)       :	是指乐曲升多少个半音演奏;
	|升降八度(1-3)	  : 1:降八度, 2:不升不降, 3:升八度;
	|演奏速度(1-12000):	值越大速度越快;

***************************************************************************/
#ifndef __SOUNDPLAY_H_REVISION_FIRST__
#define __SOUNDPLAY_H_REVISION_FIRST__

#include "reg52.h"
#include "sbit.h"
#include "PlayMusic.h"

//**************************************************************************
	//音长定时器初值暂存
//**************************************************************************
#define SYSTEM_OSC 		12000000	//定义晶振频率12000000HZ
#define SOUND_SPACE 	4/5 //定义普通音符演奏的长度分率
	
//**************************************************************************

unsigned char code Music_Girl[];

unsigned char code Music_Same[];
								                                   
unsigned char code Music_Two[];

unsigned char code Music_Code[];
								  
void Delay1ms(unsigned int count);


//**************************************************************************
void InitialSound(void);

void BeepTimer0(void);	//音符发生中断

//**************************************************************************
void Play(unsigned char *Sound,unsigned char Signature,unsigned Octachord,unsigned int Speed);
extern void Play_Music1();
extern void Play_Music2();
extern void Play_Music3();
extern void Play_Music4();

//***********************************************************************************
#endif