#define u8 unsigned char
#define u16 unsigned int

#define	FOSC	22118400L//11059200L
#define	BAUD	9600

#define	FALSE	0
#define	TRUE	1

#define	WR		0
#define	RD		1

#define nop() _nop_()

#define	BIT(n)	( 1 << n )
#define	LED_NONE	0
#define	LED_LONG	1
#define	LED_200MS	2
#define	LED_80MS	3

#include "main.h"
#include "reg52.h"

sbit SETTIME_MODE=P0^0;
sbit TEMPER_MODE=P0^1;
sbit RC522_MODE=P0^2;
sbit CLOCK_SET=P0^3;
sbit LED=P0^4;

bit status=0;

/******************
UartCmdLen：UartCmd + UartErrCode
UartDataLen：UartDataBuf
*******************/

typedef struct __sUartData
{
	u8 UartCmdLen;  //命令长度
	u8 UartDataLen; //数据长度 
	u16 UartCmd;
	u8 UartErrCode;
	u8  UartDataBuf[1];
} *psUartData;

