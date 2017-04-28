#ifndef __MODBUS_H
#define	__MODBUS_H
#include <string.h>
#include "stm32f10x.h"
//IO操作函数	 
#include "bitmap.h"
#include "crc.h"
#include "define.h"
#include "filter.h"
#include "delay.h"
#include "registerlist.h"
#ifndef FALSE
#define FALSE 0 
#endif 

#ifndef TRUE
#define TRUE 1 
#endif 

#define TXEN		PAout(8)


#define SEND			1			//1
#define	RECEIVE		0

#define	READ_VARIABLES				0x03
#define	WRITE_VARIABLES				0x06
#define	MULTIPLE_WRITE				0x10
#define	CHECKONLINE					0x19

#define DATABUFLEN					200
#define DATABUFLEN_SCAN				12
#define SENDPOOLLEN         		8








#define SERIAL_COM_IDLE				0
#define INVALID_PACKET				1
#define VALID_PACKET				2

#define USART_REC_LEN  			256  	//定义最大接收字节数 200
#define USART_SEND_LEN			256

#define RESPONSERANDVALUE	1


extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern vu8 revce_count ;
extern u8 dealwithTag ;

void serial_restart(void);
void modbus_data_cope(u8 XDATA* pData, u16 length, u8 conn_id) ;
void modbus_init(void) ;
void SoftReset(void) ;
 typedef union
{
	uint8_t qauter_temp[4];
	uint16_t half_temp[2];
	float temp;
				
} _FLOAT_VALUE_;


typedef struct 
{
	u8 serial_Num[4];
//	u16 software ;
	u8 address ;
	u32 baudrate ;
	u8	baud ;
	u8 update ;
	u8 product ;
	u8 hardware_Rev;
	u8 SNWriteflag ;
	u8 com_config;
	u8 protocal ;
	u8 reset ;

//
	
}STR_MODBUS ;
extern u8 store_flag  ;
extern STR_MODBUS modbus ;
extern vu8 serial_receive_timeout_count ;
 void dealwithData(void) ;
//extern u8 i2c_test[10] ; 
void send_byte(u8 ch, u8 crc) ;
void responseCmd(u8 type, u8* pData); 
void internalDeal(u8 type,  u8 *pData) ;
void USART_SendDataString(u16 num) ;
extern u8 uart_send[USART_SEND_LEN] ;
extern u8 SERIAL_RECEIVE_TIMEOUT ;
extern u8  	Station_NUM;
extern uint16_t switch_Count ;
void Inital_Bacnet_Server(void);
void EEP_Dat_Init(void) ;
void pipe_size(uint8_t pipe_type) ;
//u16 swap_int16( u16 value) ;
void urat_print(char* str) ;
#endif
