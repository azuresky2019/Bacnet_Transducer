#ifndef __REGISTERLIST_H
#define	__REGISTERLIST_H

#include "define.h"
#include "controls.h"





typedef enum
{
	EEP_SERIALNUMBER_LOWORD = 0,
	EEP_SERIALNUMBER_HIWORD = EEP_SERIALNUMBER_LOWORD + 2,
	EEP_VERSION_NUMBER_LO = EEP_SERIALNUMBER_HIWORD + 2,
	EEP_VERSION_NUMBER_HI,
	EEP_ADDRESS,
	EEP_PRODUCT_MODEL,
	EEP_HARDWARE_REV,
	EEP_PIC_VER,
	EEP_BASE_ADDRESS,

	EEP_UPDATE_STATUS = 16,
	EEP_SERIALNUMBER_WRITE_FLAG,
	EEP_MODBUS_COM_CONFIG = 20 , 
	
	EEP_BAUDRATE ,
	EEP_INSTANCE_1 ,
	EEP_INSTANCE_2 ,
	EEP_INSTANCE_3 ,
	EEP_INSTANCE_4 ,
	EEP_STATION_NUM ,
	
	EEP_FLOW_UNIT = 30 ,
	EEP_TEMP_UNIT =  31 ,
	EEP_PIPE_TYPE = 32 ,
	EEP_FLOW_OFFSET = 33 ,
	EEP_TEMPUP_OFFSET= 35 ,
	EEP_TEMPDOWN_OFFSET = 37 ,
	EEP_DIS_TIME		=  39 ,	
	EEP_FLOW_TOTAL_LO   = 40 ,
	EEP_FLOW_TOTAL_HI	= 42 ,
	EEP_HEAT_TOTAL_LO	= 44 ,	
	EEP_HEAT_TOTAL_HI	= 46 ,
	EEP_FLOW_RATE_MIN	= 48, 
	EEP_FLOW_RATE_MAX	= 50,
	EEP_FLOW_VALUE_FOR_PULSE = 52 ,
	EEP_K_FACTOR_LO	=   53 ,
	EEP_K_FACTOR_HI	=   54 ,
	
	EEP_FAC_CAL_POINT		,
	EEP_REAL_RD0			  ,
	EEP_METER_RD0	= EEP_REAL_RD0+2 ,
	EEP_REAL_RD1	= EEP_METER_RD0+2	,
	EEP_METER_RD1	= EEP_REAL_RD1+2 ,
	EEP_REAL_RD2	= EEP_METER_RD1+2	,
	EEP_METER_RD2	= EEP_REAL_RD2+2 ,
	EEP_REAL_RD3	= EEP_METER_RD2+2	,
	EEP_METER_RD3	= EEP_REAL_RD3+2 ,
	EEP_REAL_RD4	= EEP_METER_RD3+2	,
	EEP_METER_RD4	= EEP_REAL_RD4+2 ,
	EEP_REAL_RD5	= EEP_METER_RD4+2	,
	EEP_METER_RD5	= EEP_REAL_RD5+2 ,
	EEP_REAL_RD6	= EEP_METER_RD5+2	,
	EEP_METER_RD6	= EEP_REAL_RD6+2 ,
	EEP_REAL_RD7	= EEP_METER_RD6+2	,
	EEP_METER_RD7	= EEP_REAL_RD7+2 ,
	EEP_REAL_RD8	= EEP_METER_RD7+2	,
	EEP_METER_RD8	= EEP_REAL_RD8+2 ,	
	EEP_REAL_RD9	= EEP_METER_RD8+2	,
	EEP_METER_RD9	= EEP_REAL_RD9+2 ,
	


} EEP_MAP;

typedef enum
{
	MODBUS_SERIALNUMBER_LOWORD = 0,             
	MODBUS_SERIALNUMBER_HIWORD = 2,
	MODBUS_VERSION_NUMBER_LO = 4,
	MODBUS_VERSION_NUMBER_HI = 5,
	MODBUS_ADDRESS = 6,
	MODBUS_PRODUCT_MODEL,
	MODBUS_HARDWARE_REV,
	//MODBUS_PIC_VERSION,
	MODBUS_ADDRESS_PLUG_N_PLAY,
	MODBUS_BAUDRATE = 15,			// base address select
	MODBUS_UPDATE_STATUS = 16,			// status for update flash
	
	MODBUS_PROTOCOL_TYPE = 21,
	MODBUS_INSTANCE_NUM = 39 ,
	
	MODBUS_SERINALNUMBER_WRITE_FLAG = 99,
	
	
	
	
	
	MODBUS_INPUT0 = 100 ,
	MODBUS_INPUT1 		,
	MODBUS_INPUT2 		,
	MODBUS_INPUT3 		,
	MODBUS_INPUT4 		,
	MODBUS_INPUT5 		,
	MODBUS_RANGE0 = 106 ,
	MODBUS_RANGE1 		,
	MODBUS_RANGE2 		,
	MODBUS_RANGE3 		,
	MODBUS_RANGE4 		,
	MODBUS_RANGE5 		,
	MODBUS_OUT0  = 112	,
	MODBUS_OUT1 		,
	MODBUS_OUT2 		,
	MODBUS_OUT3 		,
	MODBUS_OUT4 		,
	MODBUS_OUT5 		,
	MODBUS_AD0  = 118   ,
	MODBUS_AD1          ,
	MODBUS_AD2          ,
	MODBUS_AD3          ,
	MODBUS_AD4          ,
	MODBUS_AD5          ,
	MODBUS_TIMER0 	= 200	,
	MODBUS_TIMER1 		,
	MODBUS_TIMER2 		,
	MODBUS_TIMER3 		,
	MODBUS_TIMER4 		,
	MODBUS_TIMER5 		,
	MODBUS_FIRST_TRIGGLE0 = 206 ,
	MODBUS_FIRST_TRIGGLE1 ,
	MODBUS_FIRST_TRIGGLE2 ,
	MODBUS_FIRST_TRIGGLE3 ,
	MODBUS_FIRST_TRIGGLE4 ,
	MODBUS_FIRST_TRIGGLE5 ,
	MODBUS_SENCOND_TRIGGLE0 = 212,
	MODBUS_SENCOND_TRIGGLE1 ,
	MODBUS_SENCOND_TRIGGLE2 ,
	MODBUS_SENCOND_TRIGGLE3 ,
	MODBUS_SENCOND_TRIGGLE4 ,
	MODBUS_SENCOND_TRIGGLE5 ,
	MODBUS_INPUT_FLAG0  = 218,
	MODBUS_INPUT_FLAG1 ,
	MODBUS_INPUT_FLAG2 ,
	MODBUS_INPUT_FLAG3 ,
	MODBUS_INPUT_FLAG4 ,
	MODBUS_INPUT_FLAG5 ,
	MODBUS_OUTPUT0		= 224 	,
	MODBUS_OUTPUT1,
	MODBUS_OUTPUT2,
	MODBUS_OUTPUT3,
	MODBUS_OUTPUT4,
	MODBUS_OUTPUT5,
	MODBUS_RAW_AD0		= 230 ,
	MODBUS_RAW_AD1			  ,
	MODBUS_RAW_AD2			  ,
	MODBUS_RAW_AD3			  ,
	MODBUS_RAW_AD4			  ,
	MODBUS_RAW_AD5			  ,
						
	MODBUS_OUTPUT_BLOCK_FIRST = 10000,      
	MODBUS_OUTPUT_BLOCK_LAST = MODBUS_OUTPUT_BLOCK_FIRST + MAX_OUTS * ((sizeof(Str_out_point) + 1)/ 2) - 1,
	
	
	MODBUS_INPUT_BLOCK_FIRST = MODBUS_OUTPUT_BLOCK_FIRST + 64 * ((sizeof(Str_out_point) + 1)/ 2),
	MODBUS_INPUT_BLOCK_LAST = MODBUS_INPUT_BLOCK_FIRST + MAX_INS * ((sizeof(Str_in_point) + 1) / 2) - 1,

	MODBUS_VAR_BLOCK_FIRST = MODBUS_INPUT_BLOCK_FIRST + 64 * ((sizeof(Str_in_point) + 1) / 2) ,
	MODBUS_VAR_BLOCK_LAST = MODBUS_VAR_BLOCK_FIRST + MAX_AVS * ((sizeof(Str_variable_point) + 1) / 2) - 1,

} 
MODBUS_REGISTER_LIST;























































 




#endif
