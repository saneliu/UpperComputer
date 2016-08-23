#include "bsp_ov7725.h"
#include "bsp_sccb.h"
#include "lcd.h"

typedef struct Reg
{
    uint8_t Address;			       /*寄存器地址*/
    uint8_t Value;		           /*寄存器值*/
} Reg_Info;

/* 寄存器参数配置 */
Reg_Info Sensor_Config[] =
{
    {CLKRC,     0x00}, /*clock config*/
    {COM7,      0x46}, /*QVGA YUV */
    {HSTART,    0x3f},
    {HSIZE,     0x50},
    {VSTRT,     0x03},
    {VSIZE,     0x78},
    {HREF,      0x00},
    {HOutSize,  0x50},
    {VOutSize,  0x78},
    {EXHCH,     0x00},

    /*DSP control*/
    {TGT_B,     0x7f},
    {FixGain,   0x09},
    {AWB_Ctrl0, 0xe0},
    {DSP_Ctrl1, 0xff},
    {DSP_Ctrl2, 0x20},
    {DSP_Ctrl3,	0x00},
    {DSP_Ctrl4, 0x00},

    /*AGC AEC AWB*/
    {COM8,		  0xf0},
    {COM4,		  0x81}, /*Pll AEC CONFIG*/
    {COM6,		  0xc5},
    {COM9,		  0x21},
    {BDBase,	  0xFF},
    {BDMStep,	  0x01},
    {AEW,		    0x34},
    {AEB,		    0x3c},
    {VPT,		    0xa1},
    {EXHCL,		  0x00},
    {AWBCtrl3,  0xaa},
    {COM8,		  0xff},
    {AWBCtrl1,  0x5d},

    {EDGE1,		  0x0a},
    {DNSOff,	  0x01},
    {EDGE2,		  0x01},
    {EDGE3,		  0x01},

    {MTX1,		  0x5f},
    {MTX2,		  0x53},
    {MTX3,		  0x11},
    {MTX4,		  0x1a},
    {MTX5,		  0x3d},
    {MTX6,		  0x5a},
    {MTX_Ctrl,  0x1e},

    {BRIGHT,	  0x00},
    {CNST,		  0x25},
    {USAT,		  0x65},
    {VSAT,		  0x65},
    {UVADJ0,	  0x81},
    {SDE,		    0x06},

    /*GAMMA config*/
    {GAM1,		  0x0c},
    {GAM2,		  0x16},
    {GAM3,		  0x2a},
    {GAM4,		  0x4e},
    {GAM5,		  0x61},
    {GAM6,		  0x6f},
    {GAM7,		  0x7b},
    {GAM8,		  0x86},
    {GAM9,		  0x8e},
    {GAM10,		  0x97},
    {GAM11,		  0xa4},
    {GAM12,		  0xaf},
    {GAM13,		  0xc5},
    {GAM14,		  0xd7},
    {GAM15,		  0xe8},
    {SLOP,		  0x20},

    {HUECOS,	  0x80},
    {HUESIN,	  0x80},
    {DSPAuto,	  0xff},
    {DM_LNL,	  0x00},
    {BDBase,	  0x99},
    {BDMStep,	  0x03},
    {LC_RADI,	  0x00},
    {LC_COEF,	  0x13},
    {LC_XC,		  0x08},
    {LC_COEFB,  0x14},
    {LC_COEFR,  0x17},
    {LC_CTR,	  0x05},

    {COM3,		  0xd0},/*Horizontal mirror image*/

    /*night mode auto frame rate control*/
    {COM5,		0xf5},	 /*在夜视环境下，自动降低帧率，保证低照度画面质量*/
    //{COM5,		0x31},	/*夜视环境帧率不变*/
};

uint8_t OV7725_REG_NUM = sizeof(Sensor_Config) / sizeof(
                             Sensor_Config[0]);	 /*结构体数组成员数目*/
/* 帧同步信号标志，在中断函数和main函数里面使用 */
volatile uint8_t Ov7725_vsync;

/************************************************
 * 函数名：FIFO_GPIO_Config
 * 描述  ：FIFO GPIO配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
static void FIFO_GPIO_Config(void)
{
	    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
	
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

    /*PD3(FIFO_WEN--FIFO写使能)*/
//    macOV7725_WE_SCK_APBxClock_FUN ( macOV7725_WE_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_WE_GPIO_PIN;
    HAL_GPIO_Init(macOV7725_WE_GPIO_PORT, &GPIO_InitStructure);

    /*PB5(FIFO_WRST--FIFO写复位)*/
//    macOV7725_WRST_SCK_APBxClock_FUN ( macOV7725_WRST_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_WRST_GPIO_PIN;
    HAL_GPIO_Init(macOV7725_WRST_GPIO_PORT, &GPIO_InitStructure);

    /*PA2(FIFO_RRST--FIFO读复位) PA3(FIFO_OE--FIFO输出使能)*/
//    macOV7725_RRST_SCK_APBxClock_FUN ( macOV7725_RRST_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_RRST_GPIO_PIN;
    HAL_GPIO_Init(macOV7725_RRST_GPIO_PORT, &GPIO_InitStructure);

//    macOV7725_CS_SCK_APBxClock_FUN ( macOV7725_CS_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_CS_GPIO_PIN;
    HAL_GPIO_Init(macOV7725_CS_GPIO_PORT, &GPIO_InitStructure);

    /*PC5(FIFO_RCLK-FIFO读时钟)*/
//    macOV7725_RCLK_SCK_APBxClock_FUN ( macOV7725_RCLK_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_RCLK_GPIO_PIN;
    HAL_GPIO_Init(macOV7725_RCLK_GPIO_PORT, &GPIO_InitStructure);

    /*PB8-PB15(FIFO_DATA--FIFO输出数据)*/
//    macOV7725_DATA_SCK_APBxClock_FUN ( macOV7725_DATA_GPIO_CLK, ENABLE );
    GPIO_InitStructure.Pin = macOV7725_DATA_0_GPIO_PIN |
                             macOV7725_DATA_1_GPIO_PIN | macOV7725_DATA_2_GPIO_PIN |
                             macOV7725_DATA_3_GPIO_PIN |
                             macOV7725_DATA_4_GPIO_PIN | macOV7725_DATA_5_GPIO_PIN |
                             macOV7725_DATA_6_GPIO_PIN | macOV7725_DATA_7_GPIO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(macOV7725_DATA_GPIO_PORT, &GPIO_InitStructure);


    FIFO_CS_L();	  					/*拉低使FIFO输出使能*/
    FIFO_WE_H();   						/*拉高使FIFO写允许*/


}

void Ov7725_GPIO_Config(void)
{
    SCCB_GPIO_Config();
    FIFO_GPIO_Config();
}

/************************************************
 * 函数名：VSYNC_Init
 * 描述  ：OV7725 VSYNC中断相关配置
 * 输入  ：无
 * 输出  ：无
 * 注意  ：无
 ************************************************/
void VSYNC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


//    macOV7725_VSYNC_SCK_APBxClock_FUN ( macOV7725_VSYNC_GPIO_CLK,
//                                        ENABLE );	  /*PA0---VSYNC*/
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;    //下降沿触发
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Pin =  macOV7725_VSYNC_GPIO_PIN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_NVIC_SetPriority(macOV7725_VSYNC_EXTI_IRQ, 2, 0);
    HAL_NVIC_EnableIRQ(macOV7725_VSYNC_EXTI_IRQ);
    HAL_GPIO_Init(macOV7725_VSYNC_GPIO_PORT, &GPIO_InitStructure);
}

/************************************************
 * 函数名：Sensor_Init
 * 描述  ：Sensor初始化
 * 输入  ：无
 * 输出  ：返回1成功，返回0失败
 * 注意  ：无
 ************************************************/
ErrorStatus Ov7725_Init(void)
{
    uint16_t i = 0;
    uint8_t Sensor_IDCode = 0;

    //DEBUG("ov7725 Register Config Start......");

    if ( 0 == SCCB_WriteByte ( 0x12, 0x80 ) ) /*复位sensor */
    {
        //DEBUG("sccb write data error");
        return ERROR ;
    }

    if ( 0 == SCCB_ReadByte( &Sensor_IDCode, 1, 0x0b ) )	 /* 读取sensor ID号*/
    {
        //DEBUG("read id faild");
        return ERROR;
    }
    //DEBUG("Sensor ID is 0x%x", Sensor_IDCode);

    if (Sensor_IDCode == OV7725_ID)
    {
        for ( i = 0 ; i < OV7725_REG_NUM ; i++ )
        {
            if ( 0 == SCCB_WriteByte(Sensor_Config[i].Address, Sensor_Config[i].Value) )
            {
                //DEBUG("write reg faild", Sensor_Config[i].Address);
                return ERROR;
            }
        }
    }
    else
    {
        return ERROR;
    }
    //DEBUG("ov7725 Register Config Success");

    return SUCCESS;
}


/*       320
 * -------------------
 *|                   |
 *|                   |
 *|                   |  240
 *|                   |
 *|                   |
 * -------------------
 */
void ImagDisp(void)
{
    uint16_t Camera_Data;
//	  uint8_t  light_send[160][120];
    uint16_t i , j ;
//    uint8_t R;
//    uint8_t G;
//    uint8_t B;
//    USART1->DR = 0x00;
//    USART1->DR = 0x42;
//    while ((USART1->SR & 0X40) == 0);	//循环发送,直到发送完毕

    for ( i = 0; i < 240; i++)
    {
        for ( j = 0; j < 320; j++)
        {

            READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
//					if((i%2==0)&&(j%2==0))
//					{
            LCD_SetCursor(j, i);	//设置光标位置
            LCD_WriteRAM_Prepare();	//开始写入GRAM
            LCD_WR_DATA(Camera_Data);

//            R = (uint8_t)((Camera_Data & 0XF700) >> 11);
//            G = (uint8_t)((Camera_Data & 0X07E0) >> 5);
//            B = (uint8_t)(Camera_Data & 0X001F);
//            light_send[j][i] = (uint8_t)(0.3 * R + 0.6 * G + 0.1 * B);

//            if ( light_send[j/2][i/2] > 125)
//            {
//                light_send[j/2][i/2] = 1;
//            }
//            else
//            {
//                light_send[j/2][i/2] = 0;
//            }
//		       USART1->DR = light_send[j/2][i/2];
//					}
        }

    }

//    USART1->DR = 0x0d;
//    USART1->DR = 0x0a;
//    while ((USART1->SR & 0X40) == 0);	//循环发送,直到发送完毕
}


//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(100);      //消抖
    switch (GPIO_Pin)
    {
        case GPIO_PIN_3:
            if ( Ov7725_vsync == 0 )
            {
                FIFO_WRST_L(); 	                      //拉低使FIFO写(数据from摄像头)指针复位
                FIFO_WE_H();	                        //拉高使FIFO写允许

                Ov7725_vsync = 1;
                FIFO_WE_H();                          //使FIFO写允许
                FIFO_WRST_H();                        //允许使FIFO写(数据from摄像头)指针运动
            }
            else if ( Ov7725_vsync == 1 )
            {
                FIFO_WE_L();                          //拉低使FIFO写暂停
                Ov7725_vsync = 2;
            }
            break;
    }
}

