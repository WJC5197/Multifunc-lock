#include<reg52.h>
#include"StepperMotorControl.h"
#include "sbit.h"

//------------------------------------------------------------------------------------------------------
//名称：变量延时函数
//作用：通过改变var的值，改变延时时间
//------------------------------------------------------------------------------------------------------
void delay_var(u16 var)
{
    u16 i,j;
	 for(i=0;i<var;i++)
       for(j=0;j<110;j++);
}

//------------------------------------------------------------------------------------------------------
//名称：步进电机停止
//作用：停止
//------------------------------------------------------------------------------------------------------
void MotorStop(void)
{
    Motor_A=0;
	Motor_B=0;
	Motor_C=0;
	Motor_D=0;
}
//------------------------------------------------------------------------------------------------------
//名称：步进电机驱动函数
//作用：通过变量var控制电动机的转速高低，通过变量state判断电动机的正反转
//------------------------------------------------------------------------------------------------------
void MotorAccOrDec(u16 var,u8 state)
{
	if(!state)
	{
     Motor_A=0;
	 Motor_B=0;
	 Motor_C=0;
	 Motor_D=1;
	 delay_var(var);
	 Motor_A=0;
	 Motor_B=0;
	 Motor_C=1;
	 Motor_D=1;
	 delay_var(var);
	 Motor_A=0;
	 Motor_B=0;
	 Motor_C=1;
	 Motor_D=0;
	 delay_var(var);
	 Motor_A=0;
	 Motor_B=1;
	 Motor_C=1;
	 Motor_D=0;
	 delay_var(var);
	 Motor_A=0;
	 Motor_B=1;
	 Motor_C=0;
	 Motor_D=0;
	 delay_var(var);
	 Motor_A=1;
	 Motor_B=1;
	 Motor_C=0;
	 Motor_D=0;
	 delay_var(var);
	 Motor_A=1;
	 Motor_B=0;
	 Motor_C=0;
	 Motor_D=0;
	 delay_var(var);
	 Motor_A=1;
	 Motor_B=0;
	 Motor_C=0;
	 Motor_D=1;
	 delay_var(var);
	}
	else
	{	 
		 Motor_A=1;
	 	 Motor_B=0;
		 Motor_C=0;
	 	 Motor_D=1;
	 	 delay_var(var); 
	     Motor_A=1;
	     Motor_B=1;
	     Motor_C=0;
	     Motor_D=0;
	     delay_var(var);
	  	 Motor_A=0;
	     Motor_B=1;
	     Motor_C=0;
	     Motor_D=0;
	     delay_var(var);
		  Motor_A=0;
	     Motor_B=1;
	     Motor_C=1;
	     Motor_D=0;
	     delay_var(var);
		  Motor_A=0;
	     Motor_B=0;
	     Motor_C=1;
	     Motor_D=0;
	     delay_var(var);
		  Motor_A=0;
	     Motor_B=0;
	     Motor_C=1;
	     Motor_D=1;
	     delay_var(var);
		  Motor_A=0;
	     Motor_B=0;
	     Motor_C=0;
	     Motor_D=1;
	     delay_var(var);
 	}
}

