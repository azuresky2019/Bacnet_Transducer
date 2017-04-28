#ifndef __INPUTS_H
#define __INPUTS_H

#include "bitmap.h"
#include "stm32f10x_adc.h"
#include "define.h"
void inputs_init(void) ;
void ad_filter(void) ;

#define SAMPLE_TIME 50 //????50?
#define CHANNEL_NUM 6 //?12???


u16 ADC_getChannal(ADC_TypeDef* ADCx, u8 channal) ;
extern vu16 After_filter[CHANNEL_NUM]; 
extern vu16 AD_Value[SAMPLE_TIME][CHANNEL_NUM] ;
// void pulse_set(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) ;
 void pulse_set(uint8_t channel) ;


extern uint32_t input_timeout[6] ;
extern uint8_t first_trrigle[6] ;
extern uint8_t second_trrigle[6] ;
extern uint16_t input_timer[6] ;
extern uint8_t  input_flag[6] ;
#endif































