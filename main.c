//#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "bsp_ov7725.h"
//ALIENTEK Mini STM32�����巶������11
//TFTLCD��ʾʵ��
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
extern uint8_t Ov7725_vsync;
int main(void)
{
    delay_init();	    	 //��ʱ������ʼ��
    uart_init(315200);	 	//���ڳ�ʼ��Ϊ9600
    LCD_Init();
    LCD_Display_Dir(1);
	  /* ����Һ��ɨ�跽��Ϊ ���½�->���Ͻ� */
    LCD_Scan_Dir(L2R_D2U);
    /* ov7725 gpio ��ʼ�� */
    Ov7725_GPIO_Config();

    /* ov7725 �Ĵ������ó�ʼ�� */
    while (Ov7725_Init() != SUCCESS);

    /* ov7725 ���ź��߳�ʼ�� */
    VSYNC_Init();
    Ov7725_vsync = 0;
    while (1)
    {
        if ( Ov7725_vsync == 2 )
        {
            FIFO_PREPARE;  			/*FIFO׼��*/
            ImagDisp();					/*�ɼ�����ʾ*/
            Ov7725_vsync = 0;
        }
    }

}
