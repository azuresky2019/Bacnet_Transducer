#include "led.h"
#include "modbus.h"
#include "controls.h"
#include "inputs.h"


 

//u8 dim_timer_setting[28];
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE, GPIO_InitStructure.GPIO_Pin);	
	
}
/*****************************LED TABLE------T3-22AI**************************************/
/*
PE14 = 0
PE15 = 1
PE0-->HEATBEAT 
PE1-->INPUT0 PE2-->INPUT1 PE3-->INPUT2 PE4-->INPUT3 PE5-->INPUT4 PE6-->INPUT5 
PE7-->INPUT6 PE8-->INPUT7 PE9-->INPUT8 PE10-->INPUT9 PE11-->INPUT10 PE12-->TX PE13-->RX  

PE14 = 1
PE15 = 0
PE0-->HEATBEAT 
PE1-->INPUT11 PE2-->INPUT12 PE3-->INPUT13 PE4-->INPUT14 PE5-->INPUT15 PE6-->INPUT16 
PE7-->INPUT17 PE8-->INPUT18 PE9-->INPUT19 PE10-->INPUT20 PE11-->INPUT21 PE12-->TX PE12-->RX  
*/
/****************************************************************************/

void tabulate_LED_STATE(void)
{

  static u16 beat_count = 0;	
  static u8 hb_switch = 0;
  u8 i = 0 ;
  beat_count ++ ;
  if(beat_count >= 10 ) 
  {
	beat_count = 0 ;
    if(hb_switch == 0)
	{
		HB = LED_ON ;
		hb_switch = 1  ;
	}
	else
	{
	    if(hb_switch == 1)
		{
			HB = LED_OFF ;
			hb_switch = 0  ;
		}
	}
  }
  for(i=0; i < 6; i++)
  {
	if(input_flag[i] == 1)
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_0<<i) ;
	}
	else
	{
	   GPIO_SetBits(GPIOE,  GPIO_Pin_0<<i) ;
	}
  }
}




