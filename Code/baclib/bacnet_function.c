#include "stdint.h"
#include "types.h"
#include "define.h"
#include "usart.h"
#include "rs485.h"
#include "bacnet.h"
//#include "eepDefine.h"
#include "24cxx.h"
#include "modbus.h"
#include "inputs.h"
#include "define.h"
#include "filter.h"
#include "registerlist.h"
#include "store.h"


uint8_t panelname[20] ;



const u8 Variable_name[MAX_AVS][9] = {
					
					"MAC_ADD",
					"BAUDRATE",
					"PROTOCOL",
					"AI1_RANGE"	,
					"AI2_RANGE"	,
					"AI3_RANGE"	,
					"AI4_RANGE"	,
					"AI5_RANGE"	,
					"AI6_RANGE"	,
				};	
void switch_to_modbus(void)
{
//	printf()
	modbus.protocal = MODBUS;
//	write_eeprom(EEP_MODBUS_BACNET_SWITCH, MODBUS);	
	if(modbus.baudrate  == BAUDRATE_19200)
		uart1_init(19200);
	else if(modbus.baudrate  == BAUDRATE_9600)
		uart1_init(9600);
	else if(modbus.baudrate  == BAUDRATE_38400)
		uart1_init(38400);
	else if(modbus.baudrate  == BAUDRATE_57600)
		uart1_init(57600);
	else if(modbus.baudrate  == BAUDRATE_115200)
		uart1_init(115200);
	else if(modbus.baudrate  == BAUDRATE_76800)
		uart1_init(76800);
}

uint16_t send_count;
//u16 far Test[50];

uint8_t RS485_Get_Baudrate(void)
{
 if(modbus.baudrate == BAUDRATE_9600)
  return 5;
 else if(modbus.baudrate == BAUDRATE_19200)
  return 6;
 else if(modbus.baudrate == BAUDRATE_38400)
  return 7;
 else if(modbus.baudrate == BAUDRATE_57600)
  return 8;
 else if(modbus.baudrate == BAUDRATE_115200)
  return 9;
  else if(modbus.baudrate == BAUDRATE_76800)
  return 10;
 else 
  return 6;// default is 19200
}

//----------------------------------------------------------
void Get_AVS(void)
{
//	//bacnet_AV.reg.avs_num = 50;
//	bacnet_AV.reg.address = Modbus.address;
//	//	bacnet_AV.reg.product_model = Modbus.product_model;
//	bacnet_AV.reg.hardRev = Modbus.hardRev;
//	bacnet_AV.reg.firwareRev = SW_REV;
//	bacnet_AV.reg.tcp_type = Modbus.tcp_type;
//	memcpy(bacnet_AV.reg.ip_addr,Modbus.ip_addr,4);
//	memcpy(bacnet_AV.reg.mac_addr,Modbus.mac_addr,6);
//	memcpy(bacnet_AV.reg.subnet,Modbus.subnet,4);
//	memcpy(bacnet_AV.reg.getway,Modbus.getway,4);
//	bacnet_AV.reg.tcp_port = Modbus.tcp_port;
//	//	bacnet_AV.reg.mini_type = Modbus.mini_type;
//	memcpy(bacnet_AV.reg.com_config,Modbus.com_config,3);
//	bacnet_AV.reg.com_baudrate[0] = uart0_baudrate;
//	bacnet_AV.reg.com_baudrate[1] = uart1_baudrate;
//	bacnet_AV.reg.com_baudrate[2] = uart2_baudrate;
//	//	memcpy(bacnet_AV.reg.start_adc,Modbus.start_adc,11);
//	bacnet_AV.reg.network_number = Modbus.network_number;
//	bacnet_AV.reg.panel_number = Station_NUM;
//	
}
//modbus.input[0]
//----------------------------------------------
float Get_bacnet_value_from_buf(uint8_t type,uint8_t priority,uint8_t i)
{	
	switch(type)
	{
		case AV:
			if(i == 0) return Station_NUM ;
			else if(i==1) return modbus.baudrate ;
			else if(i==2) return modbus.protocal ;
			else if(i==3) return inputs[0].range ;
			else if(i==4) return inputs[1].range ;
			else if(i==5) return inputs[2].range ;
			else if(i==6) return inputs[3].range ;
			else if(i==7) return inputs[4].range ;
			else if(i==8) return inputs[5].range ;
		break ;
		case AI:
				if(inputs[i].digital_analog == 0 )
				{
					if(inputs[i].range <= LOW_HIGH )
					 {
							if(inputs[i].value >= 1000 )
							{
									inputs[i].value  = 1 ;
							}
							else
							{
									inputs[i].value  = 0 ;
							}
					 }
					 else
					 {
							if(inputs[i].value >= 1000 )
							{
								inputs[i].value  = 0 ;
							}
							else
							{
								inputs[i].value  = 1 ;
							}
					 }
					return  inputs[i].value ;						
			}
			else
			{	
				return ((float)inputs[i].value/1000) ;
			}
		
		break ;
		case AO:
			return ((float)outputs[i].value/1000) ;
		break;			
		default:
			return 0 ;
		break;
				
	}	

}

//------------------------------------------------------------
void wirte_bacnet_value_to_buf(uint8_t type,uint8_t priority,uint8_t i,int16_t value)
{


		switch(type)
		{
			case AV:
				if(i== 0)
				{
					//modbus.address = value ;
					Station_NUM = value ;
					AT24CXX_WriteOneByte((u16)EEP_STATION_NUM, value);
					Inital_Bacnet_Server();
					dlmstp_init(NULL);
					Recievebuf_Initialize(0);
				}
				else if(i== 1)
				{
					modbus.baudrate = value ;
					if(modbus.baudrate == BAUDRATE_9600)
					{
						modbus.baud = 0 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						uart1_init(BAUDRATE_9600);
						SERIAL_RECEIVE_TIMEOUT = 6;
					}
					else if(modbus.baudrate == BAUDRATE_19200)
					{
						modbus.baud = 1 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						uart1_init(BAUDRATE_19200);	
						SERIAL_RECEIVE_TIMEOUT = 3;
					}
					else if(modbus.baudrate == BAUDRATE_38400)
					{
						modbus.baud = 2 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						uart1_init(BAUDRATE_38400);
						SERIAL_RECEIVE_TIMEOUT = 2;
					}
					else if(modbus.baudrate == BAUDRATE_57600)
					{
						modbus.baud = 3 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						SERIAL_RECEIVE_TIMEOUT = 1;
						uart1_init(BAUDRATE_57600);
					}
					else if(modbus.baudrate == BAUDRATE_115200)
					{
						modbus.baud = 4 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						SERIAL_RECEIVE_TIMEOUT = 1;
						uart1_init(BAUDRATE_115200);
					}
					else if(modbus.baudrate == BAUDRATE_76800)
					{
						modbus.baud = 5 ;
						AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
						SERIAL_RECEIVE_TIMEOUT = 1;
						uart1_init(BAUDRATE_76800);
					}
	
				}
				else if(i== 2)
				{
					modbus.protocal = value ;
					if(modbus.protocal == MODBUS)
					AT24CXX_WriteOneByte((u16)EEP_MODBUS_COM_CONFIG, modbus.protocal);
				}
				else if((i>=3)&&(i<=8))
				{
					if(value < 31)
					{
						inputs[i-3].range = value ;
						inputs[i-3].digital_analog = 0 ;
					}
					else
					{
						inputs[i-3].range = value-30 ;
						inputs[i-3].digital_analog = 1 ;
					}
					write_page_en[1] = 1 ;
				}
			break;
			case AI:
			break;
			case BO:
			break;
			case AO:
			break;
	
			default:
			break;
		}			

}
//-------------------------------------------------
void write_bacnet_name_to_buf(uint8_t type,uint8_t priority,uint8_t i,char* str)
{

		switch(type)
		{
			case AI:
				memcpy(inputs[i].label,str,8);
				write_page_en[EN_IN] = 1 ;
			break;
			case AV:
			  memcpy(var[i].label,str,8);
			  write_page_en[EN_VAR] = 1 ;
			break;
			case AO:
				memcpy(outputs[i].label,str,8);
				write_page_en[EN_OUT] = 1 ;
			break;	
			default:
			break;
		}	
}
//---------------------------------------------------
void write_bacnet_unit_to_buf(uint8_t type,uint8_t priority,uint8_t i,uint8_t unit)
{
	U8_T temp;
//      switch(type)
//      {
//         case AV:
//		switch(i)
//		{
//			case 0:
//			case 1:
//			case 2:
//			break;				
//			default:
//			break;
//		}	 
//		break ;
//         default:
//         break;
//      }
}
//------------------------------------------------------------
char get_AM_Status(uint8_t type,uint8_t num)
{	
   	switch(type)
      {
		case AI:
		break ;	
		case AO:
		break;
		case BO:
		break;
		default:
		return 0 ;
         break;
      }
}
//------------------------------------------------------------
void write_bacent_AM_to_buf(uint8_t type,uint8_t i,uint8_t am)
{
		switch(type)
		{
			case AI:
				break;
			case BO:
				break;
			case AO:
				break;
	
			default:
			break;
		}
	
}
//------------------------------------------------------------
void add_remote_panel_db(uint32_t device_id,uint8_t panel)
{				
}
//------------------------------------------------------------

char* get_label(uint8_t type,uint8_t num)
{
	switch(type)
      {
         case AV: 
			if(num < MAX_AVS)
				return (char *)var[num].label;
         break;
         case AI:
			 if(num < MAX_AIS)
				return (char *)inputs[num].label;
			  break;
         case AO:
			 if(num < AOS)
				return (char *)outputs[num].label;
		 break;
         default:
         break;
      }
	  return "null";
}
char* get_description(uint8_t type,uint8_t num)
{
		switch(type)
      {
         case AV: 
			return (char *)var[num].description;
         break;
         case AI:
			 if(num < MAX_AIS)
				return (char *)inputs[num].description;
			 break;
         case AO:
			 if(num < AOS)
				return (char *)outputs[num].description;
			 break;
         default:
         break;
      }
	return "null";
}

char get_range(uint8_t type,uint8_t num)
{
 
	if(type == AV)   
	{
		if((num ==3)||(num == 4))    
		return UNITS_NO_UNITS ;
	}
	if(type == AO)
	{        
		return UNITS_VOLTS;
	}
	if(type == BO)   
	{
		return UNITS_NO_UNITS;   
	}

}

void Set_Object_Name(char * name)	
{
	memcpy(panelname,name,20);
}
void write_bacnet_description_to_buf(uint8_t type, uint8_t priority, uint8_t i, char* str)
{				
		switch(type)
		{
			case AI:
				memcpy(inputs[i].description,str,21);
				write_page_en[EN_IN] = 1 ;
				break;
			case AO:
				memcpy(outputs[i].description,str,21);
				write_page_en[EN_OUT] = 1 ;
				break;
			case AV:
				memcpy(var[i].description,str,sizeof(var[i].description) );
				write_page_en[EN_VAR] = 1 ;
				break;
			default:
			break;
		}
}	

char* Get_Object_Name(void)
{
	return (char*)panelname;
}
