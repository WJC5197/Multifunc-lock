# define u8 unsigned char
# define u16 unsigned int
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LCD_init(void); 
void LCD_write_command(u8 command);
void LCD_write_data(u8 dat); 
void LCD_set_xy(u8 x,u8 y);
void LCD_disp_char(u8 x,u8 y,u8 dat); 
void LCD_disp_string(u8 X,u8 Y,u8 *s);
void LCD_check_busy(void);
void LCD_delay_10us(u16 n);
void LCD_delay_50us(u16 n);
void display_clear_line(u8 a);
void display_rfid(u8 a,u8 b,u8 *p,u8 num);