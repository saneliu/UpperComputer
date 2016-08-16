#ifndef __BLUE_TOOTH_H
#define __BLUE_TOOTH_H

#include "stm32f1xx_hal.h"

#define USART_REC_LEN  			20  	//定义最大接收字节数 200
extern uint8_t
USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern uint16_t USART_RX_STA;         		//接收状态标记

/***************接收数据类型***************/
#define  Rec_Balance_P 0x20
#define  Rec_Balance_I 0x21
#define  Rec_Balance_D 0x22
#define  Rec_Position_P 0x23
#define  Rec_Position_I 0x24
#define  Rec_Position_D 0x25
#define  Rec_Speed_P 0x26
#define  Rec_Speed_I 0x27
#define  Rec_Speed_D 0x28
#define  Rec_Steer_P 0x29
#define  Rec_Steer_I 0x30
#define  Rec_Steer_D 0x31
#define  Rec_Forward  0x32
#define  Rec_Backward  0x33
#define  Rec_TurnLeft  0x34
#define  Rec_TurnRight  0x35
#define  Rec_Firing  0x36
#define  Rec_Misfire  0x37
void UpperComputer(uint8_t *usart_receive);

#endif
