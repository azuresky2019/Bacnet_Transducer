#ifndef __GP22_H
#define __GP22_H
#include "bitmap.h"
#define DN20            6.95
#define DN25            8.05
#define DN32            11.3
#define DN40            14
#define DN50            18

#define DN20_L            (62)			//mm
#define DN25_L            (80) 		//mm
#define DN32_L            80
#define DN40_L            80
#define DN50_L            80


#define	GP22_SET_RST()	(GPIOB->BSRR = GPIO_Pin_10)   
#define GP22_CLR_RST()	(GPIOB->BRR = GPIO_Pin_10) 

#define	GP22_SET_CS()	(GPIOB->BSRR = GPIO_Pin_12)   
#define GP22_CLR_CS()	(GPIOB->BRR = GPIO_Pin_12) 

#define GP22_SET_CLK()	(GPIOB->BSRR = GPIO_Pin_13)
#define GP22_CLR_CLK()	(GPIOB->BRR = GPIO_Pin_13) 

#define GP22_SET_MOSI()	(GPIOB->BSRR = GPIO_Pin_15) 
#define GP22_CLR_MOSI() (GPIOB->BRR = GPIO_Pin_15) 



#define GP22_READ_MISO() (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
#define GP22_READ_INIT() (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11))


void spi_write32(uint8_t address, uint32_t data_out);
void gp22_io_init(void) ;
void testcomunication(void) ;
void gp22_device_init(void) ;
void CalStart(void) ;
void Read_Flow(void);
void measurement(void) ;
void ReadGP2Data(void);
void Measure_Temp(void) ;
 uint32_t Get_Temp(uint32_t R_Buf) ;
 uint32_t Cal_Flow(void) ;
 
 extern  uint32_t gp22_data[4] ; 
 extern uint16_t gp22_status  ;
 
 
























#endif
