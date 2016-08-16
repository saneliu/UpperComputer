#include "BlueTooth.h"

float f1, f2, f3, F4, F5, F6;
uint8_t temp;
uint8_t USART_RX_BUF[USART_REC_LEN] = {0};   //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA = 0;     //接收状态标记

/**
  * @brief  接收上位机数据
  * @param  usart_receive 接收串口数组
	  * @arg  第一位  0x00  检验
		* @arg  第二位  对应数据
		* @arg  第三位  数据整数部分
		* @arg  第四位  数据第一二位小数
		* @arg  第五位  0x0d  检验
		* @arg  第六位  0x0a  检验
	* @retval NONE
  * @attention
    @arg
  */
void UpperComputer(uint8_t *usart_receive)
{
    float usart_temp;
    if ((USART_RX_STA & 0x8000) == 0) //接收未完成
    {
        if (USART_RX_STA & 0x4000) //接收到了0x0d
        {
            if (*usart_receive != 0x0a)
            {
                USART_RX_STA = 0; //接收错误,重新开始
            }
            else
            {
                USART_RX_STA |= 0x8000;	//接收完成了
            }
        }
        else //还没收到0X0D
        {
            if (*usart_receive == 0x0d)
            {
                USART_RX_STA |= 0x4000;
            }
            else
            {
                USART_RX_BUF[USART_RX_STA & 0X3FFF] = *usart_receive ;
                USART_RX_STA++;
                if (USART_RX_STA > (USART_REC_LEN - 1))
                {
                    USART_RX_STA =  0; //接收数据错误,重新开始接收
                }
            }
        }
    }

    if (USART_RX_STA & 0x8000)
    {
        //检验第一位  确保数据正确
        if (USART_RX_BUF[0] == 0xff)
        {
            usart_temp = (float)(USART_RX_BUF[2]) + ((float)USART_RX_BUF[3]) / 100;
            switch (USART_RX_BUF[1])
            {
                case Rec_Balance_P:
                    f1 = usart_temp;
                    break;
                case Rec_Balance_I:
                    
                    break;
                case Rec_Balance_D:
                    
                    break;
                case Rec_Position_P:
                    
                    break;
                case Rec_Position_I:
                    
                    break;
                case Rec_Position_D:
                    
                    break;
                case Rec_Speed_P:
                    
                    break;
                case Rec_Speed_I:
                    
                    break;
                case Rec_Speed_D:
                    
                    break;
                case Rec_Steer_P:
                    
                    break;
                case Rec_Steer_I:
                    
                    break;
                case Rec_Steer_D:
                    
                    break;
                case Rec_Forward:
                    
                    break;
                case Rec_Backward:
                    
                    break;
                case Rec_TurnLeft:
                    
                    break;
                case Rec_TurnRight:
                    
                    break;
                case Rec_Firing:
                    
                    break;
                case Rec_Misfire:
                    
                    break;


            }
        }

        USART_RX_STA = 0;
    }



}
