
#include "reg52.h"
#include "intrins.h"
#include "uart.h"
#include "sbit.h"
#include "EEPROM.h"

sfr IAP_DATA    = 0xE2;
sfr IAP_ADDRH   = 0xE3;
sfr IAP_ADDRL   = 0xE4;
sfr IAP_CMD     = 0xE5;
sfr IAP_TRIG    = 0xE6;
sfr IAP_CONTR   = 0xE7;

//定义Flash 操作等待时间及允许IAP/ISP/EEPROM 操作的常数
//#define ENABLE_ISP 0x80 //系统工作时钟<5MHz 时，对IAP_CONTR 寄存器设置此值
//#define ENABLE_ISP 0x81 //系统工作时钟<10MHz 时，对IAP_CONTR 寄存器设置此值
#define ENABLE_ISP 0x82 //系统工作时钟<20MHz 时，对IAP_CONTR 寄存器设置此值
//#define ENABLE_ISP 0x83 //系统工作时钟<40MHz 时，对IAP_CONTR 寄存器设置此值

#define DATA_FLASH_START_ADDRESS 0x8000 //STC89C58rd 系列 EEPROM 测试起始地址
//------------------------------------------------------------------------------------------

u8 Byte_Read(u16 add);              //读一字节，调用前需打开IAP 功能
void Byte_Program(u16 add, u8 ch); //字节编程，调用前需打开IAP 功能
void Sector_Erase(u16 add);            //擦除扇区
void IAP_Disable();                       //关闭IAP 功能
void delayms(u16 z);
void EEPROM_Init();
//------------------------------------------------------------------------------
/*struct EEP_dat
{   
u16 add;
u8 dat;
}count;*/
//------------------------------------------------------------------------------
//a: 扇区 (0~7) 代表1到8个扇区
void EEPROM_WRITE(u8 a,u8 *p,u8 b)
{	
   u8 i;
   Sector_Erase(DATA_FLASH_START_ADDRESS+0x2000*a);//删除第a+1个扇区内容，才能写入  

   for(i=0;i<b;i++)
   Byte_Program(DATA_FLASH_START_ADDRESS+0x2000*a+i,p[i]);
   delayms(500);

}//-----------------------------------------------------------------------------
void EEPROM_READ(u8 a, u8 *p, u8 b)
{
   u8 i;
   
   for(i=0;i<b;i++)
   p[i]=Byte_Read(DATA_FLASH_START_ADDRESS+0x2000*a+i);	    
   delayms(500);
}
//-----------------------------------------------------------------------------
//读一字节，调用前需打开IAP 功能，入口:DPTR = 字节地址，返回:A = 读出字节
u8 Byte_Read(u16 add)
{
    IAP_DATA = 0x00;
    IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间
    IAP_CMD = 0x01;                 //IAP/ISP/EEPROM 字节读命令

    IAP_ADDRH = (u8)(add>>8);    //设置目标单元地址的高8 位地址
    IAP_ADDRL = (u8)(add&0xff);    //设置目标单元地址的低8 位地址

    EA = 0;
    IAP_TRIG = 0x46;   //先送 46h,再送B9h 到ISP/IAP 触发寄存器,每次都需如此
    IAP_TRIG = 0xB9;   //送完 B9h 后，ISP/IAP 命令立即被触发起动
    _nop_();
    EA = 1;
    IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态,
                    //一次连续的IAP 操作完成之后建议关闭IAP 功能,不需要每次都关
    return (IAP_DATA);
}//------------------------------------------------------------------------------

//字节编程，调用前需打开IAP 功能，入口:DPTR = 字节地址, A= 须编程字节的数据
void Byte_Program(u16 add,u8 ch)
{
    IAP_CONTR = ENABLE_ISP;         //打开 IAP 功能, 设置Flash 操作等待时间
    IAP_CMD = 0x02;                 //IAP/ISP/EEPROM 字节编程命令


    IAP_ADDRH = (u8)(add>>8);    //设置目标单元地址的高8 位地址
    IAP_ADDRL = (u8)(add&0xff);    //设置目标单元地址的低8 位地址

    IAP_DATA = ch;                  //要编程的数据先送进IAP_DATA 寄存器
    EA = 0;
    IAP_TRIG = 0x46;   //先送 46h,再送B9h 到ISP/IAP 触发寄存器,每次都需如此
    IAP_TRIG = 0xB9;   //送完 B9h 后，ISP/IAP 命令立即被触发起动
    _nop_();
    EA = 1;
    IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态,
                    //一次连续的IAP 操作完成之后建议关闭IAP 功能,不需要每次都关
}//------------------------------------------------------------------------------

//擦除扇区, 入口:DPTR = 扇区地址
void Sector_Erase(u16 add)
{
    IAP_CONTR = ENABLE_ISP;         //打开IAP 功能, 设置Flash 操作等待时间
    IAP_CMD = 0x03;                 //IAP/ISP/EEPROM 扇区擦除命令

    IAP_ADDRH = (u8)(add>>8);    //设置目标单元地址的高8 位地址
    IAP_ADDRL = (u8)(add&0xff);    //设置目标单元地址的低8 位地址

    EA = 0;
    IAP_TRIG = 0x46;   //先送 46h,再送B9h 到ISP/IAP 触发寄存器,每次都需如此
    IAP_TRIG = 0xB9;   //送完 B9h 后，ISP/IAP 命令立即被触发起动
    _nop_();
    EA = 1;
    IAP_Disable(); //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态,
                    //一次连续的IAP 操作完成之后建议关闭IAP 功能,不需要每次都关
}//------------------------------------------------------------------------------

void IAP_Disable()
{
    //关闭IAP 功能, 清相关的特殊功能寄存器,使CPU 处于安全状态,
    //一次连续的IAP 操作完成之后建议关闭IAP 功能,不需要每次都关
    IAP_CONTR = 0;      //关闭IAP 功能
    IAP_CMD   = 0;      //清命令寄存器,使命令寄存器无命令,此句可不用
    IAP_TRIG = 0;      //清命令触发寄存器,使命令触发寄存器无触发,此句可不用
    IAP_ADDRH = 0;
    IAP_ADDRL = 0;
}//------------------------------------------------------------------------------
void delayms(u16 z)
{
u16 x,y;
for(x=z;x>0;x--)
   for(y=125;y>0;y--);
}//-------------------------------------------------------------------------------
