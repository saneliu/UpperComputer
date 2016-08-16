#include "BlueTooth.h"

float f1, f2, f3, F4, F5, F6;
uint8_t temp;
uint8_t USART_RX_BUF[USART_REC_LEN] = {0};   //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
uint16_t USART_RX_STA = 0;     //����״̬���

/**
  * @brief  ������λ������
  * @param  usart_receive ���մ�������
	  * @arg  ��һλ  0x00  ����
		* @arg  �ڶ�λ  ��Ӧ����
		* @arg  ����λ  ������������
		* @arg  ����λ  ���ݵ�һ��λС��
		* @arg  ����λ  0x0d  ����
		* @arg  ����λ  0x0a  ����
	* @retval NONE
  * @attention
    @arg
  */
void UpperComputer(uint8_t *usart_receive)
{
    float usart_temp;
    if ((USART_RX_STA & 0x8000) == 0) //����δ���
    {
        if (USART_RX_STA & 0x4000) //���յ���0x0d
        {
            if (*usart_receive != 0x0a)
            {
                USART_RX_STA = 0; //���մ���,���¿�ʼ
            }
            else
            {
                USART_RX_STA |= 0x8000;	//���������
            }
        }
        else //��û�յ�0X0D
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
                    USART_RX_STA =  0; //�������ݴ���,���¿�ʼ����
                }
            }
        }
    }

    if (USART_RX_STA & 0x8000)
    {
        //�����һλ  ȷ��������ȷ
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
