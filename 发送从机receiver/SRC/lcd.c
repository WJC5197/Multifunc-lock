#include "lcd.h"
#include "reg52.h"
#define LCD_DB   P2  // - - P0   = DB0～DB7
sbit  LCD_RS=P0^5; // - - p2.0 = RS
sbit  LCD_RW=P0^6; // - - p2.1 = RW
sbit  LCD_E=P0^7;  // - - p2.2 = E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - 定义函数
#define u8 unsigned char
#define u16 unsigned int

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - 定义子程序函数
void LCD_init(void); // - - 初始化LCD1602函数
void LCD_write_command(u8 command); // - - 向LCD1602写指令函数
void LCD_write_data(u8 dat); // - - 向LCD1602写数据函数
void LCD_set_xy(u8 x,u8 y); // - - 设置LCD1602显示位置 X（0-16),y(1-2)
void LCD_disp_char(u8 x,u8 y,u8 dat); // - - 在LCD1602上显示一个字符
void LCD_disp_string(u8 X,u8 Y,u8 *s); // - - 在LCD1602上显示一个字符串
void LCD_check_busy(void);//检查忙函数。我没用到此函数，因为通过率极低。
void LCD_delay_10us(u16 n); // - - 10微秒的延时子程序
void LCD_delay_50us(u16 n); // - - 50微秒的延时子程序
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - 初始化LCD1602
void LCD_init(void)
{
LCD_delay_10us(20);
LCD_write_command(0x38); // - - 设置8位格式，2行，5x7
LCD_delay_10us(5);
LCD_write_command(0x0c); // - - 整体显示，关光标，不闪烁
LCD_delay_10us(5);
LCD_write_command(0x06); // - - 设定输入方式，增量不移位
LCD_delay_10us(5);
LCD_write_command(0x01); // - - 清除屏幕显示
LCD_delay_50us(40);
}
//********************************

// - - 向LCD1602写指令
void LCD_write_command(u8 dat)
{
LCD_delay_10us(5);
LCD_RS=0; // - - 指令
LCD_RW=0; // - - 写入
LCD_DB=dat;
LCD_delay_10us(5);
LCD_E=1; // - - 允许
LCD_delay_10us(5);
LCD_E=0;
}

// - - 向LCD1602写数据
void LCD_write_data(u8 dat) 
{
LCD_delay_10us(5);
LCD_RS=1;// - - 数据
LCD_RW=0;// - - 写入
LCD_DB=dat;
LCD_delay_10us(5);
LCD_E=1;// - - 允许
LCD_delay_10us(5);
LCD_E=0;
}

// - - 设置显示位置
void LCD_set_xy(u8 x,u8 y)
{
  u8 address;
  if(y==1)
  {
    address=0x80+x; // - - 第一行位置
  } else {
    address=0xc0+x; // - - 第二行位置
  }
    LCD_delay_10us(5);
    LCD_write_command(address);
}

// - - 显示一个字符函数
void LCD_disp_char(u8 x,u8 y,u8 dat) // - - LCD_disp_char(0,1,0x38); // - - 显示8
{
    LCD_set_xy(x,y);
    LCD_delay_10us(5);
    LCD_write_data(dat);
}

// - - 显示一个字符串函数
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
	  LCD_set_xy(a,b);             //第一行显示数字0~9	
      for(i=0;i<num;i++)
          LCD_write_data(p[i]);      //显示字母
              
 }

//********************************

void LCD_check_busy() //实践证明，在我的LCD1602上，检查忙指令通过率极低，以
{  //至于不能正常使用LCD。因此我没有再用检查忙函数。而使
  do   //用了延时的方法，延时还是非常好用的。我试了一下，用
  { LCD_E=0;   //for循环作延时，普通指令只要1次循就可完成。清屏指令
    LCD_RS=0;                 //要用200次循环便能完成。   
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

void LCD_delay_10us(u16 n) // - - 10微秒的延时子程序
{ 
  u16 i,j;
  for(i=n;i>0;i--)
    for(j=2;j>0;j--);  
} 

void LCD_delay_50us(u16 n) // - - 50微秒的延时子程序
{ 
  u16 i,j;
  for(i=n;i>0;i--)
    for(j=22;j>0;j--);  
}
