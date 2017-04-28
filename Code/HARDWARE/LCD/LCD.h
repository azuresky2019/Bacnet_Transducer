
#ifndef _LCD_H
#define _LCD_H 
#include "bitmap.h"
#include "delay.h"
#ifndef	TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif
#define LCD_BL		PEout(1)
#define LCD_WR		PEout(2)
#define LCD_RD		PEin(3)
#define LCD_RST		PEout(4)
#define LCD_CS		PEout(5)
#define LCD_REG		PEout(6)



#define BL_ON()		(LCD_BL = 0)
#define BL_OFF()	(LCD_BL = 1)


#define LCD_WRITE(x)	(GPIOE->ODR =(GPIOE->ODR&0x807F)|(x<<7))

//#define LCD_RD			P1_0
//#define LCD_WR			P1_1
//#define LCD_RS			P1_2
//#define LCD_RST			P1_3
//#define LCD_CS			P1_4
//#define LCD_BLA			P1_5






#define	LCD_BLA_OFF()	(GPIOE->BSRR = GPIO_Pin_1)					//(LCD_BLA = 0)
#define	LCD_BLA_ON()	(GPIOE->BRR = GPIO_Pin_1)  					//(LCD_BLA = 1)
#define	LCD_SET_WR()	(GPIOE->BSRR = GPIO_Pin_2)  				//(LCD_WR = 1)
#define LCD_CLR_WR()	(GPIOE->BRR = GPIO_Pin_2)   				//(LCD_WR = 0)
#define	LCD_SET_RD()	(GPIOE->BSRR = GPIO_Pin_3)   
#define LCD_CLR_RD()	(GPIOE->BRR = GPIO_Pin_3)  
#define	LCD_SET_RST()	(GPIOE->BSRR = GPIO_Pin_4)   				//(LCD_RST = 1)
#define LCD_CLR_RST()	(GPIOE->BRR = GPIO_Pin_4) 					//(LCD_RST = 0)
#define	LCD_SET_CS()	(GPIOE->BSRR = GPIO_Pin_5) 					// (LCD_CS = 1)
#define LCD_CLR_CS()	(GPIOE->BRR = GPIO_Pin_5)					//(LCD_CS = 0)
#define	LCD_SET_RS()	(GPIOE->BSRR = GPIO_Pin_6)  				//(LCD_Reg = 1)
#define LCD_CLR_RS()	(GPIOE->BRR = GPIO_Pin_6) 					//(LCD_Reg = 0)


#define MAX_ROW   			8
#define MAX_ITEM_LINE		4

#define	MAX_LINE	16
#define DISP_INV  1
#define DISP_NOR  0

void lcd_io_init(void) ;
void lcd_init(void);
void lcd_full_screen(uint8_t fill);
void lcd_print(uint8_t x, uint8_t y, uint8_t *p);
void center_print_str(uint8_t *p, uint8_t point_position) ;
void bottom_print( uint8_t *p) ;
void display_character_with_start_bit(uint8_t row, uint8_t start_line, uint8_t start_bit, uint8_t end_bit, uint8_t c, uint8_t disp_mode) ;

void start_scrolling(void) ;
void stop_scrolling(void) ;
void scrolling_message(void) ;

void Lcd_Show_String(uint8_t pos_row, uint8_t pos_line, uint8_t disp_mode, uint8_t *str) ;
void print_unit(uint8_t type);


extern uint8_t scroll_message[80] ; 
extern uint8_t scroll_message_length ;

extern uint8_t backlight_timer ;
extern 	 uint8_t start_bit ;
extern 	 uint8_t start_byte ;
void center_print(uint8_t col_addr, uint8_t p, uint8_t point) ;

#endif










