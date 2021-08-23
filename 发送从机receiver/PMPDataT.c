
#include "reg52.h"    //此文件中定义了单片机的一些特殊功能寄存器
#include <string.h>
#include <intrins.h>
#include "lcd.h"
#include "ds1302.h"
#include "temp.h"
#include "ctrl.h"
#include "rc522.h"
#include "uart.h"
#include "EEPROM.h"
#include "main.h"

#define u16 unsigned int
#define u8 unsigned char

u8 a[]="time:";
u8 b[]="temperature:";

u8 KeyValue;
u8 timeset_buf[] ="000000000000";
int tempdata[5]={0,0,0,0};
u8 dater_buf[] = "2000.01.01";
u8 timer_buf[] = "00:00:00";
u8 dateset[]="xxxxxx";								
u8 timeset[]="xxxxxx";
u8 clock1[7];
u16 j=0;
u8 k=0;
u8 i=0;
u8 m=0;
u8 n=0;

bit status=0;
bit flag_fail=0;	   //错误状态
bit flag_settime=0;  //修改时间状态
bit flag_tempdisp=0; //温度显示状态
bit flag_clock=0;
bit clock1_on=0;
bit ds18b20_init_success=0;

void delay_main(u16 i)  //当i=1，大约延时10us
{
	while(i--);
}

void delay(u16 i)
{
  	while(i--);
}
void delay_ms_main(u16 t)
{
  	u16 i;
	while(t--)
	{
		/* 对于11.0592M时钟，约延时1ms */
		for (i=0;i<125;i++);
	}
}

void led_blink_times(u8 j)
{
  u8 i=0;
  for(i=0;i<j;i++)
  {
	LED=0;
	delay(5000);
    LED=1;
	delay(3000);
  }
}

void Datahandle(void) 	 
{
  	Ds1302ReadTime();
	dater_buf[2] = (TIME[6]/16)+48;				//年
	dater_buf[3] = (TIME[6]&0x0f)+48;
	dater_buf[5] = (TIME[4]/16)+48;				//月
	dater_buf[6] = (TIME[4]&0x0f)+48;
	dater_buf[8] = (TIME[3]/16)+48;				//日
	dater_buf[9] = (TIME[3]&0x0f)+48;	
	timer_buf[0] = (TIME[2]/16)+48;				//时
	timer_buf[1] = (TIME[2]&0x0f)+48;				 
	timer_buf[3] = (TIME[1]/16)+48;				//分
	timer_buf[4] = (TIME[1]&0x0f)+48;	
	timer_buf[6] = (TIME[0]/16)+48;				//秒
	timer_buf[7] = (TIME[0]&0x0f)+48;
	if(timer_buf[0]==clock1[0]&&timer_buf[1]==clock1[1]&&timer_buf[3]==clock1[2]&&timer_buf[4]==clock1[3]&&clock1[4]==1)
	{
	  clock1_on=1;
	}
}

void temppros(int temp) 	 
{
		tempdata[0] = (temp % 10000 / 1000)+48;
		tempdata[1] = (temp % 1000 / 100)+48;
		tempdata[2] = (temp % 100 / 10)+48;
		tempdata[3] = (temp % 10)+48;
}

void EEPROM_init()
{
   EEPROM_READ(0,clock1,7);
   EEPROM_READ(1,KEY_BUF,8);
   EEPROM_READ(2,Card_saved_asc,128);
}


void keypros()
{
	if(KeyValue==0||KeyValue==1||KeyValue==2||KeyValue==3)  
	{	
		LCD_write_command(0x01);
		delay(1000);
	}//清屏
	//温度显示
	if(KeyValue>=4&&KeyValue<=7&&flag_settime==1)//修改时间
	{
	   switch(KeyValue)
	   {
	   case 4:
	    if(timeset_buf[i]>='0'&&timeset_buf[i]<='9')
	    {
		 timeset_buf[i]=timeset_buf[i]+1; 
		if((i>=0&&i<=5)||i==9||i==11)
		{
			if(timeset_buf[i]=='9'+1)
			{
         	timeset_buf[i] -=10;
			}
		}
		if(i==6)
		{
		    if(timeset_buf[i]=='2'+1)
			{
         	timeset_buf[i] -=3;
			}
		}
		if(i==7&&timeset_buf[6]=='2')
		{
		    if(timeset_buf[i]=='3'+1)
			{
         	timeset_buf[i] -=4;
			}
		}
		if(i==7&&timeset_buf[6]=='1'||timeset_buf[6]=='0')
		{
		    if(timeset_buf[i]=='9'+1)
			{
         	timeset_buf[i] -=10;
			}
		}
		if(i==8||i==10)
		{
		    if(timeset_buf[i]=='5'+1)
			{
         	timeset_buf[i] -=6;
			}
		}
	}
	   break;
	   case 5:
	    if(timeset_buf[i]>='0'&&timeset_buf[i]<='9')
	    {
		 timeset_buf[i]=timeset_buf[i]-1; 
		if((i>=0&&i<=5)||i==9||i==11)
		{
			if(timeset_buf[i]=='0'-1)
			{
         	timeset_buf[i] +=10;
			}
		}
		if(i==6)
		{
		    if(timeset_buf[i]=='0'-1)
			{
         	timeset_buf[i] +=3;
			}
		}
		if(i==7&&timeset_buf[6]=='1'||timeset_buf[6]=='0')
		{
		    if(timeset_buf[i]=='0'-1)
			{
         	timeset_buf[i] +=10;
			}
		}
		if(i==7&&timeset_buf[6]=='2')
		{
		    if(timeset_buf[i]=='0'-1)
			{
         	timeset_buf[i] +=4;
			}
		}
		if(i==8||i==10)
		{
		    if(timeset_buf[i]=='0'-1)
			{
         	timeset_buf[i] +=6;
			}
		}
	}
	   break;
	   case 6:
	   i++;break;
	   case 7:
	   	if(i==0)
	   {
		break;
	   }
	   else
	   {
        i--;
		break;
	   }
	   }
	   if(i<6)
		{
			dateset[i]=timeset_buf[i];
		}
		if(i>=6)
		{
			timeset[i-6]=timeset_buf[i];
		}
		if(i==12)
		{
			i=0;
			for(m=0;m<12;m++)
			{
			  timeset_buf[m]-=48;
			}	 
			TIME[6]=(timeset_buf[0]*10+timeset_buf[1])+((timeset_buf[0]*10+timeset_buf[1])/10)*6;     //年
			TIME[4]=(timeset_buf[2]*10+timeset_buf[3])+((timeset_buf[2]*10+timeset_buf[3])/10)*6;     //月
			TIME[3]=(timeset_buf[4]*10+timeset_buf[5])+((timeset_buf[4]*10+timeset_buf[5])/10)*6;     //日
			TIME[2]=(timeset_buf[6]*10+timeset_buf[7])+((timeset_buf[6]*10+timeset_buf[7])/10)*6;     //时 
			TIME[1]=(timeset_buf[8]*10+timeset_buf[9])+((timeset_buf[8]*10+timeset_buf[9])/10)*6;     //分
			TIME[0]=(timeset_buf[10]*10+timeset_buf[11])+((timeset_buf[10]*10+timeset_buf[11])/10)*6; //秒
			for(m=0;m<12;m++)
			{
			  timeset_buf[m]+=48;
			}
			Ds1302Init();
			status=0;
			flag_settime=0;
		}
		LCD_write_command(0x01);
		delay(1000);
		LCD_write_command(0x0c);
		delay(1000);
	}
	if(flag_clock==1)
	{
	  switch(KeyValue)
	  {
		case 4:
	    if(clock1[i]>='0'&&clock1[i]<='9'&&i==3)
	    {
		 clock1[i]=clock1[i]+1;
		} 
		if(clock1[i]=='9'+1)
		{
         clock1[i] -=10;
		}
		if(i==1)
		{
		 if(clock1[0]=='0'||clock1[0]=='1')
		 {
		  if(clock1[i]>='0'&&clock1[i]<='9')
		  {
			clock1[i]++;
		  }
		  if(clock1[i]=='9'+1)
		  {
		   clock1[i]-=10;
		  }
		 }
		 if(clock1 [0]=='2')
		 {
		  if(clock1[i]>='0'&&clock1[i]<='3')
		  {
			clock1[i]++;
		  }
		  if(clock1[i]=='3'+1)
		  {
		   clock1[i]-=4;
		  }
		 }
		}
		if(i==0)
		{
		  if(clock1[i]>='0'&&clock1[i]<='2')
		  {
			clock1[i]++;
		  }
		  if(clock1[i]=='2'+1)
		  {
		   clock1[i]-=3;
		  }
		}
		if(i==2)
		{
		  if(clock1[i]>='0'&&clock1[i]<='5')
		  {
			clock1[i]++;
		  }
		  if(clock1[i]=='5'+1)
		  {
		   clock1[i]-=6;
		  }

		}
		if(i==4)
		{
		  clock1[4]=1;
		}
		break;
		case 5:
	    if(clock1[i]>='0'&&clock1[i]<='9'&&i==3)
	    {
		 clock1[i]=clock1[i]-1;
		} 
		if(clock1[i]=='0'-1)
		{
         clock1[i] +=10;
		}
		if(i==1)
		{
		 if(clock1[0]=='1'||clock1[0]=='0')
		 {
		   if(clock1[i]>='0'&&clock1[i]<='9')
		  {
			clock1[i]--;
		  }
		  if(clock1[i]=='0'-1)
		  {
		   clock1[i]+=10;
		  }
		 }
		 if(clock1[0]=='2')
		 { 
		  if(clock1[i]>='0'&&clock1[i]<='3')
		  {
			clock1[i]--;
		  }
		  if(clock1[i]=='0'-1)
		  {
		   clock1[i]+=4;
		  }
		 }
		}
		if(i==0)
		{
		  if(clock1[i]>='0'&&clock1[i]<='2')
		  {
			clock1[i]--;
		  }
		  if(clock1[i]=='0'-1)
		  {
		   clock1[i]+=3;
		  }
		}
	     if(i==2)
		{
		  if(clock1[i]>='0'&&clock1[i]<='5')
		  {
			clock1[i]--;
		  }
		  if(clock1[i]=='0'-1)
		  {
		   clock1[i]+=6;
		  }
		}
		if(i==4)
		{
		  clock1[4]=0;
		}
		break;
		case 6:
		i++;
		break;
		case 7:
	    if(i==0)
	   {
		break;
	   }
	   else
	   {
        i--;
		break;
	   }
	 }
		if(i==5)
		{
		 status=0;
		 flag_clock=0;
		 i=0;
		 EEPROM_WRITE(0,clock1,5);
        }
		LCD_write_command(0x01);
		delay(1000);
		LCD_write_command(0x0c);
		delay(1000);
	}
}
 
void KeyDown(void)
{
	char a=0;
	if(SETTIME_MODE!=1)
	{
	  delay(1000);
	  if(SETTIME_MODE!=1)
	  {
	    led_blink_times(1);
	    KeyValue=0;
		if(status==1)
		{
         KeyValue+=4;
		} //修改时间状态
		if(status==0)
		{
		   flag_settime=1;
		   status=1;
		}

	    keypros();
	   		while((a<70)&&(SETTIME_MODE!=1))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}	
	  }
	}
	if(TEMPER_MODE!=1)
	{
	  delay(1000);
	  if(TEMPER_MODE!=1)
	  {
	    led_blink_times(1);
	    KeyValue=1;
		if(status==1)
		{
         KeyValue+=4;
		}
		if(status==0)
		{
		   flag_tempdisp=1;
		   status=1;
		}//温度显示状态

		 keypros();
	   			while((a<70)&&(TEMPER_MODE!=1))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}	
	  }
	}
	if(RC522_MODE!=1)
	{
	  delay(1000);
	  if(RC522_MODE!=1)
	  {
	   led_blink_times(1);
	   KeyValue=2;
	   	if(status==1)
		{
         KeyValue+=4;
		}
	   if(status==0)
		{  
		   rc522_state=1;
		   status=1;
		}
		 keypros();
	   			while((a<70)&&(RC522_MODE!=1))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}	
	  }
	}
	if(CLOCK_SET!=1)
	{
	  delay(1000);
	  if(CLOCK_SET!=1)
	  {
	    led_blink_times(1);
	    KeyValue=3;
		if(status==1)
		{
         KeyValue+=4;
		}
		 if(status==0)
		{
		   flag_clock=1;
		   status=1;
		}
		 keypros();
					while((a<70)&&(CLOCK_SET!=1))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}	
	  }
	}
	
}

void display()
{
   if(status==0)
   {
	LCD_disp_string(1,1,"Date:");
    LCD_disp_string(1,2,"Time:");
	LCD_disp_string(6,1,dater_buf);
	LCD_disp_string(7,2,timer_buf);
	if(clock1_on==1)
	 {
	   clock1_on=0;
	   LCD_write_command(0x01);
	   delay_ms_main(100);
	   LCD_disp_string(1,1,"clock1 is on!");
	   send_byte(0x02);
	   delay_ms_main(5000);
	 }
   }
   if(status==1&&rc522_state==0)
   {
	 if(flag_settime==1)
	 {
	   	LCD_disp_string(6,1,"20");
		LCD_disp_string(1,1,"Date:");
	    LCD_disp_string(1,2,"Time:");
		LCD_disp_char(8,1,dateset[0]);
		LCD_disp_char(9,1,dateset[1]);
		LCD_disp_char(10,1,'.');
		LCD_disp_char(11,1,dateset[2]);
		LCD_disp_char(12,1,dateset[3]);
		LCD_disp_char(13,1,'.');
		LCD_disp_char(14,1,dateset[4]);
		LCD_disp_char(15,1,dateset[5]);
		LCD_disp_char(7,2,timeset[0]);
		LCD_disp_char(8,2,timeset[1]);
	    LCD_disp_char(9,2,':');
		LCD_disp_char(10,2,timeset[2]);
		LCD_disp_char(11,2,timeset[3]);
		LCD_disp_char(12,2,':');
		LCD_disp_char(13,2,timeset[4]);
		LCD_disp_char(14,2,timeset[5]);
		for(j=0;SETTIME_MODE==1&&TEMPER_MODE==1&&RC522_MODE==1&&CLOCK_SET==1;j++)
        {
		  	if(i>=0&&i<=1)
	        {
	    	LCD_write_command(0x80 + i+ 8 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}
			if(i>=2&&i<=3)
	        {
	    	LCD_write_command(0x80 + i+ 9 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}
			if(i>=4&&i<=5)
	        {
	    	LCD_write_command(0x80 + i+ 10 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}	//光标闪烁指令
			if(i>=6&&i<=7)
			{
		  	LCD_write_command(0xc0 + i+1 ) ;	//指定光标位置
	      	LCD_write_command(0x0f) ;
			}
			if(i>=8&&i<=9)
			{
		  	LCD_write_command(0xc0 + i+2 ) ;	//指定光标位置
	      	LCD_write_command(0x0f) ;
			}
			if(i>=10&&i<=11)
			{
		  	LCD_write_command(0xc0 + i+3 ) ;	//指定光标位置
	      	LCD_write_command(0x0f) ;
			}
		}
	 }
	 else if(flag_tempdisp==1)
	 {
	  if(ds18b20_init_success==0)
	  {
	  LCD_disp_string(1,1,"init failed!");
	  Delay1ms(3000);
	  flag_tempdisp=0;
	  status=0;
	  LCD_write_command(0x01);
	  }
	  else
	  {
	  temppros(Ds18b20ReadTemp());
	  LCD_disp_string(1,1," temperature:");
 	  LCD_disp_char(2,2,tempdata[0]);
	  LCD_disp_char(3,2,tempdata[1]);
	  LCD_disp_char(4,2,tempdata[2]);
	  LCD_disp_char(5,2,'.');
	  LCD_disp_char(6,2,tempdata[3]);
	  LCD_disp_char(7,2,'C');
	  Delay1ms(3000);
	  flag_tempdisp=0;
	  status=0;
	  LCD_write_command(0x01);//清屏
	  }
	 }
	 else if(flag_clock==1)
	 {
	   LCD_disp_string(1,1,"clock1|");
	   LCD_disp_char(8,1,clock1[0]);
	   LCD_disp_char(9,1,clock1[1]);
	   LCD_disp_char(10,1,':');
	   LCD_disp_char(11,1,clock1[2]);
	   LCD_disp_char(12,1,clock1[3]);
	   LCD_disp_string(13,1,"| ");
	   if(clock1[4]==1)
	   {
	   LCD_disp_char(15,1,'1');
	   }
	   else if(clock1[4]==0)
	   {
	   LCD_disp_char(15,1,'0');
	   } 
	   for(j=0;SETTIME_MODE==1&&TEMPER_MODE==1&&RC522_MODE==1&&CLOCK_SET==1;j++)
        {
		  	if(i>=0&&i<=1)
	        {
	    	LCD_write_command(0x80 + i+ 8 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}
			if(i>=2&&i<=3)
	        {
	    	LCD_write_command(0x80 + i+ 9 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}	//光标闪烁指令
			if(i==4)
	        {
	    	LCD_write_command(0x80 + i+ 11 ) ;	//指定光标位置
	    	LCD_write_command(0x0f) ;
			}	//光标闪烁指令
		}
	 }
	 else if(clock1_on==1)
	 {
	   LCD_write_command(0x01);
	   delay_ms_main(100);
	   LCD_disp_string(1,1,"clock1 is on!");
	   send_byte(0x02);
	   Delay1ms(5000*12);
	   status=0;
	   flag_clock=0;
	   flag_tempdisp=0;
	   flag_settime=0;
	   clock1_on=0;
	 }
   }
   
}

void recv_from_host() interrupt	4
{
 	u8 Data;
	Data=SBUF;
	if(RI)
	{
	 	if(Data==0x01)
    	{
		rc522_state=1;
		status=1;
		flag_settime=0;  //修改时间状态
	    flag_tempdisp=0; //温度显示状态
		flag_clock=0;
		LCD_write_command(0x01);
		delay_ms_main(100);
		}
		RI=0;
	}
}

void main()
{  
   EA = 0;
   LCD_init();
   Uart_Init();
   EEPROM_init();  //串口初始化
   init_rc522();
   ds18b20_init_success=Ds18b20Init(); //初始化Ds18b20
   Ds18b20ReadTemp();
   EA = 1;
   ES = 1;
	while(1)
	{   
        KeyDown();
	   	while(rc522_state)
		{
		  RC522_KeyValue=4;
		  RC522_KeyDown();
		  ctrl_process();
		} 
	  Datahandle();
	 display(); 
	}
}