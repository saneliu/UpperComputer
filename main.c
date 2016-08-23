//#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "bsp_ov7725.h"
//ALIENTEK Mini STM32开发板范例代码11
//TFTLCD显示实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
extern uint8_t Ov7725_vsync;
int main(void)
{
    delay_init();	    	 //延时函数初始化
    uart_init(315200);	 	//串口初始化为9600
    LCD_Init();
    LCD_Display_Dir(1);
	  /* 设置液晶扫描方向为 左下角->右上角 */
    LCD_Scan_Dir(L2R_D2U);
    /* ov7725 gpio 初始化 */
    Ov7725_GPIO_Config();

    /* ov7725 寄存器配置初始化 */
    while (Ov7725_Init() != SUCCESS);

    /* ov7725 场信号线初始化 */
    VSYNC_Init();
    Ov7725_vsync = 0;
    while (1)
    {
        if ( Ov7725_vsync == 2 )
        {
            FIFO_PREPARE;  			/*FIFO准备*/
            ImagDisp();					/*采集并显示*/
            Ov7725_vsync = 0;
        }
    }

}
