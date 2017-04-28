#ifndef __TIMERX_H
#define __TIMERX_H

#include "stm32f10x.h"


//V1.1 20120904
//1,����TIM3_PWM_Init������
//2,����LED0_PWM_VAL�궨�壬����TIM3_CH2����									  
//////////////////////////////////////////////////////////////////////////////////  


//ͨ���ı�TIM3->CCR2��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define LED0_PWM_VAL	TIM3->CCR2    
#define SWTIMER_INTERVAL	1
//void TIM3_Int_Init(u16 arr, u16 psc);
//void TIM3_PWM_Init(u16 arr, u16 psc);
void TIM7_Int_Init(u16 arr, u16 psc) ;
//extern u32 uip_timer;
void TIM6_Int_Init(u16 arr, u16 psc);
//void uip_polling(void) ;
//extern u32 dhcp_run_time  ;
extern uint8_t refresh_flag[6] ;
extern uint32_t Frquence_count[6] ;
extern uint16_t output_value[6] ;
#endif