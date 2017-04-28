#include <string.h>
#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
//#include "key.h"
#include "24cxx.h"
#include "spi.h"

#include "flash.h"
#include "stmflash.h"

#include "mmc_sd.h"
#include "dma.h"
#include "vmalloc.h"
#include "ff.h"
#include "fattester.h"
#include "exfuns.h"
#include "timerx.h"
//#include "uip.h"
//#include "uip_arp.h"
//#include "tapdev.h"
//#include "usb_app.h"
//#include "ai.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "modbus.h"
#include "define.h"
#include "inputs.h"
#include "../output/output.h"
#include "dlmstp.h"
#include "rs485.h"
#include "datalink.h"
#include "config.h"
//#include "handlers.h"
#include "device.h"	
#include "registerlist.h"
#include "../filter/filter.h"
#include "../KEY/key.h"
#include "bacnet.h"
#include "store.h"
#include "watchdog.h"
//#include "include.h"
u8 store_flag = 0 ;
u16 Test[50] ; //define this for bacnet lib



static void vCOMMTask( void *pvParameters );

//static void vUSBTask( void *pvParameters );

//void vKEY_Task( void *pvParameters );
void vOUTPUTSTask( void *pvParameters );
static void vMSTP_TASK(void *pvParameters ) ;
static void vLED0Task( void *pvParameters );
static void vFLASHSTask( void *pvParameters ) ;
static void  vINPUTSTask(void *pvParameters ) ;
//void uip_polling(void);

//#define	BUF	((struct uip_eth_hdr *)&uip_buf[0])	
	
u32 Instance = 0x0c;
uint8_t  PDUBuffer[MAX_APDU];



static void debug_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8008000);
	debug_config();
	TIM_DeInit(TIM3);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
 	delay_init(72);		
	EEP_Dat_Init();	
	//Lcd_Initial();
	SPI1_Init();
	SPI2_Init();
	watchdog_init();
//	mem_init(SRAMIN);
//	TIM3_Int_Init(5000, 7199);

	TIM7_Int_Init(9, 7199);//1ms
	printf("T3_series\n\r");
 	xTaskCreate( vCOMMTask, ( signed portCHAR * ) "COMM", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );
	xTaskCreate( vOUTPUTSTask, ( signed portCHAR * ) "OUTPUT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( vINPUTSTask, ( signed portCHAR * ) "INPUT", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( vLED0Task, ( signed portCHAR * ) "LED0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
	xTaskCreate( vMSTP_TASK, ( signed portCHAR * ) "MSTP", configMINIMAL_STACK_SIZE + 256  , NULL, tskIDLE_PRIORITY + 4, NULL );
	xTaskCreate( vFLASHSTask, ( signed portCHAR * ) "FLASH", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY +3, NULL );
	vTaskStartScheduler();
}




void vINPUTSTask(void *pvParameters )
{
	u8 i ;
	for(i=0; i<6; i++)
	{
		pulse_set(i) ;
	}
	inputs_init() ;
	for( ;; )
	{
		ad_filter() ;
		delay_ms(500);
	
	}
}


void vCOMMTask(void *pvParameters )
{
	modbus_init();
	for( ;; )
	{
		if (dealwithTag)
		{  
		 dealwithTag--;
		  if(dealwithTag == 1)//&& !Serial_Master )	
			dealwithData();
		}
		if(serial_receive_timeout_count>0)  
		{
				serial_receive_timeout_count -- ; 
				if(serial_receive_timeout_count == 0)
				{
					serial_restart();
				}
		}
		IWDG_ReloadCounter();
		delay_ms(5) ;
		
	}	
}

void vLED0Task( void *pvParameters )
{	
	LED_Init();
	for( ;; )
	{
		tabulate_LED_STATE() ;
		delay_ms(100);
	}
}

void vFLASHSTask( void *pvParameters )
{

	mass_flash_init() ;
	for( ;; )
	{
		Flash_Write_Mass() ;
		delay_ms(1000);
	}	
}

void vOUTPUTSTask( void *pvParameters )
{

	output_init();
	led_output_init() ;
	for( ;; )
	{
		output_refresh();
		delay_ms(500);
	}	
}


void Inital_Bacnet_Server(void)
{

Set_Object_Name("PWM");	
 Device_Init();
 Device_Set_Object_Instance_Number(Instance);  
 address_init();
// bip_set_broadcast_addr(0xffffffff);

#if  READ_WRITE_PROPERTY   
  AIS = 6;
  AOS = 8;
  BOS = 6;
  AVS = MAX_AVS;

#endif	
}
void vMSTP_TASK(void *pvParameters )
{
	uint16_t pdu_len = 0; 
	BACNET_ADDRESS  src;
	TIM6_Int_Init(9, 7199);//1ms
	Inital_Bacnet_Server();
	dlmstp_init(NULL);
	Recievebuf_Initialize(0);
	for (;;)
    {
			
		if(modbus.protocal == BAC_MSTP)
		{
					pdu_len = datalink_receive(&src, &PDUBuffer[0], sizeof(PDUBuffer), 0,	modbus.protocal);
					if(pdu_len) 
						{
				        npdu_handler(&src, &PDUBuffer[0], pdu_len, BAC_MSTP);	
						} 
						
		}
	    delay_ms(5);
	}
	
}
void EEP_Dat_Init(void)
{
		u8 loop ;
//		u8 temp[6]; 
		AT24CXX_Init();
		modbus.serial_Num[0] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_LOWORD);
		modbus.serial_Num[1] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_LOWORD+1);
		modbus.serial_Num[2] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_HIWORD);
		modbus.serial_Num[3] = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_HIWORD+1);
	
		if((modbus.serial_Num[0]==0xff)&&(modbus.serial_Num[1]== 0xff)&&(modbus.serial_Num[2] == 0xff)&&(modbus.serial_Num[3] == 0xff))
		{
					modbus.serial_Num[0] = 1 ;
					modbus.serial_Num[1] = 1 ;
					modbus.serial_Num[2] = 2 ;
					modbus.serial_Num[3] = 2 ;
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD, modbus.serial_Num[0]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+1, modbus.serial_Num[1]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+2, modbus.serial_Num[2]);
					AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+3, modbus.serial_Num[3]);
		}
		Instance = modbus.serial_Num[0] + (U16_T)(modbus.serial_Num[1] << 8);
		AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_LO, SOFTREV&0XFF);
		AT24CXX_WriteOneByte(EEP_VERSION_NUMBER_HI, (SOFTREV>>8)&0XFF);
		modbus.address = AT24CXX_ReadOneByte(EEP_ADDRESS);
		if((modbus.address == 255)||(modbus.address == 0))
		{
					modbus.address = 254 ;
					AT24CXX_WriteOneByte(EEP_ADDRESS, modbus.address);
		}
//		modbus.product = PRODUCT_ID ;
		Station_NUM = AT24CXX_ReadOneByte(EEP_STATION_NUM);
		if(Station_NUM > 127)
		{
					Station_NUM = 3 ;
		}
		modbus.hardware_Rev = AT24CXX_ReadOneByte(EEP_HARDWARE_REV);
		if((modbus.hardware_Rev == 255)||(modbus.hardware_Rev == 0))
		{
					modbus.hardware_Rev = HW_VER ;
		//			AT24CXX_WriteOneByte(EEP_HARDWARE_REV, modbus.hardware_Rev);
		}
		modbus.update = AT24CXX_ReadOneByte(EEP_UPDATE_STATUS);
		modbus.SNWriteflag = AT24CXX_ReadOneByte(EEP_SERIALNUMBER_WRITE_FLAG);
		
		modbus.baud = AT24CXX_ReadOneByte(EEP_BAUDRATE);
		if(modbus.baud > 5) 
		{	
			modbus.baud = 1 ;
//			AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
		}
		switch(modbus.baud)
				{
					case 0:
						modbus.baudrate = BAUDRATE_9600 ;
						SERIAL_RECEIVE_TIMEOUT = 6;
					break ;
					case 1:
						modbus.baudrate = BAUDRATE_19200 ;
						SERIAL_RECEIVE_TIMEOUT = 3;
					break;
					case 2:
						modbus.baudrate = BAUDRATE_38400 ;
						SERIAL_RECEIVE_TIMEOUT = 2;
					break;
					case 3:
						modbus.baudrate = BAUDRATE_57600 ;
						SERIAL_RECEIVE_TIMEOUT = 1;
					break;
					case 4:
						modbus.baudrate = BAUDRATE_115200 ;
						SERIAL_RECEIVE_TIMEOUT = 1;
					break;
					case 5:
						modbus.baudrate = BAUDRATE_76800 ;	
						SERIAL_RECEIVE_TIMEOUT = 1;
					break;
					default:
						modbus.baudrate = BAUDRATE_19200 ;
						SERIAL_RECEIVE_TIMEOUT = 3;
					break ;				
				}
				uart1_init(modbus.baudrate);

				modbus.protocal = MODBUS ;											

}









