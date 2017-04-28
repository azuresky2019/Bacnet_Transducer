//#include "key.h"
//#include "delay.h"
//#include "stdio.h"
////#include "include.h"
//#include "modbus.h"

//uint8_t pre_key_value = KEY_NON;
//uint8_t key_value = KEY_NON;
//uint8_t new_key = FALSE;
//uint8_t backlight_timer = 0 ; 

//  const struct
// {
//	GPIO_TypeDef* GPIOx;
//	uint16_t GPIO_Pin;
//	uint8_t PortSource;
//	uint8_t PinSource;
//	uint8_t NVIC_IRQChannel;
//	uint32_t Exit_line ; 
//	 
// } _STR_PLUSE_SETTING_[4] = 
// {
//	{GPIOD, GPIO_Pin_0, GPIO_PortSourceGPIOD, GPIO_PinSource0, EXTI0_IRQn, EXTI_Line0},
//	{GPIOD, GPIO_Pin_1, GPIO_PortSourceGPIOD, GPIO_PinSource1, EXTI1_IRQn, EXTI_Line1},
//	{GPIOD, GPIO_Pin_2, GPIO_PortSourceGPIOD, GPIO_PinSource2, EXTI2_IRQn, EXTI_Line2},
//	{GPIOD, GPIO_Pin_3, GPIO_PortSourceGPIOD, GPIO_PinSource3, EXTI3_IRQn, EXTI_Line3},
// };
//  void key_init(uint8_t channel)
// {
//	//u8 port_source ;
//	 
//	GPIO_InitTypeDef GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOD, ENABLE) ; 
//	GPIO_InitStructure.GPIO_Pin = _STR_PLUSE_SETTING_[channel].GPIO_Pin ;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(_STR_PLUSE_SETTING_[channel].GPIOx, &GPIO_InitStructure);
//	GPIO_SetBits(_STR_PLUSE_SETTING_[channel].GPIOx, _STR_PLUSE_SETTING_[channel].GPIO_Pin );
//		 
//	GPIO_EXTILineConfig(_STR_PLUSE_SETTING_[channel].PortSource, _STR_PLUSE_SETTING_[channel].PinSource ); 
//	EXTI_InitStructure.EXTI_Line  = _STR_PLUSE_SETTING_[channel].Exit_line ; 
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	
//	
//	NVIC_InitStructure.NVIC_IRQChannel = _STR_PLUSE_SETTING_[channel].NVIC_IRQChannel ;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure); 
// 
// }

// void EXTI0_IRQHandler(void)				//input10
//{
//	BL_ON() ;
//	new_key = TRUE;
//	if(EXTI->PR & (1 << 0))	//是0线的中断
//	{    
//		EXTI->PR = (1 << 0);	//清除LINE0上的中断标志位
//		 key_value = KEY_RIGHT  ;
//	}
//}
// void EXTI1_IRQHandler(void)				//input10
//{
//	new_key = TRUE;
//	if(EXTI->PR & (1 << 1))	//是0线的中断
//	{    
//		EXTI->PR = (1 << 1);	//清除LINE0上的中断标志位
//		 key_value = KEY_UP  ;
//	}
//}
// void EXTI2_IRQHandler(void)				//input10
//{
//	new_key = TRUE;
//	if(EXTI->PR & (1 << 2))	//是0线的中断
//	{    
//		EXTI->PR = (1 << 2);	//清除LINE0上的中断标志位
//		 key_value = KEY_DOWN  ;
//	}
//}
// void EXTI3_IRQHandler(void)				//input10
//{
//	new_key = TRUE;
//	if(EXTI->PR & (1 << 3))	//是0线的中断
//	{    
//		EXTI->PR = (1 << 3);	//清除LINE0上的中断标志位
//		 key_value = KEY_LEFT  ;
//	}
//}
// 

//void key_cope(void)
//{
////	if(key_freeze_timer)
////	{
////		key_freeze_timer--;
////		new_key = FALSE;
////	}
////	else
////	{
////		if(new_key == TRUE) // new key interrupt
////		{
////			backlight_timer = 50 ; 
////			if(pre_key_value != key_value)
////			{
////				key_freeze_timer = KEY_FREEZE_TIMER;
////				pre_key_value = key_value;
////			}
////			menu_keycope();
////			pre_key_value = key_value;
////			new_key = FALSE;
////		}
////		else
////		{
////			key_value = 0;
////			pre_key_value = key_value;
////			new_key = FALSE;
////		}
////	}
//}
