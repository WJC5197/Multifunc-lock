#include "lcd.h"
#include "reg52.h"
#define LCD_DB   P2  // - - P0   = DB0��DB7
sbit  LCD_RS=P0^5; // - - p2.0 = RS
sbit  LCD_RW=P0^6; // - - p2.1 = RW
sbit  LCD_E=P0^7;  // - - p2.2 = E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - ���庯��
#define u8 unsigned char
#define u16 unsigned int

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - �����ӳ�����
void LCD_init(void); // - - ��ʼ��LCD1602����
void LCD_write_command(u8 command); // - - ��LCD1602дָ���
void LCD_write_data(u8 dat); // - - ��LCD1602д���ݺ���
void LCD_set_xy(u8 x,u8 y); // - - ����LCD1602��ʾλ�� X��0-16),y(1-2)
void LCD_disp_char(u8 x,u8 y,u8 dat); // - - ��LCD1602����ʾһ���ַ�
void LCD_disp_string(u8 X,u8 Y,u8 *s); // - - ��LCD1602����ʾһ���ַ���
void LCD_check_busy(void);//���æ��������û�õ��˺�������Ϊͨ���ʼ��͡�
void LCD_delay_10us(u16 n); // - - 10΢�����ʱ�ӳ���
void LCD_delay_50us(u16 n); // - - 50΢�����ʱ�ӳ���
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - ��ʼ��LCD1602
void LCD_init(void)
{
LCD_delay_10us(20);
LCD_write_command(0x38); // - - ����8λ��ʽ��2�У�5x7
LCD_delay_10us(5);
LCD_write_command(0x0c); // - - ������ʾ���ع�꣬����˸
LCD_delay_10us(5);
LCD_write_command(0x06); // - - �趨���뷽ʽ����������λ
LCD_delay_10us(5);
LCD_write_command(0x01); // - - �����Ļ��ʾ
LCD_delay_50us(40);
}
//********************************

// - - ��LCD1602дָ��
void LCD_write_command(u8 dat)
{
LCD_delay_10us(5);
LCD_RS=0; // - - ָ��
LCD_RW=0; // - - д��
LCD_DB=dat;
LCD_delay_10us(5);
LCD_E=1; // - - ����
LCD_delay_10us(5);
LCD_E=0;
}

// - - ��LCD1602д����
void LCD_write_data(u8 dat) 
{
LCD_delay_10us(5);
LCD_RS=1;// - - ����
LCD_RW=0;// - - д��
LCD_DB=dat;
LCD_delay_10us(5);
LCD_E=1;// - - ����
LCD_delay_10us(5);
LCD_E=0;
}

// - - ������ʾλ��
void LCD_set_xy(u8 x,u8 y)
{
  u8 address;
  if(y==1)
  {
    address=0x80+x; // - - ��һ��λ��
  } else {
    address=0xc0+x; // - - �ڶ���λ��
  }
    LCD_delay_10us(5);
    LCD_write_command(address);
}

// - - ��ʾһ���ַ�����
void LCD_disp_char(u8 x,u8 y,u8 dat) // - - LCD_disp_char(0,1,0x38); // - - ��ʾ8
{
    LCD_set_xy(x,y);
    LCD_delay_10us(5);
    LCD_write_data(dat);
}

// - - ��ʾһ���ַ�������
void LCD_disp_string(u8 x,u8 y,u8 *s)
{
  LCD_set_xy(x,y);
  LCD_delay_10us(5);
  while(*s!='\0')
  {
    LCD_write_data(*s);
    s++;
  }
}
void display_rfid(u8 a,u8 b,u8 *p,u8 num)
 {

      u8 i;       	  
	  LCD_set_xy(a,b);             //��һ����ʾ����0~9	
      for(i=0;i<num;i++)
          LCD_write_data(p[i]);      //��ʾ��ĸ
              
 }

//********************************

void LCD_check_busy() //ʵ��֤�������ҵ�LCD1602�ϣ����æָ��ͨ���ʼ��ͣ���
{  //���ڲ�������ʹ��LCD�������û�����ü��æ��������ʹ
  do   //������ʱ�ķ�������ʱ���Ƿǳ����õġ�������һ�£���
  { LCD_E=0;   //forѭ������ʱ����ָͨ��ֻҪ1��ѭ�Ϳ���ɡ�����ָ��
    LCD_RS=0;                 //Ҫ��200��ѭ��������ɡ�   
    LCD_RW=1;
    LCD_DB=0xff;
    LCD_E=1;
  }
  while(LCD_DB^7==1);
}

void display_clear_line(u8 a)
 {
   u8 i;
   LCD_set_xy(a,0);
   for(i=0;i<16;i++)
   LCD_write_data(' ');
 }

void LCD_delay_10us(u16 n) // - - 10΢�����ʱ�ӳ���
{ 
  u16 i,j;
  for(i=n;i>0;i--)
    for(j=2;j>0;j--);  
} 

void LCD_delay_50us(u16 n) // - - 50΢�����ʱ�ӳ���
{ 
  u16 i,j;
  for(i=n;i>0;i--)
    for(j=22;j>0;j--);  
}
