#ifndef __FPMXX_H__
#define __FPMXX_H__

extern bit status;
extern bit use_fingerprint;
extern bit ini_over;
extern bit is_lock;		
extern bit add_fingerprint;
extern bit del_fingerprint;
extern bit set_fingerprint;
extern bit open_just_now;
extern unsigned char errors[3];
extern unsigned char FPM10A_RECEICE_BUFFER[32];
void Delay_Us(int i);
void Delay_Ms_fpm(int m);
void Buzz_Times(unsigned char k);
void FPM10A_Cmd_Get_Img(void);
void FPM10A_Cmd_Check(void);
void Device_Check(void);
void FPM10A_Receive_Data(unsigned char ucLength);
void FPM10A_Delete_All_Fingerprint();
void FPM10A_Find_Fingerprint();
void FPM10A_Cmd_Search_Finger(void);
void FPM10A_Add_Fingerprint();
void FPM10A_Cmd_Reg_Model();
void FPM10A_Cmd_Save_Finger( unsigned int storeID );
void FINGERPRINT_Cmd_Delete_All_Finger(void);

#endif