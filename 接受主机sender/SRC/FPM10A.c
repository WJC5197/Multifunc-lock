#include "reg52.h"
#include "FPM10A.h"
#include "uart.h"
#include "oled.h"
#include "sbit.h"
#include "EEPROM.h"

#define u8 unsigned char
#define u16 unsigned int

volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned char finger_id = 0;
u8 key_value_fp=16;
u8 errors[3]={'0','0',0};

bit init_fail=0;
bit status=0;
bit is_lock=1;
bit ini_over=0;
bit open_just_now=0;
bit use_fingerprint=0;
bit fingerprint_fail=0;
bit fingerprint_pass=0;
bit add_fingerprint=0;
bit del_fingerprint=0;
bit set_fingerprint=0;
bit unable_input=0;

//FINGERPRINTͨ��Э�鶨��

code unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//������֤
code unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //Э���ͷ
code unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //���ָ��ͼ��
code unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //���ģ������
code unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; //����ָ��������Χ0 - 999,ʹ��BUFFER1�е�����������
code unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //����0-9��ָ��
code unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //��ͼ����뵽BUFFER1
code unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //��ͼ����뵽BUFFER2
code unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //��BUFFER1��BUFFER2�ϳ�����ģ��
code unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//ɾ��ָ��ģ�������е�ģ��
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//��BUFFER1�е��������ŵ�ָ����λ��
//volatile:ϵͳ�������´������ڵ��ڴ��ȡ���ݣ���ʹ��ǰ���ָ��ոմӸô���ȡ������
/*------------------ FINGERPRINT������ --------------------------*/
 //���Ͱ�ͷ
void Delay_Us(int i)
{
	while(--i);
}

void Delay_Ms_fpm(int m)
{
 	while(m--)
	{
	 	Delay_Us(125);
	}
}
void Buzz_Times(u8 k)
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

void keydown_fp(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//��ȡ�����Ƿ���
	{
		Delay_Us(8);//��ʱ10ms��������
		if(GPIO_KEY!=0x0f)//�ٴμ������Ƿ���
		{	
			//������
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	key_value_fp=0;break;
				case(0X0b):	key_value_fp=1;break;
				case(0X0d): key_value_fp=2;break;
				case(0X0e):	key_value_fp=3;break;
			}
			//������
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0Xe0):	key_value_fp=key_value_fp;break;
				case(0Xd0):	key_value_fp=key_value_fp+4;break;
				case(0Xb0): key_value_fp=key_value_fp+8;break;
				case(0X70):	key_value_fp=key_value_fp+12;break;
			}
		}		
	}
	while((a<75)&&(GPIO_KEY!=0XF0))	 //��ⰴ�����ּ��
	{
		Delay_Ms_fpm(1);
		a++;
	}
}

void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;	
	for(i=0;i<6;i++) //��ͷ
   {
     Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    }		
}
//����ָ��
void FPM10A_Cmd_Check(void)
{
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
	for(i=0;i<10;i++)
	 {		
		Uart_Send_Byte(FPM10A_Get_Device[i]);
	  }
}
//���շ������ݻ���
void FPM10A_Receive_Data(unsigned char ucLength)
{
  unsigned char i;

  for (i=0;i<ucLength;i++)
     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();

}

//FINGERPRINT_���ָ��ͼ������
void FPM10A_Cmd_Get_Img(void)
{
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
    for(i=0;i<6;i++) //�������� 0x1d
	{
       Uart_Send_Byte(FPM10A_Get_Img[i]);
	}
}
//��ͼ��ת��������������Buffer1��
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ      
   	for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
     {
      Uart_Send_Byte(FPM10A_Img_To_Buffer1[i]);
   	  }
}
//��ͼ��ת��������������Buffer2��
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
     for(i=0;i<6;i++)    //���Ͱ�ͷ
	 {
    	Uart_Send_Byte(FPM10A_Pack_Head[i]);   
   	 }
     for(i=0;i<7;i++)   //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
      {
      	Uart_Send_Byte(FPM10A_Img_To_Buffer2[i]);
   	  }
}
//����ȫ���û�999ö
void FPM10A_Cmd_Search_Finger(void)
{
       unsigned char i;	   	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ
       for(i=0;i<11;i++)
           {
    	      Uart_Send_Byte(FPM10A_Search[i]);   
   		   }
}

void FPM10A_Cmd_Reg_Model(void)
{
       unsigned char i;	   
	    
			 FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ

       for(i=0;i<6;i++)
           {
    	      Uart_Send_Byte(FPM10A_Reg_Model[i]);   
   		   }


}
//ɾ��ָ��ģ���������ָ��ģ��
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;    
    for(i=0;i<6;i++) //��ͷ
      Uart_Send_Byte(FPM10A_Pack_Head[i]);   
    for(i=0;i<6;i++) //����ϲ�ָ��ģ��
	   {
      Uart_Send_Byte(FPM10A_Delete_All_Model[i]);   
		 }	
}
//����ָ��
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
       unsigned long temp = 0;
		   unsigned char i;
       FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
       FPM10A_Save_Finger[6] = (storeID&0x00FF);
		   for(i=0;i<7;i++)   //����У���
		   	   temp = temp + FPM10A_Save_Finger[i]; 
		   FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //���У������
		   FPM10A_Save_Finger[8]= temp & 0x0000FF;		   
       FPM10A_Cmd_Send_Pack_Head(); //����ͨ��Э���ͷ	
       for(i=0;i<9;i++)  
      		Uart_Send_Byte(FPM10A_Save_Finger[i]);      //�������� ��ͼ��ת���� ������ ����� CHAR_buffer1
}
//���ָ��
void FPM10A_Add_Fingerprint()
{
	unsigned char id_show[]={0,0,0};
    OLED_Clear();
	finger_id=0;
	key_value_fp=5;  
	while(1)
	{
	 keydown_fp();
	 id_show[0]=finger_id/100+48;
	 id_show[1]=finger_id%100/10+48;
	 id_show[2]=finger_id%100%10+48;
     OLED_ShowString(20,2,"Add Finger",36);
	 OLED_ShowString(8,4,"  ID is     ",36);
	 OLED_ShowChar(70,4,id_show[0],30);
	 OLED_ShowChar(80,4,id_show[1],30);
	 OLED_ShowChar(90,4,id_show[2],30);
	 OLED_ShowString(0,6,"press '0' to add",36);
	if(key_value_fp == 2) 
	{
	   OLED_Clear();
	   set_fingerprint=0;
	   add_fingerprint=0;
	   del_fingerprint=0;
	   status=0;
	   key_value_fp=5;	 
		 break;
	}
	//���л���ָ��iDֵ��1
	if(key_value_fp == 1)
	{
		if(finger_id == 1000)
		{
			finger_id = 0;
		}
		else
		finger_id = finger_id + 1;
		key_value_fp=5;
	}
	 //��ȷ�ϼ���ʼ¼��ָ����Ϣ 		 			
	 if(key_value_fp == 0)
	  {
	   OLED_Clear();	
	   OLED_ShowString(0,1,"Please Finger: ",36);
			while(1)
		   {	
		        keydown_fp();
			  //���·��ؼ��˳�¼�뷵��fingerID����״̬   
				if(key_value_fp == 2) 
				 {
				 set_fingerprint=0;
	   			 add_fingerprint=0;
	 			 del_fingerprint=0;
				 status=0;
				 key_value_fp=5;
				  break;
				  }
				FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
				FPM10A_Receive_Data(12);
				//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
				if(FPM10A_RECEICE_BUFFER[9]==0)
				 {
					Delay_Ms_fpm(100);
					FINGERPRINT_Cmd_Img_To_Buffer1();
				    FPM10A_Receive_Data(12);
					OLED_Clear_Line(1);
					OLED_ShowString(0,1,"Success entry!",36);
   				    Buzz_Times(1);
					Delay_Ms_fpm(1000);
					OLED_Clear_Line(1);
	                OLED_ShowString(0,1,"Finger Again.",36);
					while(1)
					{	
					    keydown_fp();
						if(key_value_fp == 2) 
						{	
						set_fingerprint=0;
					    add_fingerprint=0;
					    del_fingerprint=0;
						status=0;
						key_value_fp=5;
						break;
						}
					 FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
					 FPM10A_Receive_Data(12);
					//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
					if(FPM10A_RECEICE_BUFFER[9]==0)
					{
						Delay_Ms_fpm(200);
						OLED_Clear_Line(1);
						OLED_ShowString(0,1,"Success entry!",36);
                        OLED_ShowString(0,3,"  ID is     ",36);
						 //ָ��iDֵ��ʾ���� 
	                    OLED_ShowChar(70,3,finger_id/100+48,30);
	                    OLED_ShowChar(80,3,finger_id%100/10+48,30);
	                    OLED_ShowChar(90,3,finger_id%100%10+48,30);
						FINGERPRINT_Cmd_Img_To_Buffer2();
				  		FPM10A_Receive_Data(12);
						FPM10A_Cmd_Reg_Model();//ת����������
	         			FPM10A_Receive_Data(12); 
					  	FPM10A_Cmd_Save_Finger(finger_id);                		         
	          			FPM10A_Receive_Data(12);
 					    Buzz_Times(3);
						Delay_Ms_fpm(1000);
						finger_id=finger_id+1;
						set_fingerprint=0;
					    add_fingerprint=0;
					    del_fingerprint=0;
						status=0;
				    	break;
				  	}
				   }
	        		break;
					}
				}
            OLED_Clear();
			}
			if(set_fingerprint==0)
			{
			 break;
			}
			key_value_fp=5;
		}
}

//����ָ��
void FPM10A_Find_Fingerprint()
{
	unsigned int find_fingerid = 0;
	unsigned char id_show[]={0,0,0};
	do
	{
		OLED_ShowString(0,1,"Please put on",30);
	    OLED_ShowString(30,2,"your finger.",30);
		FPM10A_Cmd_Get_Img(); //���ָ��ͼ��
		FPM10A_Receive_Data(12);		
		//�жϽ��յ���ȷ����,����0ָ�ƻ�ȡ�ɹ�
		if(FPM10A_RECEICE_BUFFER[9]==0)
		{			
			Delay_Ms_fpm(100);
			FINGERPRINT_Cmd_Img_To_Buffer1();
			FPM10A_Receive_Data(12);		
			FPM10A_Cmd_Search_Finger();
			FPM10A_Receive_Data(16);			
			if(FPM10A_RECEICE_BUFFER[9] == 0) //������  
			{
			     fingerprint_pass=1;
			     OLED_Clear_Line(1);
				 OLED_Clear_Line(2);
				 OLED_ShowString(0,1," Search success ",30);
				 OLED_ShowString(0,3,"    ID is       ",30);
				 errors[0]='0';
	  			 errors[1]='0';
	  			 EEPROM_WRITE(2,errors,3);
				 Buzz_Times(3);					
				//ƴ��ָ��ID��
				 find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];					
				 //ָ��iDֵ��ʾ���� 
				 OLED_ShowChar(80,3,find_fingerid/100+48,30);
				 OLED_ShowChar(90,3,find_fingerid%100/10+48,30);
				 OLED_ShowChar(100,3,find_fingerid%100%10+48,30);	
				 LED = 0;
				 Delay_Ms_fpm(3000);	
				 LED = 1;
			  }
				else //û���ҵ�
				{
				    fingerprint_fail=1;
				    OLED_Clear_Line(1);
				    OLED_Clear_Line(2);
					OLED_ShowString(0,1," Search  failed. ",30);
					errors[1]++;
					EEPROM_WRITE(2,errors,3);
					if(errors[1]=='3')
					{
						errors[2]=1;
						EEPROM_WRITE(2,errors,3);
					}
					OLED_ShowString(0,3,"Error times:",30);
					OLED_ShowChar(100,3,errors[1],30);
					LED=0;
				    Delay_Ms_fpm(250);
					LED=1;
					Delay_Ms_fpm(250);
					LED=0;
					Delay_Ms_fpm(250);
					LED=1;
				 	Buzz_Times(5);
					ES=1;
				}
				if(fingerprint_fail==1)
				{
				   Uart_Init_Slave();
				   Delay_Ms_fpm(50);
				   use_fingerprint=0;
				   OLED_Clear();
				   OLED_ShowString(0,3,"FAIL ENTRY!",36);
				   fingerprint_fail=0;
				   Delay_Ms_fpm(500);
				   ES=1;
				}
				if(fingerprint_pass==1)
				{
				   Uart_Init_Slave();
				   Delay_Ms_fpm(50);
				   use_fingerprint=0;
				   is_lock=0;
				   open_just_now=1;
				   ES=1;
				   fingerprint_pass=0;
				}
			}		
		}while(use_fingerprint==1);
}
//ɾ�����д�����ָ�ƿ�
void FPM10A_Delete_All_Fingerprint()
{
		unsigned char i=0;
		OLED_ShowString(20,1,"empty all",30);
	    OLED_ShowString(5,3,"1.Yes or 2.No?",30);
		key_value_fp=5;
		do
		 {
		    keydown_fp();
			if(key_value_fp==1)
			{
			    OLED_Clear();
			 	OLED_ShowString(20,1,"emptying",30); 
				Delay_Ms_fpm(300);
				for(i=0;i<16;i++)
				 {
				    OLED_ShowChar(8*i,2,'*',30);
					Delay_Ms_fpm(100);
				 }
				FINGERPRINT_Cmd_Delete_All_Model();
			    FPM10A_Receive_Data(12);
				OLED_Clear(); 
			    OLED_ShowString(20,1,"All empty.",30);
				Delay_Ms_fpm(500);
			    Buzz_Times(3);
				break;
			}
			if(key_value_fp==2)
			{
			  set_fingerprint=0;
			  status=0;
			  break;
			}
		 }while(1);
}

void Device_Check(void)
{
		unsigned char i=0;
		Uart_Init_As608();
		Delay_Ms_fpm(500);
		FPM10A_RECEICE_BUFFER[9]=1;
		OLED_Clear();				           //��������ھ�λ���ж��Ƿ�ͨ������								
		OLED_ShowString(0,1,"Dock  failure",30);      //Һ������ʾ�Խ�ʧ�ܣ����ָ��ģ���ԵĻ��Ὣ�串��	
		FPM10A_Cmd_Check();									//��Ƭ����ָ��ģ�鷢��У������
		FPM10A_Receive_Data(12);							//�����ڽ��յ�������ת��
		if(FPM10A_RECEICE_BUFFER[9] != 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
		{	
		    use_fingerprint=0;
			ini_over=0;
		    Delay_Ms_fpm(1000);
			OLED_Clear();	//���ϳɹ���������ʾ�Խӳɹ�
		}
 		if(FPM10A_RECEICE_BUFFER[9] == 0)					//�ж����ݵ͵�9λ�Ƿ���յ�0
		{	
		    OLED_Clear_Line(1);
			OLED_ShowString(0,1,"Dock  success",30);
			ini_over=1;
		    Delay_Ms_fpm(1000);
			OLED_Clear();	//���ϳɹ���������ʾ�Խӳɹ�
		}
		
}





