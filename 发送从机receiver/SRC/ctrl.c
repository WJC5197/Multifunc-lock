#include "reg52.h"
#include "string.h"
#include "uart.h"  
#include "rc522.h"
#include "ctrl.h"
#include "EEPROM.h"
#include "main.h"
#include "timer.h"
#include "lcd.h"


u8 i_rc522=0;
u8 RC522_KeyValue=4;
u8 Card_type[2],	//卡片类型
      Card_SN[4]={0},//IC卡号 series nubmers
	  Card_SN_BUF[4]={0xff,0xff,0xff,0xff},
	  Card_saved_asc[128]={'E','5','8','7','D','C','3','0','7','7','9','A','A','0','6','0','F','9','5','C','9','1','D','5','D','9','D','4','8','6','D','3','\0'},   
	  KEY_BUF[8],
	  ID_ASC[8]={0};

u8 bPass = 0, bWarn = 0;

u8 key_count=0;
u8 table[8]={'0','0','0','0','0','0','0','0'};
u16 j_rc522=0;

bit rc522_card_check=0;
bit card_pass=0;
bit in_rc522_password=0;
bit rc522_newcard=0;
bit rc522_set_password=0;
bit rc522_manage=0;
bit rc522_state=0;
bit sounder_beep=0;
bit card_compare=0;
bit is_newcard=0;
	   
void Led_light()
{
  LED=0;
}

void Led_shut()
{
  LED=1;
}

void LED_BLINK_1()
{
   LED = 0;	
   Delay_ms(500);
   LED = 1;		
}

void LED_BLINK_2()
{
   LED = 0;
   Delay_ms(250);
   LED = 1;
   Delay_ms(250);
   LED = 0;
   Delay_ms(250);
   LED = 1;
} 

void uart_over( void )	//串口数据还原
{
	UartCount = 0;
	UartStart = FALSE;
	UartComp = FALSE;
}

void pass( void )
{
   Delay_ms(1000);//让卡片离开
   LED_BLINK_1();
}

void warn(void)
{
  LED_BLINK_2();
}

void RC522_KeyDown()
{
  char a=0;
	if(SETTIME_MODE!=1)
	{
	  Delay_ms(5);
	  if(SETTIME_MODE!=1)
	  {
	    RC522_KeyValue=0;
	   		while((a<70)&&(SETTIME_MODE!=1))	 //检测按键松手检测
			{
				Delay_ms(5);
				a++;
			}	
	  }
	}
	if(TEMPER_MODE!=1)
	{
	  Delay_ms(5);
	  if(TEMPER_MODE!=1)
	  {
	    RC522_KeyValue=1;
	   			while((a<70)&&(TEMPER_MODE!=1))	 //检测按键松手检测
			{
				Delay_ms(5);
				a++;
			}	
	  }
	}
	if(RC522_MODE!=1)
	{
	  Delay_ms(5);
	  if(RC522_MODE!=1)
	  {
	   RC522_KeyValue=3;
	   			while((a<70)&&(RC522_MODE!=1))	 //检测按键松手检测
			{
				Delay_ms(5);
				a++;
			}	
	  }
	}
	if(CLOCK_SET!=1)
	{
	  Delay_ms(5);
	  if(CLOCK_SET!=1)
	  {
	    RC522_KeyValue=2;
					while((a<70)&&(CLOCK_SET!=1))	 //检测按键松手检测
			{
				Delay_ms(5);
				a++;
			}	
	  }
	}
	
}

u8 IC_READ()
{
    u8 i;
	if( PcdRequest( PICC_REQIDL, Card_type ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
	{
		if( PcdRequest( PICC_REQIDL, Card_type ) != MI_OK )//寻天线区内未进入休眠状态的卡，返回卡片类型 2字节	
		{		   
			return FALSE;
		}	
	}
	
	if( PcdAnticoll( Card_SN ) != MI_OK ) //防冲撞，返回卡的序列号 4字节 
	{

		bWarn = 1;
		return FALSE;	
	}

	bPass = 1;
/*	send_bytes(Card_type,2);
	send_bytes(Card_SN,4);	 */
	
	for(i=0;i<4;i++) //卡ID号转化成ASCII
	{
	  if(Card_SN[i]/16>9)	ID_ASC[i*2]=Card_SN[i]/16+'7';
	    else ID_ASC[i*2]=Card_SN[i]/16+'0';

      if(Card_SN[i]%16>9)	ID_ASC[i*2+1]=Card_SN[i]%16+'7';
	    else ID_ASC[i*2+1]=Card_SN[i]%16+'0';
	}
	return TRUE;
}

bit rc522_card_compare()
{
   u8 j,m,num;
   bit wrong=0;
   bit cycle_finished=0;	
   m=0;
   num=strlen(Card_saved_asc)/8;
   for(m=0;m<num;++m)
   {
     for(j=0;j<8;j++)
	{
	 if(j==7)
	 {
	  cycle_finished=1;
	 }	  
	 if(ID_ASC[j]!=Card_saved_asc[(m)*8+j])
	 {
	   wrong=1;
	   break;
	 }
	}
	 if(wrong==0&&cycle_finished==1)
	 {
	   cycle_finished=0;
       return 1;
	 }
	 if(wrong==1)
	 {
	   if(m==num)
	   {
		return 0;
	   }
	   wrong=0;
	   cycle_finished=0;
	   LED=0;
	   continue;
	 }
   }
}

void ctrl_process()
{		  
   u8 i;
	if(rc522_state==1&&status==1&&card_compare==0&&in_rc522_password==0&&rc522_manage==0&&rc522_newcard==0&&rc522_set_password==0)
	{
          rc522_card_check=1;
		  if(rc522_card_check==1&&card_pass==0)
		  {
		  LCD_disp_string(0,1,"---check card---");
		  LCD_disp_string(0,2,"wait for card...");
		  switch(RC522_KeyValue)
		  {
		  	case 0:
			   card_pass=0;
			   rc522_card_check=0;
			   in_rc522_password=0;
			   rc522_newcard=0;
               rc522_set_password=0;
			   rc522_manage=0;
			   rc522_state=0;
			   status=0;
			   RC522_KeyValue=4;
			   LCD_write_command(0x01);
			   Delay_ms(100);
			   break;
			default :break;
		  }	          //IC卡读卡输入
		       if(IC_READ()) 
			  {
			      IC_READ();
			      LCD_write_command(0x01);
				  Delay_ms(50);
              /*  if ((Card_SN[0]==Card_SN_BUF[0])
			      &&(Card_SN[1]==Card_SN_BUF[1])
				  &&(Card_SN[2]==Card_SN_BUF[2])
				  &&(Card_SN[3]==Card_SN_BUF[3]))
				  {	   */
				    LCD_disp_string(1,1,"Read success!");
				    LCD_disp_string(1,2,"ID:"); //显示卡号
					LCD_disp_string(4,2,ID_ASC);
					pass();//灯开关
					LCD_write_command(0x01);
					Delay_ms(100);
					rc522_card_check=0;
					card_compare=1;
			    //  }
/*				  else 
				  {
				   warn();
				   LCD_write_command(0x01);
				   LCD_disp_string(1,1,"fail!");   */
				  } 
			   }
			 }
	    if(card_compare==1)
		{
		  card_pass=rc522_card_compare();
		  Delay_ms(1000);
		  LCD_write_command(0x01);
		  Delay_ms(50);
		  if(card_pass==1)
		  {
			card_compare=0;
		    LCD_disp_string(0,1,"Compare success!");
			LCD_disp_string(0,2,"Welcome Home!");
			send_byte(0x01);
			Delay_ms(1000);
			LCD_write_command(0x01);
			Delay_ms(50);
		  }
		  else
		  {
			card_compare=0;
			Delay_ms(50);
			LCD_disp_string(1,1,"Card not exist!");
			Delay_ms(1000);
			LCD_write_command(0x01);
			Delay_ms(100);
			rc522_card_check=1;
		  }

		}
	    if(card_pass==1)
		{
		  LCD_disp_string(0,1,"|SETTINGS ENTRY|");
		  LCD_disp_string(2,2,"press < or >");
		  switch(RC522_KeyValue)
		  {
		   case 1:card_pass=0;
		          rc522_card_check=1;
				  RC522_KeyValue=4;
				  LCD_write_command(0x01);
			      Delay_ms(100);
				 break;
		   case 2:
		         card_pass=0;
		         in_rc522_password=1;
				 RC522_KeyValue=4;
				 LCD_write_command(0x01);
			      Delay_ms(100);
				 break;
		   case 3:
		         card_pass=0;
				 rc522_manage=0;
				 RC522_KeyValue=4;
		         in_rc522_password=1;
				 LCD_write_command(0x01);
			      Delay_ms(100);
				 break;
		   case 0:
		         card_pass=0;
				 rc522_card_check=0;
				 in_rc522_password=0;
				 rc522_newcard=0;
                 rc522_set_password=0;
				 rc522_manage=0;
				 rc522_state=0;
				 status=0;
				 RC522_KeyValue=4;
				 LCD_write_command(0x01);
				 Delay_ms(100);					//完全退出rc522模式
				 break;
		  }

		}
		if(in_rc522_password==1)
		{
		  switch(RC522_KeyValue)
		  {
			 case 0:table[key_count]++;
			 if(table[key_count]=='9'+1)
			 {
			  table[key_count]-=10;
			 }
			 break;
			 case 1:table[key_count]--;
			 if(table[key_count]=='0'-1)
			 {
			  table[key_count]+=10;
			 }
			 break;
			 case 3:
			 if(key_count<8)
			 {
			 key_count++;
			   break;
			 }
			 if(key_count==8)
			 {
			   break;
			 }
			 case 2:
			 if(key_count>0)
			 {
			 key_count--;
			 	 break;
			 }
			 if(key_count==0)
			 {
			  break;
			 }
		  }
		  LCD_disp_string(1,1,"enter password:");
		  LCD_disp_char(1,2,table[0]);
		  LCD_disp_char(2,2,table[1]);
		  LCD_disp_char(3,2,table[2]);
		  LCD_disp_char(4,2,table[3]);
		  LCD_disp_char(5,2,table[4]);
		  LCD_disp_char(6,2,table[5]);
		  LCD_disp_char(7,2,table[6]);
		  LCD_disp_char(8,2,table[7]);
		for(j_rc522=0;SETTIME_MODE==1&&TEMPER_MODE==1&&RC522_MODE==1&&CLOCK_SET==1;j_rc522++)
        {
		  	if(key_count>=0&&key_count<=7)
	        {
	    	LCD_write_command(0xc0 + key_count+1) ;
	    	LCD_write_command(0x0f);
			}
		}	 
		      if(key_count==8)
				  {
				     LCD_write_command(0x0c);
					 Delay_ms(100);
				     if( table[0]==KEY_BUF[0] &&
					     table[1]==KEY_BUF[1] &&
					 	 table[2]==KEY_BUF[2] &&
						 table[3]==KEY_BUF[3] &&
						 table[4]==KEY_BUF[4] &&
						 table[5]==KEY_BUF[5] &&
						 table[6]==KEY_BUF[6] &&
						 table[7]==KEY_BUF[7] )	  //密吗正确 
					  {
					   bPass=1;
					   rc522_manage=1;
					   in_rc522_password=0;
					   LCD_write_command(0x01);
					   Delay_ms(100);
					   LCD_disp_string(0,1,"enter to manage");
					   LCD_disp_string(0,2,"successfully!");
					   Delay_ms(1000);
					   LCD_write_command(0x01);
			           Delay_ms(100);
					   RC522_KeyValue=4;
					   Led_shut();
					   key_count=0;
					   }				  
					  else 	 //密码错误
					 {	
						//beep一下
						key_count=0;
						for(i_rc522=0;i_rc522<8;i_rc522++)
						{
						  table[i_rc522]='0';
						}
						LCD_write_command(0x01);
						Delay_ms(100);
						LCD_disp_string(0,1,"password error!");
						Delay_ms(1000);
						LCD_write_command(0x0c);
					    Delay_ms(100);
						RC522_KeyValue=4;
						card_pass=1;
						in_rc522_password=0;
					    LCD_write_command(0x01);
			      		Delay_ms(100);
						bWarn=1;
					 }
		          }
			}
	
		if(rc522_manage==1)
	   {
		  LCD_disp_string(0,1,"|RC522 SETTINGS|");
		  LCD_disp_string(0,2,"newcard< reset>");
		  switch(RC522_KeyValue)
        {
		  case 1:
		  rc522_manage=0;
		  card_pass=1;
		  RC522_KeyValue=4;
		  LCD_write_command(0x01);
		  Delay_ms(100);
		  break;
		  case 2:
		  rc522_manage=0;
		  rc522_newcard=1;
		  RC522_KeyValue=4;
		  LCD_write_command(0x01);
		  Delay_ms(100);
		  break;
		  case 3:
		  rc522_manage=0;
		  rc522_set_password=1;
		  RC522_KeyValue=4;
		  LCD_write_command(0x01);
		  Delay_ms(100);
		  break;
		  case 0:
		  card_pass=0;
		  rc522_card_check=0;
		  in_rc522_password=0;
		  rc522_newcard=0;
          rc522_set_password=0;
		  rc522_manage=0;
		  RC522_KeyValue=4;
		  LCD_write_command(0x01);
		  Delay_ms(100);
		  break;
		}
	   }
        if(rc522_newcard==1)
		{
		  LCD_disp_string(1,1,"set new card");
		  LCD_disp_string(1,2,"check waiting...");
	      if(IC_READ())  
			   {
			    LCD_write_command(0x01);
				Delay_ms(100);
				LCD_disp_string(1,1,"Read success!");
				LCD_disp_string(1,2,"ID:");
				LCD_disp_string(5,2,ID_ASC);
				Delay_ms(2000);
				is_newcard=rc522_card_compare();
				if(is_newcard==0)
				{
				LCD_write_command(0x01);
				Delay_ms(100);
				LCD_disp_string(0,1,"success recorded!");
				LCD_disp_string(1,2,"ID:");
				LCD_disp_string(5,2,ID_ASC);
				Delay_ms(2000);	 
			    for(i_rc522=strlen(Card_saved_asc),i=0;i_rc522<strlen(Card_saved_asc)+8;i_rc522++,i++)
				{
				   Card_saved_asc[i_rc522]=ID_ASC[i];
				}
				EEPROM_WRITE(2,Card_saved_asc,128);//写入EEPROM
			    RC522_KeyValue=4;
				rc522_newcard=0;
		  		rc522_manage=1;
		   		LCD_write_command(0x01);
		  		Delay_ms(100);
				bPass=1;
				}
				else
				{
				  LCD_write_command(0x01);
				  Delay_ms(100);
				  LCD_disp_string(1,1,"Card is exist!");
				  LCD_disp_string(1,2,"fail saving.");
				  Delay_ms(2000);
				  RC522_KeyValue=4;
				  rc522_newcard=0;
		  		  rc522_manage=1;
				  LCD_write_command(0x01);
		  		  Delay_ms(100);
				  bWarn=1;
				}
			   }
		
		}
		if(rc522_set_password==1)
		{
          switch(RC522_KeyValue)
		  {
			 case 0:table[key_count]++;
			 if(table[key_count]=='9'+1)
			 {
			  table[key_count]-=10;
			 }
			 break;
			 case 1:table[key_count]--;
			 if(table[key_count]=='0'-1)
			 {
			  table[key_count]+=10;
			 }
			 break;
			 case 3:
			 if(key_count<8)
			 {
			 key_count++;
			   break;
			 }
			 if(key_count==8)
			 {
			   break;
			 }
			 case 2:
			 if(key_count>0)
			 {
			 key_count--;
			 	 break;
			 }
			 if(key_count==0)
			 {
			  break;
			 }
		  }
		  LCD_disp_string(1,1,"in new password:");
		  LCD_disp_char(1,2,table[0]);
		  LCD_disp_char(2,2,table[1]);
		  LCD_disp_char(3,2,table[2]);
		  LCD_disp_char(4,2,table[3]);
		  LCD_disp_char(5,2,table[4]);
		  LCD_disp_char(6,2,table[5]);
		  LCD_disp_char(7,2,table[6]);
		  LCD_disp_char(8,2,table[7]);
		for(j_rc522=0;SETTIME_MODE==1&&TEMPER_MODE==1&&RC522_MODE==1&&CLOCK_SET==1;j_rc522++)
        {
		  	if(key_count>=0&&key_count<=7)
	        {
	    	LCD_write_command(0xc0 + key_count+1) ;
	    	LCD_write_command(0x0f);
			Delay_ms(5);
			}
		}
		  if(key_count==8)
		  { 
		    EEPROM_WRITE(1,table,8);
			for(i_rc522=0;i_rc522<8;i_rc522++)
			{
			  KEY_BUF[i_rc522]=table[i_rc522];
			}
			key_count=0;
			for(i_rc522=0;i_rc522<8;i_rc522++)
			{
			  table[i_rc522]='0';
			}
			rc522_set_password=0;
			rc522_manage=1;
			LCD_write_command(0x01);
			Delay_ms(100);
			LCD_write_command(0x0c);
			Delay_ms(100);
			LCD_disp_string(0,1,"reset password");
			LCD_disp_string(0,2,"successfully!");
			Delay_ms(2000);
	        LCD_write_command(0x01);
			Delay_ms(100);
		  }
		}	  		

	if( bPass )		 //处理成功
	{
		bPass = 0;
		pass();	
	
	}

	if( bWarn )	     //处理失败
	{
		bWarn = 0;
		warn();
	}
	  
}

