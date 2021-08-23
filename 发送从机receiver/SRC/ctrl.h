
#ifndef  __ctrl_h__
#define  __ctrl_h__



#define ERROR_NONE			0x00	//����
#define	ERROR_NOMONEY		0x01	//����
#define	ERROR_NOCMD			0x02	//��Ч����
#define	ERROR_CHECK			0x03	//У�����
#define ERROR_KEYA_IC 		0x04	//�������
#define	ERROR_READ_IC  		0x05	//����ʧ��
#define	ERROR_WRITE_IC  	0x06	//д��ʧ��
#define ERROR_WRITE_KEY_IC	0x07	//�޸�����ʧ��
#define ERROR_SET_CSTSN		0x08	//���ÿͻ�����ʧ��
#define ERROR_NOCARD		0x09	//IC��������
#define	ERROR_ATCLL			0x0A	//����ͻʧ��
#define ERROR_SLCT			0x0B	//ѡ��ʧ��
#define	ERROR_BLOCK_ADDR	0x0C	//�����
#define	ERROR_OP_TYPE		0x0D	//������ʽ����	

extern bit rc522_state;
extern bit rc522_manage;
extern u8 RC522_KeyValue;
void uart_over( void );
void RC522_KeyDown(void);
void ctrl_process( void );
extern bit sounder_beep;
extern u8 KEY_BUF[8];
extern u8 Card_saved_asc[128];

#endif