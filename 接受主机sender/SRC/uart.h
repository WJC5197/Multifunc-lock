#ifndef  __uart_h__
#define  __uart_h__

#define	UART_MAX_LEN	50
#define u8 unsigned char
#define u16 unsigned int

extern u8 data UartBuf[UART_MAX_LEN];

extern u8 UartCount, UartLen, UartStart, UartComp;

extern void send_byte(u8  SendData);
extern void send_bytes( u8 *Buf, u8 Len );
extern void Uart_Init_As608(void);
extern void Uart_Init_Slave(void);
extern void send(u8 Data);
extern u8 recv_data(void);
extern void recv_from_host(void);
extern void Uart_Send_Byte(u8 c);
extern u8 Uart_Receive_Byte();
extern void send_addr(u8 addr);
extern void send_data(u8 ddata);

#endif
