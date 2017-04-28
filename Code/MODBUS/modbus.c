#include "stdlib.h"
#include "crc.h"
#include "usart.h"

#include "delay.h"
#include "define.h"
#include "modbus.h"
#include "timerx.h"
#include "inputs.h"
#include "../filter/filter.h"
#include "led.h"
#include "fifo.h"
//#include "bacnettimer.h"
#include "../KEY/key.h"
#include "bacnet.h"
#include "rs485.h"
#include "store.h"
#include "ud_str.h"
//#include "ud_str.h"
void Timer_Silence_Reset(void);
static u8 randval = 0 ;
//u8 i2c_test[10] ;
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 uart_send[USART_SEND_LEN] ;
vu8 transmit_finished = 0 ; 
vu8 revce_count = 0 ;
vu8 rece_size = 0 ;
vu8 serial_receive_timeout_count ;
u8 SERIAL_RECEIVE_TIMEOUT ;
u8 dealwithTag ;
STR_MODBUS modbus ;
u8 DealwithTag ;
u16 sendbyte_num = 0 ;
//u16 uart_num = 0 ;
 u8 Station_NUM= 12;
//extern uint8_t Receive_Buffer_Data0[512];
extern FIFO_BUFFER Receive_Buffer0;
 
 
void USART1_IRQHandler(void)                   //串口1中断服务程序
{      
   u8 receive_buf ;
   static u16 send_count = 0 ;
   if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)   //接收中断
   {
         if(modbus.protocal == MODBUS )   
         {
               if(revce_count < 250)
                  USART_RX_BUF[revce_count++] = USART_ReceiveData(USART1);//(USART1->DR);      //读取接收到的数据
                  else
                      serial_restart();
                  if(revce_count == 1)
                  {
                     // This starts a timer that will reset communication.  If you do not
                     // receive the full packet, it insures that the next receive will be fresh.
                     // The timeout is roughly 7.5ms.  (3 ticks of the hearbeat)
                     rece_size = 250;
                     serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;
                  }
                  else if(revce_count == 3 )
                  {
                     if(USART_RX_BUF[1] == CHECKONLINE)
                     rece_size = 6;
                  }
                  else if(revce_count == 4)
                  {
                     //check if it is a scan command
                     if((((vu16)(USART_RX_BUF[2] << 8) + USART_RX_BUF[3]) == 0x0a) && (USART_RX_BUF[1] == WRITE_VARIABLES))
                     {
                        rece_size = DATABUFLEN_SCAN;
                        serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;   
                     }
                  }
                  else if(revce_count == 7)
                  {
                     if((USART_RX_BUF[1] == READ_VARIABLES) || (USART_RX_BUF[1] == WRITE_VARIABLES))
                     {
                        rece_size = 8;
                        //dealwithTag = 1;
                     }
                     else if(USART_RX_BUF[1] == MULTIPLE_WRITE)
                     {
                        rece_size = USART_RX_BUF[6] + 9;
                        serial_receive_timeout_count = USART_RX_BUF[6] + 8;
                     }
                     else
                     {
                        rece_size = 250;
                     }
                  }
                  else if(USART_RX_BUF[0] == 0x55 && USART_RX_BUF[1] == 0xff && USART_RX_BUF[2] == 0x01 && USART_RX_BUF[5] == 0x00 && USART_RX_BUF[6] == 0x00)
                  {//bacnet protocal detected
                        modbus.protocal = BAC_MSTP;
//                        AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, BAC_MSTP);
                     Recievebuf_Initialize(0);                     
                  }
                  else if(revce_count == rece_size)      
                  {
                     // full packet received - turn off serial timeout
                     serial_receive_timeout_count = 0;
                     dealwithTag = 5;      // making this number big to increase delay
                  }
      
         }
         else if(modbus.protocal == BAC_MSTP )
         {
               if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
               {
                     receive_buf =  USART_ReceiveData(USART1); 
                     FIFO_Put(&Receive_Buffer0, receive_buf);
               }
         }
   }
//   else if( USART_GetITStatus(USART1, USART_IT_TC) == SET  )
//     {
//         if( uart_num >=sendbyte_num)
//       {
//             USART_ClearFlag(USART1, USART_FLAG_TC);
//          uart_num = 0 ;
//       }
//         else
//             USART_SendData(USART1, pDataByte[uart_num++]);
//   }
   else  if( USART_GetITStatus(USART1, USART_IT_TXE) == SET  )
     {
        if((modbus.protocal == MODBUS )||(modbus.protocal == BAC_MSTP))
      {
//            if( send_count >= sendbyte_num)
//            {
//               //while(USART_GetITStatus(USART1, USART_IT_TC) == RESET) ;
//               USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
//               send_count = 0 ;
//               Timer_Silence_Reset();
//               serial_restart();
//            }
//            else
//            {
//               USART_SendData(USART1, uart_send[send_count++] );
//               Timer_Silence_Reset();
//            }
         USART_SendData(USART1, uart_send[send_count++]);
         Timer_Silence_Reset();        
         if(send_count >= sendbyte_num)
         {
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
            USART_ClearFlag(USART1, USART_FLAG_TC);
            
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            send_count = 0;
            serial_restart();
         }
      }               
  }
}

void serial_restart(void)
{
   TXEN = RECEIVE;
   revce_count = 0;
   dealwithTag = 0;
} 

//it is ready to send data by serial port . 
static void initSend_COM(void)
{
   TXEN = SEND;
}

void send_byte(u8 ch, u8 crc)
{   
   USART_ClearFlag(USART1, USART_FLAG_TC); 
   USART_SendData(USART1,  ch);
   if(crc)
   {
      crc16_byte(ch);
   }
}
//void USART_SendDataString(u8 *pbuf, u8 num)
// {
//     sendbyte_num = num ;
//    uart_num = 0 ;
//    pDataByte = pbuf ;
//    USART_ClearFlag(USART1, USART_FLAG_TC);   
//     USART_SendData(USART1, pDataByte[uart_num++] ); 
//    tx_count = 20 ;
// }
 void USART_SendDataString( u16 num )
 {
    sendbyte_num = num;
//    uart_num = 0 ;
   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//
 }
void modbus_init(void)
{
   //uart1_init(19200);
   serial_restart();
   SERIAL_RECEIVE_TIMEOUT = 3;
   serial_receive_timeout_count = SERIAL_RECEIVE_TIMEOUT;
}

void internalDeal(u8 type,  u8 *pData)
{
   u16 i ;
   u8 HeadLen ;
   u16 StartAdd ;
   u8 buf1, buf2 ;
   if(type == 0)
   {
      HeadLen = 0 ;   
   }
   else
   {
      HeadLen = 6 ;

   }
   StartAdd = (u16)(pData[HeadLen + 2] <<8 ) + pData[HeadLen + 3];

    if(pData[HeadLen + 1] == WRITE_VARIABLES)
   {                     
         // If writing to Serial number Low word, set the Serial number Low flag
         if(StartAdd <= MODBUS_SERIALNUMBER_LOWORD+1)
         {
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_LOWORD, (u8)pData[HeadLen+5]);
            AT24CXX_WriteOneByte(EEP_SERIALNUMBER_LOWORD+1, pData[HeadLen+4]) ;
            modbus.serial_Num[0] = pData[HeadLen+5] ;
            modbus.serial_Num[1] = pData[HeadLen+4] ;
            modbus.SNWriteflag |= 0x01;
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
            
            if(modbus.SNWriteflag)
            {
               modbus.update = 0;
               AT24CXX_WriteOneByte((u16)EEP_UPDATE_STATUS, 0);
            }
         }
         // If writing to Serial number High word, set the Serial number High flag
         else if(StartAdd <= MODBUS_SERIALNUMBER_HIWORD+1)
         {
            
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_HIWORD, pData[HeadLen+5]);
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_HIWORD+1, pData[HeadLen+4]);
            modbus.serial_Num[2] = pData[HeadLen+5] ;
            modbus.serial_Num[3] = pData[HeadLen+4] ;
            modbus.SNWriteflag |= 0x02;
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
            
            if(modbus.SNWriteflag)
            {
               modbus.update = 0;
               AT24CXX_WriteOneByte((u16)EEP_UPDATE_STATUS, 0);
            }
         }
//         else if(USART_RX_BUF[3] <= MODBUS_VERSION_NUMBER_LO+1)
//         {   
//            AT24CXX_WriteOneByte((u16)EEP_VERSION_NUMBER_LO, USART_RX_BUF[5]);
//            AT24CXX_WriteOneByte((u16)EEP_VERSION_NUMBER_LO+1, USART_RX_BUF[4]);
//            modbus.software = (USART_RX_BUF[5]<<8) ;
//            modbus.software |= USART_RX_BUF[4] ;            
//         }
         else if(StartAdd == MODBUS_ADDRESS )
         {
            AT24CXX_WriteOneByte((u16)EEP_ADDRESS, pData[HeadLen+5]);
            modbus.address   = pData[HeadLen+5] ;
					  Station_NUM = modbus.address /128 ;
         }
//         else if(StartAdd == MODBUS_PRODUCT_MODEL )
//         {
//            AT24CXX_WriteOneByte((u16)EEP_PRODUCT_MODEL, pData[HeadLen+5]);
//            modbus.product   = pData[HeadLen+5] ;
//            modbus.SNWriteflag |= 0x08;
//            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
//         }
         else if(StartAdd == MODBUS_HARDWARE_REV )
         {
            AT24CXX_WriteOneByte((u16)EEP_HARDWARE_REV, pData[HeadLen+5]);
            modbus.hardware_Rev   = pData[HeadLen+5] ;
            modbus.SNWriteflag |= 0x04;
            AT24CXX_WriteOneByte((u16)EEP_SERIALNUMBER_WRITE_FLAG, modbus.SNWriteflag);
         }
         else if(StartAdd == MODBUS_BAUDRATE )         // july 21 Ron
         {         
            modbus.baud = pData[HeadLen+5] ;
			AT24CXX_WriteOneByte(EEP_BAUDRATE, modbus.baud);
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
				break ;				
			}
			uart1_init(modbus.baudrate);
			modbus_init();
         }
         else if(StartAdd == MODBUS_UPDATE_STATUS )         // july 21 Ron
         {
            modbus.update = pData[HeadLen+5] ;
         }
         else if(StartAdd == MODBUS_PROTOCOL_TYPE )         // july 21 Ron
         {
            if((pData[HeadLen+5] == MODBUS)||(pData[HeadLen+5] == BAC_MSTP))
            {
               AT24CXX_WriteOneByte(EEP_MODBUS_COM_CONFIG, pData[HeadLen+5]);
                  modbus.protocal = pData[HeadLen+5] ;
            }
		 }
		 else if(StartAdd  >= MODBUS_OUTPUT_BLOCK_FIRST && StartAdd  <= MODBUS_OUTPUT_BLOCK_LAST)
		 {
			 write_page_en[0] =1 ;
			 if((StartAdd - MODBUS_OUTPUT_BLOCK_FIRST) % ((sizeof(Str_out_point) + 1) / 2) == 0)
			 {
				i = (StartAdd - MODBUS_OUTPUT_BLOCK_FIRST) / ((sizeof(Str_out_point) + 1) / 2);
				memcpy(&outputs[i],&pData[HeadLen + 7],sizeof(Str_out_point));   
			 }
		 }
		 else if(StartAdd  >= MODBUS_INPUT_BLOCK_FIRST && StartAdd  <= MODBUS_INPUT_BLOCK_LAST)
		 {
			 write_page_en[1] =1 ;
			 if((StartAdd - MODBUS_INPUT_BLOCK_FIRST) % ((sizeof(Str_in_point) + 1   ) / 2) == 0)
			 {
				i = (StartAdd - MODBUS_INPUT_BLOCK_FIRST) / ((sizeof(Str_in_point) + 1) / 2);
				memcpy(&inputs[i],&pData[HeadLen + 7],sizeof(Str_in_point));
				
			 }			 
		 }
		 else if(StartAdd  >= MODBUS_VAR_BLOCK_FIRST && StartAdd  <= MODBUS_VAR_BLOCK_LAST)
		 {
			 write_page_en[2] =1 ;
			 if((StartAdd - MODBUS_VAR_BLOCK_FIRST) % ((sizeof(Str_variable_point) + 1   ) / 2) == 0)
			 {
				i = (StartAdd - MODBUS_VAR_BLOCK_FIRST) / ((sizeof(Str_variable_point) + 1) / 2);
				memcpy(&var[i],&pData[HeadLen + 7],sizeof(Str_variable_point)); 
			 }
		 }
   }
   if (modbus.update == 0x7F)
   {
      SoftReset();      
   }
   else if(modbus.update == 0x8e)
   {      
      //address_temp = AT24CXX_ReadOneByte(EEP_UPDATE_STATUS);
      for(i=0; i<255; i++)
      {
            if((i != EEP_UPDATE_STATUS)&&(i != EEP_SERIALNUMBER_LOWORD)&&(i != EEP_SERIALNUMBER_LOWORD+1)&&(i != EEP_ADDRESS)&&(i != EEP_BAUDRATE)
            &&(i != EEP_SERIALNUMBER_HIWORD)&&(i != EEP_SERIALNUMBER_HIWORD+1)&&(i != EEP_HARDWARE_REV)&&(i != EEP_PRODUCT_MODEL)
            )
            AT24CXX_WriteOneByte(i, 0xff);
      }
      EEP_Dat_Init();
//      SoftReset();
      AT24CXX_WriteOneByte(EEP_UPDATE_STATUS, 0);
      modbus.SNWriteflag = 0x00;
      AT24CXX_WriteOneByte(EEP_SERIALNUMBER_WRITE_FLAG, 0);
      SoftReset();
   }
//   // --------------- reset board -------------------------------------------
//   else if (update_flash == 0xFF)
//   {   
////      // disable the global interrupts
////      EA = 0;
////      WDTC = 0x80;    //reset the CPU
////      while(1){};

//   }
}


//static void responseData(u16 start_address)
void responseCmd(u8 type, u8* pData)
{
   u8  i, temp1 =0, temp2 = 0;
	 u8 buf1 , buf2 ;
   u16 send_cout = 0 ;
   u8  sendbuf[300];
   u8 HeadLen = 0 ;
   u16  RegNum;
   u8 cmd  ;
   u16 StartAdd ;
   if(type == 0)
   {
      HeadLen = 0 ;   
   }
   else
   {
      HeadLen = 6 ;
      for(i=0; i<6; i++)
      {
         sendbuf[i] = 0 ;   
      }
      
   }
   cmd = pData[HeadLen + 1]; 
   StartAdd = (u16)(pData[HeadLen + 2] <<8 ) + pData[HeadLen + 3];
   RegNum = (u8)pData[HeadLen + 5];
   
   
 if(cmd == WRITE_VARIABLES)
   {      
      send_cout = HeadLen ;

      if(type == 0)
      {
         for(i = 0; i < rece_size; i++)
         {
            sendbuf[send_cout++] = pData[i] ;
            

         }
         memcpy(uart_send, sendbuf, send_cout);
         USART_SendDataString(send_cout);      
      }    
   }
   else if(cmd == MULTIPLE_WRITE)
   {
      if(type == 0)
      {      
         for(i = 0; i < 6; i++)
         {
         sendbuf[HeadLen+i] = pData[HeadLen+i] ;
         crc16_byte(sendbuf[HeadLen+i]);
         }
         sendbuf[HeadLen+i] = CRChi ;
         sendbuf[HeadLen+i+1] = CRClo ;
         memcpy(uart_send, sendbuf, 8);
         USART_SendDataString(8);
      }           
   }
   else if(cmd == READ_VARIABLES)
   {
      u16 address;
      u16 address_temp ;
      u16 address_buf ;
	  u16 temp ;
      sendbuf[HeadLen] = pData[HeadLen] ;
      sendbuf[HeadLen + 1] = pData[HeadLen + 1];
      sendbuf[HeadLen + 2] = RegNum * 2;
      crc16_byte(sendbuf[HeadLen]);
      crc16_byte(sendbuf[HeadLen + 1]);
      crc16_byte(sendbuf[HeadLen + 2]);
      send_cout = HeadLen + 3 ;
      for(i = 0; i < RegNum; i++)
      {
         address = StartAdd + i;
         if(address <= MODBUS_SERIALNUMBER_HIWORD + 1)
         {
            temp1 = 0 ;
            temp2 = modbus.serial_Num[address] ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
         else if(address == MODBUS_VERSION_NUMBER_LO)
         {
            temp1 = (SOFTREV>>8)&0xff ;
            temp2 =  (u8)(SOFTREV) ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);      
         }
         else if(address == MODBUS_VERSION_NUMBER_HI)
         {
            temp1 = 0 ;
            temp2 = 0 ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);   
         }
         else if(address == MODBUS_ADDRESS)
         {
            temp1 = 0 ;
            temp2 =  modbus.address;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);      
         }
         else if(address == MODBUS_PRODUCT_MODEL)
         {
            temp1 = 0 ;
            temp2 =  PRODUCT_ID;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
         else if(address == MODBUS_HARDWARE_REV)
         {
            temp1 = 0 ;
            temp2 =  modbus.hardware_Rev;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);            
         }
         else if(address == MODBUS_BAUDRATE)
         {
            temp1 = 0 ;
            temp2 =  modbus.baud;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);            
         }
         
         else if(address == MODBUS_UPDATE_STATUS)
         {
            temp1 = 0 ;
            temp2 =   modbus.update;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
         else if(address == MODBUS_SERINALNUMBER_WRITE_FLAG)
         {
            temp1 = 0 ;
            temp2 =  modbus.SNWriteflag ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
         else if(address == MODBUS_PROTOCOL_TYPE)
         {
            temp1 = 0 ;
            temp2 =  modbus.protocal ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if(address == MODBUS_INSTANCE_NUM)
         {
            temp1 = 0 ;
            temp2 =  Station_NUM ;
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_INPUT0)&&(address<= MODBUS_INPUT5))
         {
            address_temp = address - MODBUS_INPUT0 ;
			if(inputs[address_temp].digital_analog == 1) 
			{
				temp1 = ((inputs[address_temp].value/10)>>8)&0xff ;
				temp2 =  (inputs[address_temp].value/10)&0xff; 			
			}	
			else
			{
				if(inputs[address_temp].range <= LOW_HIGH )
				{
					if(inputs[address_temp].value >= 1000 )
					{
						temp1 = 0 ;
						temp2 = 1;
					}
					else
					{
						temp1 = 0 ;
						temp2 = 0;
					}
				}
				else
				{
					if(inputs[address_temp].value >= 1000 )
					{
						temp1 = 0 ;
						temp2 = 0;
					}
					else
					{
						temp1 = 0 ;
						temp2 = 1;
					}
				}
			}
			sendbuf[send_cout++] = temp1 ;
			sendbuf[send_cout++] = temp2 ;
			crc16_byte(temp1);
			crc16_byte(temp2);														
         }
		 else if((address >= MODBUS_RANGE0)&&(address<= MODBUS_RANGE5))
         {
            address_temp = address - MODBUS_RANGE0 ; 
            temp1 = (inputs[address_temp].range>>8)&0xff ;
            temp2 =  inputs[address_temp].range&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_AD0)&&(address<= MODBUS_AD5))
         {
            address_temp = address - MODBUS_AD0 ; 
            temp1 = (After_filter[address_temp]>>8)&0xff ;
            temp2 = After_filter[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_OUT0)&&(address<= MODBUS_OUT5))
         {
            address_temp = address - MODBUS_OUT0 ; 
            temp1 = (outputs[address_temp].value>>8)&0xff ;
            temp2 =  outputs[address_temp].value&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_TIMER0)&&(address<= MODBUS_TIMER5))
         {
            address_temp = address - MODBUS_TIMER0 ; 
            temp1 = (input_timer[address_temp]>>8)&0xff ;
            temp2 =  input_timer[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_FIRST_TRIGGLE0)&&(address<= MODBUS_FIRST_TRIGGLE5))
         {
            address_temp = address - MODBUS_FIRST_TRIGGLE0 ; 
            temp1 = (first_trrigle[address_temp]>>8)&0xff ;
            temp2 =  first_trrigle[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_SENCOND_TRIGGLE0)&&(address<= MODBUS_SENCOND_TRIGGLE5))
         {
            address_temp = address - MODBUS_SENCOND_TRIGGLE0 ; 
            temp1 = (second_trrigle[address_temp]>>8)&0xff ;
            temp2 =  second_trrigle[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_INPUT_FLAG0)&&(address<= MODBUS_INPUT_FLAG5))
         {
            address_temp = address - MODBUS_INPUT_FLAG0 ; 
            temp1 = (input_flag[address_temp]>>8)&0xff ;
            temp2 =  input_flag[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
		 else if((address >= MODBUS_OUTPUT0)&&(address<= MODBUS_OUTPUT5))
         {
            address_temp = address - MODBUS_OUTPUT0 ; 
            temp1 = (output_value[address_temp]>>8)&0xff ;
            temp2 =  output_value[address_temp]&0xff; 
            sendbuf[send_cout++] = temp1 ;
            sendbuf[send_cout++] = temp2 ;
            crc16_byte(temp1);
            crc16_byte(temp2);
         }
//		 else if((address >= MODBUS_RAW_AD0)&&(address<= MODBUS_RAW_AD5))
//         {
//            address_temp = address - MODBUS_RAW_AD0 ; 
//            temp1 = (After_filter[address_temp]>>8)&0xff ;
//            temp2 =  After_filter[address_temp]&0xff; 
//            sendbuf[send_cout++] = temp1 ;
//            sendbuf[send_cout++] = temp2 ;
//            crc16_byte(temp1);
//            crc16_byte(temp2);
//         } 
		else if((address >= MODBUS_OUTPUT_BLOCK_FIRST)&&(address<= MODBUS_OUTPUT_BLOCK_LAST))
		{
		   U8_T  index,item;   
		   U16_T  *block ;
		   index = (address - MODBUS_OUTPUT_BLOCK_FIRST) / ((sizeof(Str_out_point) + 1) / 2);
		   block = (U16_T  *)&outputs[index];
		   item = (address - MODBUS_OUTPUT_BLOCK_FIRST) % ((sizeof(Str_out_point) + 1) / 2);      
		   temp1= (block[item]>>8)&0xff ;
		   temp2= block[item]&0xff ;
		   sendbuf[send_cout++] = temp1 ;
		   sendbuf[send_cout++] = temp2 ;
		   crc16_byte(temp1);
		   crc16_byte(temp2);
		}
		else if((address >= MODBUS_INPUT_BLOCK_FIRST)&&(address<= MODBUS_INPUT_BLOCK_LAST))
		{
		   U8_T  index,item;   
		   U16_T  *block ;
		   index = (address - MODBUS_INPUT_BLOCK_FIRST) / ((sizeof(Str_in_point) + 1) / 2);
		   block = (U16_T  *)&inputs[index];
		   item = (address - MODBUS_INPUT_BLOCK_FIRST) % ((sizeof(Str_in_point) + 1) / 2);      
		   temp1= (block[item]>>8)&0xff ;
		   temp2= block[item]&0xff ;
		   sendbuf[send_cout++] = temp1 ;
		   sendbuf[send_cout++] = temp2 ;
		   crc16_byte(temp1);
		   crc16_byte(temp2);
		}
		else if((address >= MODBUS_VAR_BLOCK_FIRST)&&(address<= MODBUS_VAR_BLOCK_LAST))
		{
		   U8_T  index,item;   
		   U16_T  *block ;
		   index = (address - MODBUS_VAR_BLOCK_FIRST) / ((sizeof(Str_variable_point) + 1) / 2);
		   block = (U16_T  *)&var[index];
		   item = (address - MODBUS_VAR_BLOCK_FIRST) % ((sizeof(Str_variable_point) + 1) / 2);      
		   temp1= (block[item]>>8)&0xff ;
		   temp2= block[item]&0xff ;
		   sendbuf[send_cout++] = temp1 ;
		   sendbuf[send_cout++] = temp2 ;
		   crc16_byte(temp1);
		   crc16_byte(temp2);
		}	
		else
		{
			temp1 = 0 ;
			temp2 =  0; 
			sendbuf[send_cout++] = temp1 ;
			sendbuf[send_cout++] = temp2 ;
			crc16_byte(temp1);
			crc16_byte(temp2);
		}

      }//end of number
      temp1 = CRChi ;
      temp2 =  CRClo; 
      sendbuf[send_cout++] = temp1 ;
      sendbuf[send_cout++] = temp2 ;
      if(type == 0)
      {
         memcpy(uart_send, sendbuf, send_cout);
         USART_SendDataString(send_cout);
      }
   }

   
   else if (USART_RX_BUF[1] == CHECKONLINE)
   {
      // send first byte of information
         
         temp2 =  pData[HeadLen+0]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
      

         temp2 = pData[HeadLen+1]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
      

         temp2 =  modbus.address; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
         

         temp2 =  modbus.serial_Num[0]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
         
         temp2 =  modbus.serial_Num[1]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
         

         temp2 =  modbus.serial_Num[2]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
         

         temp2 =  modbus.serial_Num[3]; 
         uart_send[send_cout++] = temp2 ;
         crc16_byte(temp2);
         
         temp2 =  CRChi; 
         uart_send[send_cout++] = temp2 ;
         temp2 =  CRClo; 
         uart_send[send_cout++] = temp2 ;
         USART_SendDataString(send_cout);
   }
}
u8 checkData(u16 address)
{
   //static unsigned char xdata rand_read_ten_count = 0 ;
   u16 crc_val;
   u8 minaddr,maxaddr, variable_delay;
   u8 i;
   static u8 srand_count =0 ;
   srand_count ++ ;
   // check if packet completely received
   if(revce_count != rece_size)
      return 0;

   // check if talking to correct device ID
   if(USART_RX_BUF[0] != 255 && USART_RX_BUF[0] != modbus.address && USART_RX_BUF[0] != 0)
      return 0;   

   //  --- code to verify what is on the network ---------------------------------------------------
   if( USART_RX_BUF[1] == CHECKONLINE)
   {

   crc_val = crc16(USART_RX_BUF,4) ;
   if(crc_val != (USART_RX_BUF[4]<<8) + USART_RX_BUF[5] )
   {
      return 0;
   }
   minaddr = (USART_RX_BUF[2] >= USART_RX_BUF[3] ) ? USART_RX_BUF[3] : USART_RX_BUF[2] ;   
   maxaddr = (USART_RX_BUF[2] >= USART_RX_BUF[3] ) ? USART_RX_BUF[2] : USART_RX_BUF[3] ;
   if(modbus.address < minaddr || modbus.address > maxaddr)
      return 0;
   else
   {   // in the TRUE case, we add a random delay such that the Interface can pick up the packets
      srand(srand_count);
      variable_delay = rand() % 20;
      for ( i=0; i<variable_delay; i++)
         delay_us(100);
      return 1;
   }

   }
   // ------------------------------------------------------------------------------------------------------



   // check that message is one of the following
   if( (USART_RX_BUF[1]!=READ_VARIABLES) && (USART_RX_BUF[1]!=WRITE_VARIABLES) && (USART_RX_BUF[1]!=MULTIPLE_WRITE) )
      return 0;
   // ------------------------------------------------------------------------------------------------------
      // ------------------------------------------------------------------------------------------------------
      
   if(USART_RX_BUF[2]*256 + USART_RX_BUF[3] ==  MODBUS_ADDRESS_PLUG_N_PLAY)
   {
      if(USART_RX_BUF[1] == WRITE_VARIABLES)
      {
         if(USART_RX_BUF[6] != modbus.serial_Num[0]) 
         return FALSE;
         if(USART_RX_BUF[7] != modbus.serial_Num[1]) 
         return FALSE;
         if(USART_RX_BUF[8] != modbus.serial_Num[2])  
         return FALSE;
         if(USART_RX_BUF[9] != modbus.serial_Num[3]) 
         return FALSE;
      }
      if (USART_RX_BUF[1] == READ_VARIABLES)
      {
         randval = rand() % 5 ;
      }
      if(randval != RESPONSERANDVALUE)
      {

            return FALSE;
      }
      else
      {   // in the TRUE case, we add a random delay such that the Interface can pick up the packets
         variable_delay = rand() % 20;
         for ( i=0; i<variable_delay; i++)
            delay_us(100);
      }
      
   }

   // if trying to write the Serial number, first check to see if it has been already written
   // note this does not take count of multiple-write, thus if try to write into those reg with multiple-write, command will accept
   if( (USART_RX_BUF[1]==WRITE_VARIABLES)  && (address<= MODBUS_HARDWARE_REV) )
   {
      // Return false if trying to write SN Low word that has already been written
      if(USART_RX_BUF[3] < 2)
      {
         if(modbus.SNWriteflag & 0x01)                // low byte of SN writed
            return FALSE;
      }
      // Return false if trying to write SN High word that has already been written
      else if (USART_RX_BUF[3] < 4)
      {
         if(modbus.SNWriteflag  & 0x02)                 // high byte of SN writed
            return FALSE;
      }
      else if (USART_RX_BUF[3] ==  MODBUS_HARDWARE_REV)
      {
         if(modbus.SNWriteflag  & 0x04)                 // hardware byte writed
            return FALSE;
      }

   }


   crc_val = crc16(USART_RX_BUF, rece_size-2);

   if(crc_val == (USART_RX_BUF[rece_size-2]<<8) + USART_RX_BUF[rece_size-1] )
   {
      return 1;
   }
   else
   {
      return 0;
   }
   //return TRUE;

 }

 
 
 void dealwithData(void)
{   
   u16 address;
   // given this is used in multiple places, decided to put it as an argument
   address = (u16)(USART_RX_BUF[2]<<8) + USART_RX_BUF[3];
   if (checkData(address))
   {      
//      // Initialize tranmission
      initSend_COM();   
      // Initialize CRC
      init_crc16();      

//      // Respond with any data requested
      responseCmd(0,USART_RX_BUF);
////      // Store any data being written
      internalDeal(0, USART_RX_BUF);

   }
   else
   {
      serial_restart();
   }
}


void SoftReset(void)
{
   __set_FAULTMASK(1);      // 关闭所有中断
   NVIC_SystemReset();      // 复位
}


void urat_print(char* str)
{
	
	memcpy( uart_send,str,strlen(str) );
	USART_SendDataString(strlen(str));
	
	

}