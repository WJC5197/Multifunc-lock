#ifndef  __uart_h__
#define  __uart_h__

#define	UART_MAX_LEN	50
#define u8 unsigned char
#define u16 unsigned int

extern u8 data UartBuf[UART_MAX_LEN];

extern u8 UartCount, UartLen, UartStart, UartComp;

void send_byte(u8  SendData);
void send_bytes( u8 *Buf, u8 Len );
void Uart_Init(void);
u8 Receive(void);
void send(u8 Data);
#endif
