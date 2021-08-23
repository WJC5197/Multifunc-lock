#include "reg52.h"
#include "uart.h"
#include "sbit.h"
#define TRUE 1
#define FALSE 0
#define u8 unsigned char
#define u16 unsigned int

u8 data UartBuf[UART_MAX_LEN] = { 0 };
u8 UartCount = 0, UartLen = 0, UartStart = FALSE, UartComp = FALSE;
u16 UartWaitTime = 0;

/**********************************************************************
functionName:putChar(BYTE sentData)
description:通过串口发送数据sentData
**********************************************************************/
void send_byte( u8 SendData )
{
  	ES = 0;
  	TI = 0;
  	SBUF = SendData;
  	while( TI == 0 );
  	TI = 0;
  	ES = 1;
}


void send_bytes( u8 *Buf, u8 Len )
{
	while( Len-- )
	{
		send_byte( *Buf++ );	
	}
}


/**********************************************************************
串口发送格式：fe  cmdlen  datalen cxx cxx dxx dxx dxx ... check
**********************************************************************/

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Disabled

void Uart_Init_As608(void)
{
   	T2MOD = 0x01;
	T2CON = 0x30;
	TL2 = 0xfa;     //计数初值设置波特率57600bps
	TH2 = 0xff;     //装载固定初值
	RCAP2L = 0xfa;
	RCAP2H = 0xff;
    SCON = 0x50; 	//串口方式1,1个起始位,1个停止位,8位数据，可变波特率
	PCON = 0X00;	//波特率不加倍
	TR2 = 1;
	ES = 0;			//关闭串口中断
	EA = 1;			//开启总中断
}

void Uart_Init_Slave(void)
{
   	T2MOD = 0x01;
	T2CON = 0x30;
	TL2 = 0xdc;     //计数初值设置波特率9600bps
	TH2 = 0xff;     //装载固定初值
	RCAP2L = 0xdc;
	RCAP2H = 0xff;
    SCON = 0xd0;
	PCON = 0x80;	
	TR2 = 1;
	ES = 0;		    //关闭串口中断
	EA = 1;			//开启总中断
}

void send(u8 Data)     //发送数据程序
{  
   TI=0;           //软件复位TI
   TB8=0;          //发送数据帧
   SBUF=Data;      //发送数据
   while(!TI);
   TI=0;
}

void Uart_Send_Byte(unsigned char c)//UART Send a byte
{
	SBUF = c;
	while(!TI);		//发送完为1 
	TI = 0;
}

unsigned char Uart_Receive_Byte()//UART Receive a byteg
{	
	unsigned char dat;
	while(!RI);	 //接收完为1 
	RI = 0;
	dat = SBUF;
	return (dat);
}
//双机交互函数
u8 Receive(void) //接收数据
{
   u8 Data;
   while(!RI); 
   Data=SBUF;
   RI=0;   //等待接受完毕
   return Data;
}

