#ifndef  __timer_h__
#define  __timer_h__
#define u16 unsigned int
#define u8 unsigned char

extern u16  UartWaitTime, RequestCardTime;	
extern void init_timer(void);  //20ms
extern void timer_fast( void );
extern void timer_slow( void );
extern void Delay_ms(int ms);

#endif