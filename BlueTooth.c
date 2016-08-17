/**
  ******************************************************************************
  * @file    Blue_Tooth.c
  * @author  WangRong
  * @version V1.0
  * @date    2016.8.17
  * @note    ��������
  ******************************************************************************
  */
#include "BlueTooth.h"

extern UART_HandleTypeDef huart1;
float f1;
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
	  * @arg  ��һλ  0xff  ����
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
    uint8_t len;
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
			    len = USART_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
        //�����һλ  ȷ��������ȷ
        if (USART_RX_BUF[0] == 0xff)
        {
            if (len == 4)
            {
                usart_temp = (float)(USART_RX_BUF[2]) + ((float)USART_RX_BUF[3]) / 100;
            }
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
                    temp = 3;
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
/**
  * @brief  ��uint8_t�������ݾ�����λ��
  * @param
    * @arg  data  Ҫ���͵�����
    * @arg  type  ������������
	* @retval NONE
  * @attention
		ͨѶЭ��
    * @arg  ��һλ  0xff  ����
		* @arg  �ڶ�λ  ��Ӧ��������
		* @arg  ����λ  Ҫ���͵�����
		* @arg  ����λ  0x0d  ����
		* @arg  ����λ  0x0a  ����
  */
void Send_Uint8_Data(uint8_t data, uint8_t type)
{
    uint8_t SendData[5] = {0xff, type, data, 0x0d, 0x0a};
    HAL_UART_Transmit(&huart1 , SendData, 5, 0xff);
}
/**
  * @brief  ��float�������ݾ�����λ��
  * @param
    * @arg  data  Ҫ���͵�����
    * @arg  type  ������������
	* @retval NONE
  * @attention
		ͨѶЭ��
    * @arg  ��һλ  0xff  ����
		* @arg  �ڶ�λ  ��Ӧ��������
		* @arg  ����λ  Ҫ���͵���������λ
		* @arg  ����λ  Ҫ���͵�����С��λ
		* @arg  ����λ  0x0d  ����
		* @arg  ����λ  0x0a  ����
  */
void Send_Float_Data(float data, uint8_t type)
{
    uint8_t SendData[6] = {0xff, type, 0x00, 0x00, 0x0d, 0x0a};
    //temp_1��������  temp_2��λС��
    uint8_t temp_1, temp_2;
    temp_1 = (uint8_t)data;
		SendData[2] = temp_1;
    temp_2 = (uint8_t)(100 * (data - (float)temp_1));
    SendData[3] = temp_2;

    HAL_UART_Transmit(&huart1 , SendData, 6, 0xff);
}
