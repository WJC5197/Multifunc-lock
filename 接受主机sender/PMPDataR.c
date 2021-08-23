
#include "reg52.h" 
#include "bmp.h"
#include "oled.h"
#include "steppermotorcontrol.h"
#include "PlayMusic.h"
#include "EEPROM.h"
#include "sbit.h"
#include "uart.h"
#include "fpm10a.h"
#include <string.h>
#include<absacc.h>

#define u16 unsigned int
#define u8 unsigned char

//可显示的元件

bit set_password=0;
bit use_password=0;
bit set_music=0;
bit use_rfid=0;//接受来自从机的bit
bit password_visble=0;
bit password_right=0;
bit visble_justnow=0;
bit visble_change_near=0;
bit row_1=0;
bit row_2=0;
bit flag_open=0;
bit clock_play_music = 0;

u8 song1;
u8 song2;
u8 song3;
u8 song4;
u8 song5;

u8 n;
u8 password[32];
u8* p=password;
u8 input[32];
u8 input_buf[32]="********************************";
u8 i=0;
u8 key_value;	//用来存放读取到的键值
u8 column_temp;
char i_temp=-1;//timevar是时间变量控制延时时间,
u8 row=0;
u16 a=0;
u16 time_var=2;
u8 song_set[5];
u8* p_clock=song_set;
               
extern void delay_main(u16 t)
{
	u16 i;
	while(t--)
	{
		/* 对于11.0592M时钟，约延时1ms */
		for (i=0;i<125;i++)
		{}
	}
}

void main_Buzz_Times(u8 k)
{ 
 u8 i,j;
for(j=0;j<k;j++)
{
  for(i=0;i<100;i++)
 { 
  BeepIO=~BeepIO;
  Delay_Us(100);
 }
 Delay_Ms_fpm(200);
}
BeepIO=1;  
}

void delay(u16 i)
{
	while(i--);	
}
 
void warn(u8 k)
{
 	u8 i,j;
	for(j=0;j<k;j++)
	{
  	for(i=0;i<100;i++)
 	{ 
  	BeepIO=~BeepIO;
 	 Delay_Us(100);
 	}
	}
	BeepIO=1;  
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

void EEPROM_init()
{
   EEPROM_READ(0,p,32);
   EEPROM_READ(1,p_clock,5);
  	    song1=song_set[0];
		song2=song_set[1];
		song3=song_set[2];
		song4=song_set[3];
		song5=song_set[4];
	EEPROM_READ(2,errors,3);
}

void motor_on()
{ 
  if(flag_open==1)
 {
  a=0;
  while(1)
  {
	MotorAccOrDec(time_var,5);
	a++;
	if(a==765)
	{
	  MotorStop();goto k;
	}
  }
k://Play_Music4();
  delay_main(1000);
  a=0;
  while(1)
  {
	MotorAccOrDec(time_var,0);
	a++;
	if(a==765)
	{
	  MotorStop();goto l;
	}
  }
l:
  a=0;
  flag_open=0;
 }
}
void key_pros()
{
  if(status==0&&is_lock==1&&use_password==0&&use_fingerprint==0&&use_rfid==0&&key_value>=1&&key_value<=3)
  {
   switch(key_value)
   {
    case 1:
	use_password=1;
	key_value=16;
	OLED_Clear();
	break;
	case 2:
	use_fingerprint=1;
	key_value=16;
	OLED_Clear();
	break;
	case 3:
	use_rfid=1;
	key_value=16;
	OLED_Clear();
	break;
	default:break;
   }
  }
  if(status==0&&is_lock==1&&use_password==1&&key_value>=0&&key_value<=15)
  {	
	switch(key_value)
	{
	  case 0:if(i==32){break;}else{input[i]='0'+0;i++;break;}
	  case 1:if(i==32){break;}else{input[i]='0'+1;i++;break;}
	  case 2:if(i==32){break;}else{input[i]='0'+2;i++;break;}
	  case 3:if(i==32){break;}else{input[i]='0'+3;i++;break;}
	  case 4:if(i==32){break;}else{input[i]='0'+4;i++;break;}
	  case 5:if(i==32){break;}else{input[i]='0'+5;i++;break;}
	  case 6:if(i==32){break;}else{input[i]='0'+6;i++;break;}
	  case 7:if(i==32){break;}else{input[i]='0'+7;i++;break;}
	  case 8:if(i==32){break;}else{input[i]='0'+8;i++;break;}
	  case 9:if(i==32){break;}else{input[i]='0'+9;i++;break;}
	  case 10:if(i==32){break;}else{input[i]='a';i++;break;}
	  case 11:if(i==32){break;}else{input[i]='b';i++;break;}
	  case 12:if(i>=0)
	  {
	  if(i==16)
	  {
	   row-=1;
	   if(row==0)
	   {
		 row_1=0;
	   }
	  }
	  if(i==32)
	  {
	   row-=1;
	  }
	  if(i==0)
	  {
	  break;
	  }
	  OLED_Clear_Line(5+row);input[i]='\0';i--;
	   break;
	  }
	  case 13:
	   if(password_visble==0&&visble_change_near==0)
	  {
	  password_visble=1;
	  visble_change_near=1;
	  }
	  if(password_visble==1&&visble_change_near==0)
	  {
	  password_visble=0;
	  }
	  break;
	  case 14:
	  if(strlen(input)!=strlen(password))
	  {
	   password_right=0;
	   goto m;
	  }
	  for(i=0;i<strlen(password);i++)//检验密码程序
	  {
		if(input[i]==password[i])
		{
		 password_right=1;
		}
		else
		{
		password_right=0;
		}
		if(password_right==0)
		{
		goto m;
		}
		if(i==strlen(password)&&password_right==1)
		{
		goto n;
		}	 
	  }
	  n:is_lock=0;
	  errors[0]='0';
	  errors[1]='0';
	  EEPROM_WRITE(2,errors,3);
	  open_just_now=1;
	  for(n=0;n<i;n++)
	  {
	    input[n]='\0';
	  }
	  i=0;
	  row=0;
	  break;
	  m:OLED_Clear();
	  OLED_ShowString(1,0,"Password Error!",18);
	  main_Buzz_Times(5);
	  errors[0]++;
	  EEPROM_WRITE(2,errors,3);
	  if(errors[0]=='3')
	  {
		errors[2]=1;
		EEPROM_WRITE(2,errors,3);
	  }
	  OLED_ShowString(1,2,"Error times:",18);
	  OLED_ShowChar(100,2,errors[0],18);
	  led_blink_times(3);
	  for(n=0;n<i;n++)
	  {
	   input[n]='\0';
	  }
	  row_1=0;
	  row_2=0;
	  use_password=0;
	  row=0;
	  i=0;
	  delay_main(500);
	  OLED_Clear(); 
	  break;
	  case 15:
	  for(n=0;n<i;n++)
	  {
	   input[n]='\0';
	  }
	  row_1=0;
	  row_2=0;
	  use_password=0;
	  row=0;
	  i=0;
	  OLED_Clear();
	  break;
	  default:break;
	}
  }
  if(status==1&&is_lock==0&&key_value>=0&&key_value<=15&&set_music==0)
  {	
	switch(key_value)
	{
	  case 0:if(i==32){break;}else{input[i]='0'+0;i++;break;}
	  case 1:if(i==32){break;}else{input[i]='0'+1;i++;break;}
	  case 2:if(i==32){break;}else{input[i]='0'+2;i++;break;}
	  case 3:if(i==32){break;}else{input[i]='0'+3;i++;break;}
	  case 4:if(i==32){break;}else{input[i]='0'+4;i++;break;}
	  case 5:if(i==32){break;}else{input[i]='0'+5;i++;break;}
	  case 6:if(i==32){break;}else{input[i]='0'+6;i++;break;}
	  case 7:if(i==32){break;}else{input[i]='0'+7;i++;break;}
	  case 8:if(i==32){break;}else{input[i]='0'+8;i++;break;}
	  case 9:if(i==32){break;}else{input[i]='0'+9;i++;break;}
	  case 10:if(i==32){break;}else{input[i]='a';i++;break;}
	  case 11:if(i==32){break;}else{input[i]='b';i++;break;}
	  case 12:if(i>=0)
	  {
	  if(i==16)
	  {
	   row-=1;
	   if(row==0)
	   {
		 row_1=0;
	   }
	  }
	  if(i==32)
	  {
	   row-=1;
	  }
	  if(i==0)
	  {
	  break;
	  }
	  OLED_Clear_Line(5+row);input[i]='\0';i--;
	   break;
	  }
	  case 13:
	   if(password_visble==0&&visble_change_near==0)
	  {
	  password_visble=1;
	  visble_change_near=1;
	  }
	  if(password_visble==1&&visble_change_near==0)
	  {
	  password_visble=0;
	  }
	  break;
	  case 14:
	  for(n=0;n<strlen(password);n++)
	  {
		password[n]='\0';
	  }
      for(n=0;n<strlen(input);n++)
	  {
		password[n]=input[n];
	  }
	  for(n=0;n<i;n++)
	  {
	   input[n]='\0';
	  }
	  EEPROM_WRITE(0,password,32);
	  status=0;
	  set_password=0;
	  row_1=0;
	  row_2=0;
	  row=0;
	  i=0;
	  OLED_Clear();
	  OLED_ShowString(24,3,"Reset",36);
	  OLED_ShowString(24,4,"Successfully",36);
	  led_blink_times(2);
	  delay_main(500);
	  OLED_Clear();
	  break;
	  case 15:
	  for(n=0;n<i;n++)
	  {
	   input[n]='\0';
	  }
	  row_1=0;
	  row_2=0;
	  status=0;
	  set_password=0;
	  row=0;
	  i=0;
	  OLED_Clear();
	  break;
	  default:break;
	}
  }
  if(status==0&&is_lock==0&&key_value>=1&&key_value<=4)
  {
	switch(key_value)
	{
	 case 1:
	 status=1;
	 set_password=1;
	 OLED_Clear();
	 break;
	 case 2:
	 status=1;
	 set_fingerprint=1;
	 OLED_Clear();
	 break;
	 case 3:
	 status=1;
	 set_music=1;
	 OLED_Clear();
	 break;
	 case 4:
	 status=0;
	 is_lock=1;
	 OLED_Clear();
	 OLED_ShowString(24,3,"Lock",36);
	 OLED_ShowString(24,4,"Successfully!",36);
	 Buzz_Times(2);
	 led_blink_times(3);
	 delay_main(500);
	 OLED_Clear();
	 break;
	}
  }
}
void keydown(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			led_blink_times(1);
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	key_value=0;break;
				case(0X0b):	key_value=1;break;
				case(0X0d): key_value=2;break;
				case(0X0e):	key_value=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0Xe0):	key_value=key_value;break;
				case(0Xd0):	key_value=key_value+4;break;
				case(0Xb0): key_value=key_value+8;break;
				case(0X70):	key_value=key_value+12;break;
			}
		}
		if(use_rfid==0)
		{
			if(set_fingerprint==0)		
		   {	
			key_pros();
		   }
		}
	}
	while((a<75)&&(GPIO_KEY!=0XF0))	 //检测按键松手检测
	{
		delay(100);
		a++;
	}
}


void display_pros()
{			
   if(errors[2]==1)
   {
     EA = 0;
	 OLED_ShowString(0,0,"Entry Denial!",30);
	 OLED_ShowString(0,2,"Please try after 1 minute.",30);
	 warn(20);
	 delay_main(1000*3);
	 OLED_Clear();
	 if(errors[0]=='3')
	 {
		 errors[0]='0';
		 EEPROM_WRITE(2,errors,3);
	 }
	 if(errors[1]=='3')
	 {
	     errors[1]='0';
		 EEPROM_WRITE(2,errors,3);
	 }
	 EA=1;
	 errors[2]=0;
   }
   if(status==0&&is_lock==1&&use_password==0&&use_fingerprint==0&&use_rfid==0&&errors[2]==0)
   {
		OLED_ShowString(7,0,"MultiFunc Lock",16);
		OLED_ShowString(0,2,"select open mode",14);
		OLED_ShowString(12,4,"1.password",10);
		OLED_ShowString(12,5,"2.fingerprint",10);  
		OLED_ShowString(12,6,"3.rfid card",10);
   }
   if(status==0&&is_lock==1&&use_fingerprint==1&&ini_over==0&&errors[2]==0)
   {
      ES=0;
	  delay(100*125);
      Device_Check();
   }
   if(status==0&&is_lock==1&&use_fingerprint==1&&ini_over==1&&errors[2]==0)
   { 
      ini_over=0;
	  FPM10A_Find_Fingerprint();
	  if(use_fingerprint==0)
	  {
		OLED_Clear();
	  }
   }
   if(password_visble==0&&visble_justnow==1&&errors[2]==0)
   {
      OLED_Clear();
	  visble_justnow=0;
   }
    if(status==0&&is_lock==1&&use_password==1&&password_visble==0&&errors[2]==0)
   { 
    
	 OLED_ShowString(0,2,"Please enter",30);
	 OLED_ShowString(50,3,"password:",30);
	 if(row==0)
	{
	 for(n=0;n<i;n++)
	 {
     OLED_ShowChar(8*n,5,input_buf[n],30);
	 }
	}
	if(row_1==1)
	{
	  for(n=0;n<16;n++)
	 {
     OLED_ShowChar(8*n,5,input_buf[n],30);
	 }
	}
	 if(i==16&&row==0)
	 {
	  row++;row_1=1;
	 }
	 if(row==1)
	 {
	   for(n=0;n<i-16;n++)
	   {
	   OLED_ShowChar(8*n,6,input_buf[n+16],30);
	   }
	 }
	 if(row_2==1)
	 {
	   for(n=0;n<16;n++)
	   {
	   OLED_ShowChar(8*n,6,input_buf[n+16],30);
	   }
	 }
	 if(i==32&&row==1)
	 {
	 row=2;row_2=1;
	 }
   }
	if(status==0&&is_lock==1&&use_password==1&&password_visble==1&&errors[2]==0)
	 {
	   if(visble_justnow==0)
	   {
	   OLED_Clear();
	   }
	   OLED_ShowString(0,2,"Please enter",30);
	   OLED_ShowString(50,3,"password:",30);
	   if(row==0)
	  {
	 for(n=0;n<i;n++)
	 {
     OLED_ShowChar(8*n,5,input[n],30);
	 }
	}
	if(row_1==1)
	{
	  for(n=0;n<16;n++)
	 {
     OLED_ShowChar(8*n,5,input[n],30);
	 }
	}
	 if(i==16&&row==0)
	 {
	  row++;row_1=1;
	 }
	 if(row==1)
	 {
	   for(n=0;n<i-16;n++)
	   {
	   OLED_ShowChar(8*n,6,input[n+16],30);
	   }
	 }
	 if(row_2==1)
	 {
	   for(n=0;n<16;n++)
	   {
	   OLED_ShowChar(8*n,6,input[n+16],30);
	   }
	 }
	 if(i==32&&row==1)
	 {
	 row=2;row_2=1;
	 }
	   visble_justnow=1;
	   if(visble_change_near==1)
	   {
		visble_change_near=0;
	   }
	  }
    if(status==0&&is_lock==1&&use_rfid==1&&errors[2]==0)
   {
	  OLED_ShowString(0,1,"Waiting for card",30);
      send_byte(0x01);
	  delay_ms(100);
	  OLED_ShowString(0,4,"Press ini-button",30);
	  OLED_ShowString(26,5,"to quit.",30);
	  while(RI==0);
	  use_rfid=0;
	  OLED_Clear();
	  ES=1;
   }
    if(status==0&&is_lock==0&&open_just_now==1&&errors[2]==0)
   {
      OLED_Clear();
	  OLED_ShowString(58,4,"WELCOME!",36);
      led_blink_times(3);
	  flag_open=1;
	  motor_on();
	  open_just_now=0;
	  use_password=0;
	  OLED_Clear();
   }
   if(status==0&&is_lock==0&&errors[2]==0)
   {
	  OLED_DrawBMP(2,0,34,4,atom);
	  OLED_ShowString(50,1,"MultiFunc",32);
	  OLED_ShowString(70,2,"Lock",32);
	  OLED_ShowString(0,4,"1.reset password",30);
	  OLED_ShowString(0,5,"2.finger set",30);
	  OLED_ShowString(0,6,"3.clockmusic set",30);
	  OLED_ShowString(0,7,"4.lock",30);
   }
   if(status==1&&errors[2]==0)
   {
	 if(set_password==1&&password_visble==0)
	 {
		OLED_ShowString(0,1,"Please enter",30);
		OLED_ShowString(20,2,"new password:",30);
		if(row==0)
	{
	 for(n=0;n<i;n++)
	 {
     OLED_ShowChar(8*n,5,input_buf[n],30);
	 }
	}
	if(row_1==1)
	{
	  for(n=0;n<16;n++)
	 {
     OLED_ShowChar(8*n,5,input_buf[n],30);
	 }
	}
	 if(i==16&&row==0)
	 {
	  row++;row_1=1;
	 }
	 if(row==1)
	 {
	   for(n=0;n<i-16;n++)
	   {
	   OLED_ShowChar(8*n,6,input_buf[n+16],30);
	   }
	 }
	 if(row_2==1)
	 {
	   for(n=0;n<16;n++)
	   {
	   OLED_ShowChar(8*n,6,input_buf[n+16],30);
	   }
	 }
	 if(i==32&&row==1)
	 {
	 row=2;row_2=1;
	 }
   }
   	if(set_password==1&&password_visble==1)
	 {
	   if(visble_justnow==0)
	   {
	   OLED_Clear();
	   }
	   OLED_ShowString(0,1,"Please enter",30);
	   OLED_ShowString(20,2,"new password:",30);
	   if(row==0)
	  {
	 for(n=0;n<i;n++)
	 {
     OLED_ShowChar(8*n,5,input[n],30);
	 }
	}
	if(row_1==1)
	{
	  for(n=0;n<16;n++)
	 {
     OLED_ShowChar(8*n,5,input[n],30);
	 }
	}
	 if(i==16&&row==0)
	 {
	  row++;row_1=1;
	 }
	 if(row==1)
	 {
	   for(n=0;n<i-16;n++)
	   {
	   OLED_ShowChar(8*n,6,input[n+16],30);
	   }
	 }
	 if(row_2==1)
	 {
	   for(n=0;n<16;n++)
	   {
	   OLED_ShowChar(8*n,6,input[n+16],30);
	   }
	 }
	 if(i==32&&row==1)
	 {
	 row=2;row_2=1;
	 }
	   visble_justnow=1;
	   if(visble_change_near==1)
	   {
		visble_change_near=0;
	   }
	  }

	 else if(set_fingerprint==1)
	 {	
	    OLED_ShowString(0,2,"Fingerprint Mode",36);
		OLED_ShowString(0,4,"1.add fingers",30);
		OLED_ShowString(0,5,"2.delete fingers",30);
		OLED_ShowString(0,6,"3.exit",30);
		key_value=16;
		keydown();
		if(key_value==1)
		{
		 add_fingerprint=1;
		 OLED_Clear();
		 Uart_Init_As608();
		 FPM10A_Add_Fingerprint();
		 Uart_Init_Slave();
		}
		else if(key_value==2)
		{
		 del_fingerprint=1;
		 OLED_Clear();
		 Uart_Init_As608();
		 FPM10A_Delete_All_Fingerprint();
		 Uart_Init_Slave();
		}
		else if(key_value==3)
		{
		  set_fingerprint=0;
		  status=0;
		  OLED_Clear();
		}

	 }
	 else if(set_music==1)
	 {
	   OLED_ShowString(0,0,"clock music is:",30);
	   OLED_ShowString(10,2,"song",30);
	   switch(key_value)
	   {
		 case 1:OLED_ShowChar(42,2,'1',30);
		 song1=1;
		 song2=0;
		 song3=0;
		 song4=0;
		 song5=0;
		 break;
		 case 2:OLED_ShowChar(42,2,'2',30);
		 song1=0;
		 song2=1;
		 song3=0;
		 song4=0;
		 song5=0;
		 break;
		 case 3:OLED_ShowChar(42,2,'3',30);
		 song1=0;
		 song2=0;
		 song3=1;
		 song4=0;
		 song5=0;
		 break;
		 case 4:OLED_ShowChar(42,2,'4',30);
		 song1=0;
		 song2=0;
		 song3=0;
		 song4=1;
		 song5=0;
		 break;
		 case 5:OLED_ShowChar(42,2,'5',30);
		 song1=0;
		 song2=0;
		 song3=0;
		 song4=0;
		 song5=1;
		 break;
		 case 14:
		 if(song1==1)
		 {
		   OLED_Clear();
		   OLED_ShowString(0,0,"song1 is ready.",30);
		   song_set[0]=1;
		   EEPROM_WRITE(1,song_set,5);
		   delay_main(1000);
		   OLED_Clear();
		   set_music=0;
		   status=0;
		 }
		 if(song2==1)
		 {
		   OLED_Clear();
		   OLED_ShowString(0,0,"song2 is ready.",30);
		   song_set[0]=1;
		   EEPROM_WRITE(1,song_set,5);
		   delay_main(1000);
		   OLED_Clear();
		   set_music=0;
		   status=0;
		 }
		 if(song3==1)
		 {
		   OLED_Clear();
		   OLED_ShowString(0,0,"song3 is ready.",30);
		   song_set[0]=1;
		   EEPROM_WRITE(1,song_set,5); 
		   delay_main(1000);
		   OLED_Clear();
		   set_music=0;
		   status=0;
		 }
		 if(song4==1)
		 {
		   OLED_Clear();
		   OLED_ShowString(0,0,"song4 is ready.",30);
		   song_set[0]=1;
		   EEPROM_WRITE(1,song_set,5); 
		   delay_main(1000);
		   OLED_Clear();
		   set_music=0;
		   status=0;
		 }
		 if(song5==1)
		 {
		   OLED_Clear();
		   OLED_ShowString(0,0,"song5 is ready.",30);
		   song_set[0]=1;
		   EEPROM_WRITE(1,song_set,5); 
		   delay_main(1000);
		   OLED_Clear();
		   set_music=0;
		   status=0;
		 }
		 break;
         case 15:set_music=0;
		 break;
	   }
	 }
	}
}


void recv_from_slave(void) interrupt 4
{  
	u8 Data;
	Data=SBUF;
	if(RI)
	{
		if(Data==0x01)
		{
		  if(is_lock==1)
		  {
		  set_password=0;
	      use_password=0;
		  use_rfid=0;//接受来自从机的bit
		  status=0;
		  is_lock=0;
		  ini_over=0;
		  open_just_now=1;
		  use_fingerprint=0;
		  add_fingerprint=0;
		  del_fingerprint=0;
		  set_fingerprint=0;
		  OLED_Clear();
		  }
		}
		if(Data==0x02)
		{
			clock_play_music = 1;
		}
	
		RI=0;
	}

}
void play_clock_music()
{
		if(clock_play_music==1)
 		 {
		  if(song1==1)
		  { 
		    OLED_Clear();
			OLED_ShowString(0,0,"Clock1 is on!",30);
			OLED_ShowString(0,2,"music song1.",30);
			Play_Music1();
			OLED_Clear();
			clock_play_music=0;

		  }
		  if(song2==1)
		  { 
		    OLED_Clear();
			OLED_ShowString(0,0,"Clock1 is on!",30);
			OLED_ShowString(0,2,"music song2.",30);
			Play_Music2();
			OLED_Clear();
			clock_play_music=0;
		  }
		  if(song3==1)
		  { 
		    OLED_Clear();
			OLED_ShowString(0,0,"Clock1 is on!",30);
			OLED_ShowString(0,2,"music song3.",30);
			Play_Music3();
			OLED_Clear();
			clock_play_music=0;
		  }
		  if(song4==1)
		  { 
		    OLED_Clear();
			OLED_ShowString(0,0,"Clock1 is on!",30);
			OLED_ShowString(0,2,"music song4.",30);
			Play_Music4();
			OLED_Clear();
			clock_play_music=0;
		  }
		  if(song5==1)
		  { 
		    OLED_Clear();
			OLED_ShowString(0,0,"Clock1 is on!",30);
			OLED_ShowString(0,2,"music song5.",30);
			Play_Music3();
			OLED_Clear();
			clock_play_music=0;
		  }
		}
} 

void main()	
{  		
  	    
     	Uart_Init_Slave();
		OLED_Init();
		EEPROM_init();
		ES=1;
		OLED_Clear();
		MotorStop();
 //到时候加上
		OLED_ShowString(7,1,"MultiFunc Lock",30);
		OLED_ShowString(15,4,"made by  WJC",8);
		OLED_ShowString(80,5," WXB",8);
		OLED_ShowString(80,6," LQ",8); 
		delay_ms(100); //important name	
	    OLED_Clear();	
		while(1)
		{	
			keydown();
			display_pros();
			play_clock_music();
		} 
}



