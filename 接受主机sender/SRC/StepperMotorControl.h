//------------------------------------------------------------------------------------------------------
//���ƣ������������ģ��ͷ�ļ�
//���ã����������ͺ���������ʵ��ģ�黯
//------------------------------------------------------------------------------------------------------
#ifndef STEPPERMOTORCONTROL_H
#define STEPPERMOTORCONTROL_H
#define u16 unsigned int
#define u8 unsigned char
sbit Motor_A=P0^6;
sbit Motor_B=P0^7;
sbit Motor_C=P2^7;
sbit Motor_D=P2^6;
void delay_var(u16 var);
void MotorStart(void);
void MotorStop(void);
void MotorAccOrDec(u16 var,u8 state);

#endif
