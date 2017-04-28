#ifndef __KEY_H
#define __KEY_H
 
#include "bitmap.h"
#include "define.h"	


#define	KEY_NON		0x00
#define	KEY_LEFT	0X01
#define	KEY_RIGHT	0X02
#define	KEY_UP		0X03
#define	KEY_DOWN	0X04

extern uint8_t key_value;
extern uint8_t pre_key_value;


void key_cope(void);
void switch_init(uint8_t channle) ;
void key_init(uint8_t channel) ;



					    
#endif
