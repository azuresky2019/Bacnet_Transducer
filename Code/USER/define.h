#ifndef _DEFINE_H
#define _DEFINE_H

#define STM32F10X



#ifdef STM32F10X
#define far 
#define xdata
#define idata
#endif

#define READ_WRITE_PROPERTY 1
#define SOFTREV    14

//#define MODBUS 0 
//#define BAC_MSTP 1
//#define TCP_IP 2
//#define BAC_IP 3
//#define BAC_PTP 4
//#define BAC_GSM 5

#define DEFAULT_FILTER  5

#define T38AI8AO6DO  104

//#define SW_VER 20150112


#define	BAUDRATE_9600				9600			//0
#define	BAUDRATE_19200			19200			//1
#define	BAUDRATE_38400			38400			//2
#define	BAUDRATE_57600			57600			//3
#define	BAUDRATE_115200			115200		//4
#define	BAUDRATE_76800			76800			//5
#define EN_OUT 0 
#define EN_IN  1
#define EN_VAR 2





#ifdef T38AI8AO6DO
#define OUTPUT_CONTROL
#define  MAX_AVS  	9
#define MAX_AV		MAX_AVS
#endif

#define PRODUCT_ID 		104
#ifdef T38AI8AO6DO 


#define HW_VER				1
#define MAX_AI_CHANNEL	 	6
#define MAX_AIS         MAX_AI_CHANNEL
#define MAX_INS  				MAX_AIS 

#define MAX_AO					6
#define MAX_AOS					MAX_AO
#define MAX_OUTS				MAX_AO
//#define MAX_AV					20


#define  SW_OFF  0
#define  SW_HAND 2
#define  SW_AUTO 1



#endif 




#endif 

