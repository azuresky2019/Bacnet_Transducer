#include "stdio.h"
#include "gp22.h"
#include "lcd.h"
#include "modbus.h"
#include "include.h"
#include "delay.h"
#include "store.h"

#define TEMP_NUM 5 

void Cal_T(void) ;
const uint32_t TEMP_R[11]={100000,103902,107793,111672,115540,119397,123241,127075,130896 ,134706,138505};
static float time_buf1 = 0 ;
static float time_buf2 = 0 ;

  

 float CorrectFactor = 0 ;
 uint32_t Flow_Value1, Flow_Value2, Flow_Value3 ;
 uint32_t flow_upstream , flow_downstream ;
 uint32_t gp22_data[4] ;
 uint16_t gp22_status = 0 ;
 float Pw1st_Buf = 0 ;
 

/************pin num*****************************/

//RST  		PF7
//INT	 	PF6

//SPI_CS		PB12
//SPI_CLK	 	PB13
//SPI_MISO	PB14
//SPI_MOSI	PB15 
/************************************************/

//void delay_us(void)
//{
//	uint16_t j ;
//	for(j=0; j<10; j++) ;
//}

void gp22_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOC, GPIO_Pin_5);
	gp22_device_init();
}


void OpGP2Code(uint8_t GP2_DATA){  
  uint8_t GP2_BIT;        
  for(GP2_BIT=0;GP2_BIT<8;GP2_BIT++){
     GP22_SET_CLK();                          
     if(GP2_DATA&0x80){
       GP22_SET_MOSI();
     }
     else{
       GP22_CLR_MOSI();
     }
	 delay_us(1);
     GP22_CLR_CLK();     
     GP2_DATA<<=1;
	 delay_us(1);
  }
}
void ConfigGP2(uint32_t data){//配置TDC-GP2寄存器
  unsigned char i;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();
  for(i=0;i<32;i++){
    GP22_SET_CLK();
	  delay_us(1);
     if(data&0x80000000){
       GP22_SET_MOSI();
     }
     else{
       GP22_CLR_MOSI();
     }
     GP22_CLR_CLK(); 
	 delay_us(1);
     data<<=1;
  }
 GP22_SET_CS();
}

void gp22_device_init(void){
  GP22_SET_RST(); //GP21复位高
  delay_us(10);
  GP22_CLR_RST();   //GP21复位低
  delay_us(10);
 GP22_SET_RST();  //GP21复位高
  
  GP22_SET_CS();//GP21片选高
  delay_us(10);
  GP22_CLR_CS();//GP21片选低
  OpGP2Code(0x50);  
  delay_us(500);
  testcomunication();
  
	ConfigGP2(0x80A30B68);
	delay_us(10);

	ConfigGP2(0x81214440);
	delay_us(10);
//	ConfigGP2(0x82A01F40);
	if(modbus.pipe == 0)
	{
		ConfigGP2(0x82A01F40);	//62.5US	
	}
	else if((modbus.pipe == 1)||(modbus.pipe == 2))
	{	
		ConfigGP2(0x82A02648);	//76.56
	}
	
	else
	{
		ConfigGP2(0x82202710);
	}
   delay_us(10);
   ConfigGP2(0x83D05103);
   delay_us(10);
   ConfigGP2(0x84100040);
   delay_us(10);
   ConfigGP2(0x85400000);
   delay_us(10);
   ConfigGP2(0x86C0C060);
   delay_us(10);
  GP22_SET_CS(); //GP21片选高
   CalStart();
}//=========================================


uint8_t pi_read(void)
{
  uint8_t buf_in = 0 ;
  uint8_t i ;
  GP22_SET_CLK();
  for(i=0; i<8;i++)
  {
      GP22_CLR_CLK();
	  delay_us(1);
	  if(GP22_READ_MISO()) buf_in |= (0x80>>i);
	  GP22_SET_CLK();
	  delay_us(1);
  }
  GP22_CLR_CLK();
  return buf_in ;
}

void pi_write(uint8_t buf_out)
{
  uint8_t i = 8;
  uint8_t temp ;
   do
   {          
	  GP22_SET_CLK();
	  temp = ( buf_out & 0x80 );
	  if(temp >0 ) GP22_SET_MOSI();
      else        GP22_CLR_MOSI();
	  delay_us(1);
	   GP22_CLR_CLK();
	   buf_out <<=1;
	   delay_us(1);
   } while( --i != 0 );
   GP22_SET_MOSI();
 
}


void spi_write8(uint8_t data_out)
{

  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();
  pi_write(data_out);
  GP22_SET_CS();
 
}

uint8_t spi_read8(uint8_t address)
{
   uint8_t read_result = 0 ;
   GP22_SET_CS();
   delay_us(1);
   GP22_CLR_CS();
   //delay_us(5);
   pi_write(address);  
   read_result = pi_read();
   GP22_SET_CS();
  

   return read_result ;  
}
void spi_write32(uint8_t address, uint32_t data_out)
{
   uint8_t temp1,temp2,temp3,temp4  ;
   GP22_SET_CS();
   delay_us(1);
   GP22_CLR_CS();
   temp1 = (data_out >>24)&0xff ;
   temp2 = (data_out >>16)&0xff ;
   temp3 = (data_out >>8)&0xff ;
   temp4 = (data_out)&0xff ;
   pi_write(address);
   pi_write(temp1);
   pi_write(temp2);
   pi_write(temp3);
   pi_write(temp4); 
   GP22_SET_CS();;  
}

uint32_t spi_read32(uint8_t address)
{
  uint32_t read_result = 0 ;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();
  pi_write(address);
  read_result=  pi_read();
  read_result = (read_result<<8)| pi_read();
  read_result = (read_result<<8)| pi_read();
  read_result = (read_result<<8)| pi_read();
  GP22_SET_CS();  
   return read_result ;
}

uint16_t Read_GP2STAT(){
  uint16_t GP2_STAT=0;
  uint8_t i;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();    
  OpGP2Code(0xb4);
  for(i=0;i<16;i++){	 
    GP22_SET_CLK(); 
    GP2_STAT<<=1;
	 delay_us(1); 
    if(GP22_READ_MISO()){
     GP2_STAT|=0x0001;
    }    
    GP22_CLR_CLK();
  }
  GP22_SET_CS();
  return GP2_STAT;
}







uint16_t Read_Test(){
  uint16_t GP2_STAT=0;
  uint8_t i;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();;    
  OpGP2Code(0xb5);
  for(i=0;i<8;i++){
    GP22_SET_CLK(); 
    GP2_STAT<<=1;
    if(GP22_READ_MISO()){
     GP2_STAT|=0x0001;
    }    
    GP22_CLR_CLK();
  }
  GP22_SET_CS();
  return GP2_STAT;
}

void testcomunication(void)
{

 uint8_t data_buf1, data_buf2 ;
 uint32_t reg_test = 0x12345678 ;
  pi_write(0X50);//上电复位
  delay_ms(1);
  spi_write32(0x81,reg_test);
  delay_ms(1);
  data_buf1 = Read_Test();
  data_buf2 = (reg_test>>24)&0xff ;
  
  if(data_buf1 == data_buf2)
  { 
    printf("com OK\n\r"); 
	  delay_ms(200);	
  }
  else
  { 
    printf("com error\n\r"); 
		delay_ms(200);	
  }    
}


void ReadCalStart(){
  uint8_t i;
  uint32_t data=0;
//  uint8_t str[20] ;
  float temp1 ;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();  
  OpGP2Code(0xb0);
  for(i = 0;i <32;i++){
    GP22_SET_CLK();
    data<<=1;
    if(GP22_READ_MISO()){
     data|=0x00000001;
    }   
   GP22_CLR_CLK();
  }
  GP22_SET_CS();
  
  //sprintf((char*)str,"0x%x", data);
  //lcd_print(0, 0, str); 
 // temp1 = (float)((REG_0>>16)&0xffff)+(int16)(REG_0 )*1.0/65536;
  temp1 = ((data >>16)&0xffff) +(float)(data & 0xffff)/65536 ;
  CorrectFactor=(1.0*61035 /250) /temp1;
  
}//========================================

void CalStart(void){
    
  uint16_t i = 0 ; 
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();
  OpGP2Code(0x03);

	while(GP22_READ_INIT())
	{
		i++ ;
		if(i >= 50000) break ;

	};	

  ReadCalStart();
  //printf("\n\r CorrectFactor = %.3f", CorrectFactor);
}






void measurement(void)
{
      
      static uint8_t signal_week_count = 0 ;
	  static float flow_total_old = 0 ;
	  static  uint8_t ms_count = 0 ;
	  static  uint16_t sum_speed = 0 ;
	  float heat_temp ;
	  uint16_t speed_temp = 0 ;
	  float flow_speed ;
	  uint8_t m_str[50] ;
	  uint32_t Status ;
      uint8_t Pw1st_REG ;      
	  uint16_t delay_count ; 
	  ConfigGP2(0x81214440);
	  spi_write8 (0x70) ;//初始化测量，通知gp2进入测量准备状态
	  spi_write8 (0x05);
	  delay_count = 0 ;
	while(GP22_READ_INIT())
	  {
		delay_count ++ ;
		 if(delay_count ==60000)
		 {
			 urat_print("TIMEOUT\n\r");
			 break ;
		 }
	  }
	 Read_Flow();
	  flow_upstream = Flow_Value3 ;
      ConfigGP2(0x81214440);   
      Status = Read_GP2STAT(); 
      if((Status&0x1e00)>0 )
      {
         if((Status&0x0600)>0)
         {
            gp22_status = 0x04 ;
           
         }
         else
         {
            gp22_status = 0x08 ;   
         }
		message_index = 12 ;
		modbus.signal = 0 ;
        return ;
      }
      else
      {
         gp22_status &= ~0x0C ;  
      }
      
      spi_write8 (0x70) ;//初始化测量，通知gp2进入测量准备状态
      delay_count = 0 ;
      while(GP22_READ_INIT())
	  {
		 delay_count ++ ;
		 if(delay_count ==60000)
		 {
			 break ;
		 }
	  }
	  Read_Flow();
      flow_downstream = Flow_Value3 ;

	  
      ConfigGP2(0x81214440);
      spi_write8 (0x70) ;
      delay_us(1);
      Pw1st_REG = spi_read8(0xb8);             
       if((Pw1st_REG&0x80)==0)
       {
         Pw1st_Buf = ((float)(Pw1st_REG&0x7f)/128) ;  
       }
       else
       {
        Pw1st_Buf = (1.0 +(float)(Pw1st_REG&0x7f)/128) ; 
       }    
      ConfigGP2(0x81214440); 
	  modbus.signal =  (uint16_t)(Pw1st_Buf *100); 	
	  if((modbus.signal>=20)/*&&(modbus.signal<=100)*/)
	  {				
		 speed_temp = Cal_Flow();				
	  }
	  else
	  {
		speed_temp = 0 ;
	  }
	  sum_speed  += speed_temp ;
	  ms_count ++ ;
	  if(ms_count == 5)
	  {
		flow_speed = (float)sum_speed / 5 ;
		ms_count = 0 ; 
        sum_speed = 0 ;		  
		//printf("S=%u F=%.1f T=%.1f T=%.1f\n\r", modbus.signal, flow_speed,time_buf1, time_buf2) ;
		if(flow_speed >= 1)
		modbus.flow_rate = 0.113097 * modbus.radius* modbus.radius * flow_speed ;//3.1516926*r*r*v*3600 = 11309.7*r*r*V
		else
		modbus.flow_rate = 0 ;	
		if(modbus.flow_rate > 0)
		 {
			 store_flag = 1 ;
			 heat_temp = (float)modbus.flow_rate /18000;
			 modbus.flow_total.temp +=   heat_temp; 
			 modbus.flow_total_int =  (uint32_t)(modbus.flow_total.temp *1000) ;
			 modbus.heat_total.temp +=  heat_temp *modbus.temp_diff*0.42; 		  
		 }
	}

}

void Read_Flow(void)
{
  uint8_t i ;
  Flow_Value2 = 0 ;
  Flow_Value1 = 0 ;
  Flow_Value3 = 0 ;
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();  
  OpGP2Code(0xb3);      //读取数据寄存器1//
  for(i = 0;i <32;i++){   
    GP22_SET_CLK();
	delay_us(1);
    Flow_Value3<<=1;
    if(GP22_READ_MISO()){
     Flow_Value3|=0x00000001;
    }	
    GP22_CLR_CLK();
	delay_us(1);
  }
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();  
  OpGP2Code(0xb2);     //读取数据寄存器2//
  for(i = 0;i <32;i++){   
    GP22_SET_CLK();
	delay_us(1);  
    Flow_Value2<<=1;
    if(GP22_READ_MISO()){
     Flow_Value2|=0x00000001;
    }   
    GP22_CLR_CLK();
	delay_us(1);
  }
  GP22_SET_CS();
  delay_us(1);
  GP22_CLR_CS();  
  OpGP2Code(0xb1);     //读读取数据寄存器3//
  for(i = 0;i <32;i++){   
    GP22_SET_CLK();
    Flow_Value1<<=1;
	delay_us(1);
    if(GP22_READ_MISO()){
     Flow_Value1|=0x00000001;
    }   
    GP22_CLR_CLK();
	delay_us(1);
  }
  GP22_SET_CS();
}


uint32_t Cal_Flow(void)
{
//   float buf1=0, buf2=0 ;
   float temp1=0, temp2=0 ;
   float time_diff=0 ;
   float V_buf = 0; 
   static uint8_t Sum_Count = 0 ;
   float flow_buf = 0 ;
    uint8_t m_str[80] ;  	
      temp1 = ((float)((flow_upstream>>16)&0xffff)+(float)(flow_upstream&0xffff)/65536)/3;
      temp2 = ((float)((flow_downstream>>16)&0xffff)+(float)(flow_downstream&0xffff)/65536)/3;
      time_buf1 = temp1 *250*CorrectFactor ;
      time_buf2 = temp2 *250*CorrectFactor ;			
      modbus.time[0] =  (u16)((u32)time_buf1/10) ;
			modbus.time[1] =  (u16)((u32)time_buf2/10) ;
//			printf("V= %u %u\n\r",(u32)time_buf1, (u32)time_buf2);	
			if(time_buf1> time_buf2)
      {
         time_diff = (time_buf1-time_buf2);
				 modbus.flow_time_diff = (u16)(modbus.time[0] - modbus.time[1]) ;
				 //V_buf = 5*(float)modbus.l*time_diff*10000000/(time_buf1*time_buf2) ;//cm/s
				 V_buf = (uint32_t)(time_diff *0.1125* modbus.k_factor /modbus.l);//(T*1500*1500/(2*L))  cm/s
	 	  
      }
			else
			{
			 time_diff = 0;
			 modbus.flow_time_diff = 0 ;
				V_buf = 0 ;
			}

		
//		V_buf = (uint32_t)(time_diff *112.5 /modbus.l); //(unit: cm/s) 
//		V_buf = modbus.l*time_diff/(2*time_buf1*time_buf2)
//		modbus.flow_rate = 0.1130907 * modbus.radius* modbus.radius * V_buf ;  
	  return V_buf  ;

}



void Measure_Temp(void)
{
   
   uint16_t Status ;
//	uint8_t t_str[16] ;
	uint16_t i = 0 ;
   spi_write8 (0x70) ;
   spi_write8 (0x02) ;
	while(GP22_READ_INIT())
	{
		i++ ;
		if(i >= 50000) break ;

	};
   Status = Read_GP2STAT();
   if((Status&0x1e00) > 0)
   {
      if((Status&0x0800) >0)
      {
         gp22_status = 0x02 ;			//TEMP SENSOR IS OPEN
//         return ;
      }
      if((Status&0x1000) >0)
      {
         gp22_status = 0x01 ; 		 //TEMP SENSOR IS SHORT	
//         return ;
      }
   }
   else 
   {
      gp22_status &= ~0x03 ;
   }
   ReadGP2Data();
    

   Cal_T();
  // printf("T1 =0x%x,T2 =0x%x,T3 = 0x%x,T4 = 0x%x, Status=0x%x", gp22_data[0],gp22_data[1],gp22_data[2],gp22_data[3],Status);
 //  sprintf((char*)data_str, "T1 =0x%x,T2 =0x%x,T3 = 0x%x,T4 = 0x%x, Status=0x%x", gp22_data[0],gp22_data[1],gp22_data[2],gp22_data[3],Status);

}


void ReadGP2Data(void)
{
	uint8_t i, loop;
	for(loop=0; loop<4; loop++)
	{
	  GP22_SET_CS();
	  delay_us(1);
	  GP22_CLR_CS();  
	  OpGP2Code(0xb0+loop);      //读取数据寄存器1//
	  for(i = 0;i <32;i++){   
		GP22_SET_CLK();
		gp22_data[loop]<<=1;
		 delay_us(1);
		if(GP22_READ_MISO()){
		 gp22_data[loop]|=0x00000001;
		}
		delay_us(1);
		GP22_CLR_CLK();
	  }
	 } 
	 GP22_SET_CS();	 
}//========================================

void Cal_T(void)
{
//   uint8_t t_str[16] ;
	uint32_t R1 , R2 ;
   uint16_t T1 , T2 ;
   uint32_t Average_Ref ;
   static uint32_t T1_Buf = 0 , T2_Buf = 0 ;
   static uint8_t Sum_Count = 0 ;
     Average_Ref = (gp22_data[0] + gp22_data[1])/2;
	  if((gp22_data[2]>0)&&(gp22_data[2]<0xffffffff))
	  {  
		R1 = 1.0*gp22_data[2]/Average_Ref *100000;
	  }
	  else if(gp22_data[2] == 0)
	  {
		 message_index = 	9;
	  }
	  else if(gp22_data[2] == 0xffffffff)
	  {
		message_index=  11;
	  }
	  
	  if((gp22_data[3]>0)&&(gp22_data[3]<0xffffffff))
	  {  
		R2 = 1.0*gp22_data[3]/Average_Ref *100000;
	  }
	  else if(gp22_data[2] == 0)
	  {
		 message_index = 8;
	  }
	  else if(gp22_data[2] == 0xffffffff)
	  {
		message_index=  10;
	  }
	  	
      T1_Buf += (uint16_t)Get_Temp(R1);
      T2_Buf += (uint16_t)Get_Temp(R2);
			gp22_data[0] = 0 ;
			gp22_data[1] = 0 ;
			gp22_data[2] = 0 ;
			gp22_data[3] = 0 ;	
      Sum_Count ++ ;
      if(Sum_Count == TEMP_NUM)
      {
         
         T1 =  (float)T1_Buf/TEMP_NUM/0.9931;
         T2 =  (float)T2_Buf/TEMP_NUM/0.9931; 
		 
		 if(modbus.temp_unit == 1 ) 
		 {
				modbus.temp_up = T1 ;
				modbus.temp_down = T2 ;
		 }
		 else
		 {			
				
				modbus.temp_up  = (T1  * 9)/5 +320;
				modbus.temp_down  = (T2  * 9)/5 +320;
		 }
		 if(modbus.temp_up > modbus.temp_down)  
					modbus.temp_diff = modbus.temp_up -  modbus.temp_down ;
		 else modbus.temp_diff = 0 ; 
//        printf("\n\r%luC %luC", T1,T2);      
         Sum_Count = 0 ;
         T1_Buf= 0 ;
         T2_Buf = 0 ;   
      }             
 
}


 uint32_t Get_Temp(uint32_t R_Buf)
{
   uint8_t Loop = 0 ;
   uint32_t T ;
   if(R_Buf <= TEMP_R[0])   return 0 ;
   else if(R_Buf >= TEMP_R[10]) return 2500 ;
   else
   {   Loop = 0 ;
      while(1)
      {
         if(R_Buf >TEMP_R[Loop])
         {
            Loop++;   
         }
         else
         {
            break;
         }   
      }
      T = ((R_Buf - TEMP_R[Loop - 1])*100*Loop- (R_Buf -TEMP_R[Loop])*100*(Loop-1))/ (TEMP_R[Loop] - TEMP_R[Loop -1]) ;
   }
		return T ;   
}
