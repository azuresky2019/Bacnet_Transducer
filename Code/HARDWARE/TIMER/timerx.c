#include "timerx.h"
#include "led.h"
#include "modbus.h"
#include "lcd.h"
#include "inputs.h"

#define TEN_MIN  90000
#define TEN_SEC	 10000

extern volatile uint16_t SilenceTime;
uint32_t Frquence_count[6] ;
uint16_t input_timer[6] ;
uint16_t output_value[6] ;
uint8_t  input_flag[6] = {0};

uint8_t refresh_flag[6]={0} ;
//u32 dhcp_run_time = 30000;
void TIM7_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	
	//Timer3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);
}


//��ʱ��7�жϷ������	 
void TIM7_IRQHandler(void)
{ 		    		  			    
	if(TIM_GetFlagStatus(TIM7, TIM_IT_Update) == SET)
	{
	
		uint8_t	i  ;
		TIM_ClearFlag(TIM7, TIM_IT_Update);	
		for(i=0; i<6; i++)
		{
//			input_timer[i]++ ;
			input_timeout[i]++ ;
			if(first_trrigle[i]	== 1) Frquence_count[i]++ ;
			if(second_trrigle[i]	== 1)
			{						
				if((Frquence_count[i] >= 14)&&(Frquence_count[i] <= 23))
				 {	
					if(input_flag[i] == 0)
					{
						input_timer[i]= 0 ;
					}
					input_flag[i] = 1 ;		 
				 }
				 second_trrigle[i] = 0 ;
				 first_trrigle[i] = 0 ;
				 Frquence_count[i] = 0 ;				 
			}
			if(input_timeout[i] > 23)	
			{			
					first_trrigle[i] = 0 ;	
					input_flag[i] = 0 ;	
					Frquence_count[i] = 0;
			}
			if(input_timeout[i] >= TEN_MIN) 
			{
				input_timer[i] = 0 ;
				output_value[i] = 0 ;
				input_timeout[i] = 0 ;
				input_flag[i] = 0 ;
				refresh_flag[i] = 1 ;
			}
			if(input_flag[i] == 1)
			{			
				input_timer[i]++ ;
				output_value[i] = (input_timer[i]-1) ;
				if(input_timer[i] > TEN_SEC) 
				{
					input_timer[i] = TEN_SEC ;
					refresh_flag[i] = 1 ;
				}
				else
				{
					refresh_flag[i] = 0 ;
				}
			}
			else
			{

					refresh_flag[i] = 1 ;
			}
//				Test_Flag[3+i] = input_flag[i] ;
				
		}		
	}
//	TIM3->SR &= ~(1 << 0);		//����жϱ�־λ 
	
}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
//void TIM3_Int_Init(u16 arr, u16 psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//	
//	TIM_TimeBaseStructure.TIM_Period = arr;
//	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//	
//	//Timer3 NVIC ����
//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
//	
//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	TIM_Cmd(TIM3, ENABLE);
//}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
//void TIM3_PWM_Init(u16 arr, u16 psc)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE); //TIM3_CH1->PC6, TIM3_CH2->PC7, TIM3_CH3->PC8, TIM3_CH4->PC9
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;  //TIM3_CH1-4
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	TIM_TimeBaseStructure.TIM_Period = arr;
//	TIM_TimeBaseStructure.TIM_Prescaler = psc;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//	
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
//	
//	TIM_Cmd(TIM3, ENABLE);									  
//}  	 

//void set_pwm_duty(TIM_TypeDef* TIMx, u8 channel, u16 duty)
//{
//	u16 compare_value = 0;
//	// calculate the compare value and then set it to change the pwm duty
//	
//	switch(channel)
//	{
//		case 1:
//			TIM_SetCompare1(TIMx, compare_value);
//		break;
//		case 2:
//			TIM_SetCompare2(TIMx, compare_value);
//		break;
//		case 3:
//			TIM_SetCompare3(TIMx, compare_value);
//		break;
//		case 4:
//			TIM_SetCompare4(TIMx, compare_value);
//		break;
//	}
//}

///////////////////////////////////////////////////////////////////////////////////////////
//u32 uip_timer = 0;	//uip ��ʱ����ÿ10ms����1.
//��ʱ��6�жϷ������	 
void TIM6_IRQHandler(void)//1ms
{

//	if(TIM_GetFlagStatus(TIM6, TIM_IT_Update) == SET)
//	{
//		uip_timer++;		//uip��ʱ������1
//	}
	if(SilenceTime < 1000)
	{
		//SilenceTime ++ ;
		//miliseclast_cur = miliseclast_cur + SWTIMER_INTERVAL;
		SilenceTime = SilenceTime + SWTIMER_INTERVAL;
	}
	else
	{
			SilenceTime = 0 ;
	}
	TIM_ClearFlag(TIM6, TIM_IT_Update);		
}

//������ʱ��6�жϳ�ʼ��					  
//arr���Զ���װֵ��		
//psc��ʱ��Ԥ��Ƶ��		 
//Tout= ((arr+1)*(psc+1))/Tclk��
void TIM6_Int_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	
	//Timer3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, ENABLE);
	
//	RCC->APB1ENR |= 1 << 4;					//TIM6ʱ��ʹ��    
// 	TIM6->ARR = arr;  						//�趨�������Զ���װֵ 
//	TIM6->PSC = psc;  			 			//����Ԥ��Ƶ��.
// 	TIM6->DIER |= 1 << 0;   				//��������ж�				
// 	TIM6->CR1 |= 0x01;    					//ʹ�ܶ�ʱ��6
//	MY_NVIC_Init(0, 0, TIM6_IRQn, 2);		//��ռ1�������ȼ�2����2		
}
