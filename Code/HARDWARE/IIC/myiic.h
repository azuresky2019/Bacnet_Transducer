#ifndef __MYIIC_H
#define __MYIIC_H

#include "bitmap.h"
   	   		   
//IO��������
#define SDA_IN()	{GPIOA->CRL &= 0XFFFFF0FF; GPIOA->CRL |= ((u32)8 << 8);}
#define SDA_OUT()	{GPIOA->CRL &= 0XFFFFF0FF; GPIOA->CRL |= ((u32)3 << 8);}

//IO��������	 
#define IIC_SCL		PAout(3)	//SCL
#define IIC_SDA		PAout(2)	//SDA	 
#define READ_SDA	PAin(2)		//����SDA 
#define IIC_WP		PAout(4)

//IIC���в�������
void IIC_Init(void);				//��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);				//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void);				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);
  
#endif