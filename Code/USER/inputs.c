
#include "define.h"
#include "inputs.h"

#include "delay.h"
#include "modbus.h"
#include "../filter/filter.h"
#include "controls.h"

uint32_t input_timeout[6] ;
uint8_t first_trrigle[6] ;
uint8_t second_trrigle[6] ;


vu16 AD_Value[SAMPLE_TIME][CHANNEL_NUM] = {0}; 
vu16 After_filter[CHANNEL_NUM]; //?????????????



void range_set_func(u8 channel) ;
 
#define ADC_DR_ADDRESS  0x4001244C  



 

void inputs_io_init(void)
{	
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //??????
GPIO_Init(GPIOC, &GPIO_InitStructure);	
}
void ADC1_Configuration(void)
{
ADC_InitTypeDef ADC_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
RCC_ADCCLKConfig(RCC_PCLK2_Div6);
ADC_DeInit(ADC1); //??? ADC1 ????????????

ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC????:ADC1?ADC2???????
ADC_InitStructure.ADC_ScanConvMode =ENABLE; //???????????
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //?????????????
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //????????
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC?????
ADC_InitStructure.ADC_NbrOfChannel = CHANNEL_NUM; //?????????ADC?????
ADC_Init(ADC1, &ADC_InitStructure); //??ADC_InitStruct???????????ADCx????


//????ADC??????,??????????????
//ADC1,ADC??x,????????y,?????239.5??

ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );
ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5 );
ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5 );
ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5 );
ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_239Cycles5 );
ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_239Cycles5 );

// ??ADC?DMA??(???DMA??,??????DMA?????)
ADC_DMACmd(ADC1, ENABLE);


ADC_Cmd(ADC1, ENABLE); //?????ADC1

ADC_ResetCalibration(ADC1); //?????ADC1??????

while(ADC_GetResetCalibrationStatus(ADC1)); //??ADC1??????????,???????


ADC_StartCalibration(ADC1); //????ADC1?????

while(ADC_GetCalibrationStatus(ADC1)); //????ADC1?????,???????
}

void DMA_Configuration(void)
{

DMA_InitTypeDef DMA_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //??DMA??	
DMA_DeInit(DMA1_Channel1); //?DMA???1?????????
DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)ADC_DR_ADDRESS; //DMA??ADC???
DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value; //DMA?????
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //????????????
DMA_InitStructure.DMA_BufferSize = CHANNEL_NUM*SAMPLE_TIME; //DMA???DMA?????
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //?????????
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //?????????
DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //?????16?
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //?????16?
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //?????????
DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA?? x??????
DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA??x????????????
DMA_Init(DMA1_Channel1, &DMA_InitStructure); //??DMA_InitStruct?????????DMA???
	
DMA_Cmd(DMA1_Channel1, ENABLE);
	
}

void ad_filter(void)
{
	uint32_t sum = 0;
	u8 count;
	u8 i ;
	for(i=0;i<6;i++)
	{
		for ( count=0;count<SAMPLE_TIME;count++)
		{
			sum += AD_Value[count][i];
		}
		After_filter[i]=sum/SAMPLE_TIME;
		sum=0;
	}

}
void inputs_init(void) 
{
	inputs_io_init();
	DMA_Configuration() ;
	ADC1_Configuration();
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


  const struct
 {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint8_t PortSource;
	uint8_t PinSource;
	uint8_t NVIC_IRQChannel;
	uint32_t Exit_line ; 
	 
 } _STR_PLUSE_SETTING_[6] = 
 {
	{GPIOB, GPIO_Pin_3, GPIO_PortSourceGPIOB, GPIO_PinSource3, EXTI3_IRQn, EXTI_Line3},
	{GPIOB, GPIO_Pin_4, GPIO_PortSourceGPIOB, GPIO_PinSource4, EXTI4_IRQn, EXTI_Line4},
	{GPIOB, GPIO_Pin_5, GPIO_PortSourceGPIOB, GPIO_PinSource5, EXTI9_5_IRQn, EXTI_Line5},
	{GPIOB, GPIO_Pin_6, GPIO_PortSourceGPIOB, GPIO_PinSource6, EXTI9_5_IRQn, EXTI_Line6},
	{GPIOB, GPIO_Pin_7, GPIO_PortSourceGPIOB, GPIO_PinSource7, EXTI9_5_IRQn, EXTI_Line7},
	{GPIOB, GPIO_Pin_8, GPIO_PortSourceGPIOB, GPIO_PinSource8, EXTI9_5_IRQn, EXTI_Line8},
 };
  void pulse_set(uint8_t channel)
 {
	//u8 port_source ;
	 
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	GPIO_InitStructure.GPIO_Pin = _STR_PLUSE_SETTING_[channel].GPIO_Pin ;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(_STR_PLUSE_SETTING_[channel].GPIOx, &GPIO_InitStructure);
	GPIO_SetBits(_STR_PLUSE_SETTING_[channel].GPIOx, _STR_PLUSE_SETTING_[channel].GPIO_Pin );
		 
	GPIO_EXTILineConfig(_STR_PLUSE_SETTING_[channel].PortSource, _STR_PLUSE_SETTING_[channel].PinSource ); 
	EXTI_InitStructure.EXTI_Line  = _STR_PLUSE_SETTING_[channel].Exit_line ; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = _STR_PLUSE_SETTING_[channel].NVIC_IRQChannel ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
 
 }


void EXTI3_IRQHandler(void)											//input5
{
	if(EXTI->PR & (1 << 3))	//是3线的中断
	{    
		EXTI->PR = (1 << 3);	//清除LINE3上的中断标志位
		input_timeout[5] = 0 ;
		if(first_trrigle[5] == 0) first_trrigle[5] = 1 ;
		else  second_trrigle[5] = 1 ;
//		input_timer[5]++ ;
	}
}
void EXTI4_IRQHandler(void)											//input5
{
	if(EXTI->PR & (1 << 4))	//是3线的中断
	{    
		EXTI->PR = (1 << 4);	//清除LINE3上的中断标志位
		input_timeout[4] = 0 ;
		if(first_trrigle[4] == 0) first_trrigle[4] = 1 ;
		else  second_trrigle[4] = 1 ;
//		input_timer[4]++ ;
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & (1 << 8))	//是7线的中断
	{      
		EXTI->PR  = (1 << 8);	//清除LINE7上的中断标志位
		input_timeout[0] = 0 ;
		if(first_trrigle[0] == 0) first_trrigle[0] = 1 ;
		else  second_trrigle[0] = 1 ;
//		input_timer[0]++ ;
	}
	if(EXTI->PR & (1 << 7))	//是7线的中断
	{      
		EXTI->PR  = (1 << 7);	//清除LINE7上的中断标志位
		input_timeout[1] = 0 ;
		if(first_trrigle[1] == 0) first_trrigle[1] = 1 ;
		else  second_trrigle[1] = 1 ;
//		input_timer[1]++ ;
	}
	if(EXTI->PR & (1 << 6))	//是6线的中断
	{      
		EXTI->PR  = (1 << 6);	//清除LINE6上的中断标志位
	    input_timeout[2] = 0 ;
		if(first_trrigle[2] == 0) first_trrigle[2] = 1 ;
		else  second_trrigle[2] = 1 ;
//		input_timer[2]++ ;
	}
	if(EXTI->PR & (1 << 5))	//是5线的中断
	{      
		EXTI->PR  = (1 << 5);	//清除LINE5上的中断标志位 
		input_timeout[3] = 0 ;
		if(first_trrigle[3] == 0) first_trrigle[3] = 1 ;
		else  second_trrigle[3] = 1 ;
//		input_timer[3]++ ;
	}
} 



 