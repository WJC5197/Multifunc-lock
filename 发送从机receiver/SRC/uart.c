#include "reg52.h"
#include "uart.h"
#include "main.h"
#include "timer.h"
u8 data UartBuf[UART_MAX_LEN] = { 0 };
u8 UartCount = 0, UartLen = 0, UartStart = FALSE, UartComp = FALSE;


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
/*void uart_interrupt_receive(void) interrupt 4
{
	u8 ReByte;
	if( RI == 1 )
	{
		RI = 0;
		UartWaitTime = 0;
		ReByte = SBUF;

		//send_byte( ReByte );
		
		if( ( UartStart == FALSE ) && ( ReByte == 0xFE ) ) //FE为起始码
		{
			UartStart = TRUE;
			UartCount = 0;
			UartLen = 255;
		}

		else if( UartStart == TRUE )  //已经开始了接收数据
		{
			UartBuf[ UartCount++ ] = ReByte;
			if( UartCount == 2 )
			{
				UartLen = UartBuf[0] + UartBuf[1] + 3;
			}
			else if( UartCount >= UartLen )
			{
				UartComp = TRUE;
			}
			if( UartCount >= 45 )
			{
				UartCount = 45;
			}
		}
	}
} */

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Disabled


void Uart_Init(void)
{
    T2MOD = 0x01;
	T2CON = 0x30;
	TL2 = 0xdc;//计数初值设置波特率9600bps
	TH2 = 0xff;//装载固定初值
	RCAP2L = 0xdc;
	RCAP2H = 0xff;
    SCON = 0xd0;
	PCON = 0x80;	
	TR2 = 1;
	ES = 0;
	EA = 1;
	REN= 1;
}


