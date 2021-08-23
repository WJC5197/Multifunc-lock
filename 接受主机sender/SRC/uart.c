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
description:ͨ�����ڷ�������sentData
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
���ڷ��͸�ʽ��fe  cmdlen  datalen cxx cxx dxx dxx dxx ... check
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
	TL2 = 0xfa;     //������ֵ���ò�����57600bps
	TH2 = 0xff;     //װ�ع̶���ֵ
	RCAP2L = 0xfa;
	RCAP2H = 0xff;
    SCON = 0x50; 	//���ڷ�ʽ1,1����ʼλ,1��ֹͣλ,8λ���ݣ��ɱ䲨����
	PCON = 0X00;	//�����ʲ��ӱ�
	TR2 = 1;
	ES = 0;			//�رմ����ж�
	EA = 1;			//�������ж�
}

void Uart_Init_Slave(void)
{
   	T2MOD = 0x01;
	T2CON = 0x30;
	TL2 = 0xdc;     //������ֵ���ò�����9600bps
	TH2 = 0xff;     //װ�ع̶���ֵ
	RCAP2L = 0xdc;
	RCAP2H = 0xff;
    SCON = 0xd0;
	PCON = 0x80;	
	TR2 = 1;
	ES = 0;		    //�رմ����ж�
	EA = 1;			//�������ж�
}

void send(u8 Data)     //�������ݳ���
{  
   TI=0;           //�����λTI
   TB8=0;          //��������֡
   SBUF=Data;      //��������
   while(!TI);
   TI=0;
}

void Uart_Send_Byte(unsigned char c)//UART Send a byte
{
	SBUF = c;
	while(!TI);		//������Ϊ1 
	TI = 0;
}

unsigned char Uart_Receive_Byte()//UART Receive a byteg
{	
	unsigned char dat;
	while(!RI);	 //������Ϊ1 
	RI = 0;
	dat = SBUF;
	return (dat);
}
//˫����������
u8 Receive(void) //��������
{
   u8 Data;
   while(!RI); 
   Data=SBUF;
   RI=0;   //�ȴ��������
   return Data;
}

