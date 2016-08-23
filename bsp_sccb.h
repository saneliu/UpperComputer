#ifndef __SCCB_H
#define __SCCB_H

#include "stm32f1xx_hal.h"

/************************** OV7725 连接引脚定义********************************/
//#define      macOV7725_SIO_C_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
//#define      macOV7725_SIO_C_GPIO_CLK                       RCC_APB2Periph_GPIOC
#define      macOV7725_SIO_C_GPIO_PORT                      GPIOC
#define      macOV7725_SIO_C_GPIO_PIN                       GPIO_PIN_0

//#define      macOV7725_SIO_D_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
//#define      macOV7725_SIO_D_GPIO_CLK                       RCC_APB2Periph_GPIOC
#define      macOV7725_SIO_D_GPIO_PORT                      GPIOC
#define      macOV7725_SIO_D_GPIO_PIN                       GPIO_PIN_13



#define SCL_H         HAL_GPIO_WritePin(macOV7725_SIO_C_GPIO_PORT , macOV7725_SIO_C_GPIO_PIN,GPIO_PIN_SET) 
#define SCL_L         HAL_GPIO_WritePin(macOV7725_SIO_C_GPIO_PORT , macOV7725_SIO_C_GPIO_PIN,GPIO_PIN_RESET) 
   
#define SDA_H         HAL_GPIO_WritePin(macOV7725_SIO_D_GPIO_PORT , macOV7725_SIO_D_GPIO_PIN,GPIO_PIN_SET) 
#define SDA_L         HAL_GPIO_WritePin(macOV7725_SIO_D_GPIO_PORT , macOV7725_SIO_D_GPIO_PIN,GPIO_PIN_RESET) 

#define SCL_read      HAL_GPIO_ReadPin(macOV7725_SIO_C_GPIO_PORT , macOV7725_SIO_C_GPIO_PIN) 
#define SDA_read      HAL_GPIO_ReadPin(macOV7725_SIO_D_GPIO_PORT , macOV7725_SIO_D_GPIO_PIN) 

#define ADDR_OV7725   0x42



void SCCB_GPIO_Config(void);
int SCCB_WriteByte( uint16_t WriteAddress , uint8_t SendByte);
int SCCB_ReadByte(uint8_t* pBuffer,   uint16_t length,   uint8_t ReadAddress);



#endif 
