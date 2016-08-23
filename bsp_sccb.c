#include "bsp_sccb.h"

#define DEV_ADR  ADDR_OV7725 			 /*�豸��ַ����*/

/********************************************************************
 * ��������SCCB_Configuration
 * ����  ��SCCB�ܽ�����
 * ����  ����
 * ���  ����
 * ע��  ����        
 ********************************************************************/
void SCCB_GPIO_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
	
	
  /* SCL(PC6)��SDA(PC7)�ܽ����� */
	//macOV7725_SIO_C_SCK_APBxClock_FUN ( macOV7725_SIO_C_GPIO_CLK, ENABLE );
  GPIO_InitStructure.Pin =  macOV7725_SIO_C_GPIO_PIN ;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;  
  HAL_GPIO_Init(macOV7725_SIO_C_GPIO_PORT, &GPIO_InitStructure);
	
//	macOV7725_SIO_D_SCK_APBxClock_FUN ( macOV7725_SIO_D_GPIO_CLK, ENABLE );
  GPIO_InitStructure.Pin =  macOV7725_SIO_D_GPIO_PIN ;
  HAL_GPIO_Init(macOV7725_SIO_D_GPIO_PORT, &GPIO_InitStructure);
	
}

/********************************************************************
 * ��������SCCB_delay
 * ����  ���ӳ�ʱ��
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static void SCCB_delay(void)
{	
   uint16_t i = 400; 
   while(i) 
   { 
     i--; 
   } 
}

/********************************************************************
 * ��������SCCB_Start
 * ����  ��SCCB��ʼ�ź�
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static int SCCB_Start(void)
{
	SDA_H;
	SCL_H;
	SCCB_delay();
	if(!SDA_read)
	return DISABLE;	/* SDA��Ϊ�͵�ƽ������æ,�˳� */
	SDA_L;
	SCCB_delay();
	if(SDA_read) 
	return DISABLE;	/* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
	SDA_L;
	SCCB_delay();
	return ENABLE;
}



/********************************************************************
 * ��������SCCB_Stop
 * ����  ��SCCBֹͣ�ź�
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static void SCCB_Stop(void)
{
	SCL_L;
	SCCB_delay();
	SDA_L;
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	SDA_H;
	SCCB_delay();
}



/********************************************************************
 * ��������SCCB_Ack
 * ����  ��SCCBӦ��ʽ
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static void SCCB_Ack(void)
{	
	SCL_L;
	SCCB_delay();
	SDA_L;
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	SCL_L;
	SCCB_delay();
}



/********************************************************************
 * ��������SCCB_NoAck
 * ����  ��SCCB ��Ӧ��ʽ
 * ����  ����
 * ���  ����
 * ע��  ���ڲ�����        
 ********************************************************************/
static void SCCB_NoAck(void)
{	
	SCL_L;
	SCCB_delay();
	SDA_H;
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	SCL_L;
	SCCB_delay();
}

/********************************************************************
 * ��������SCCB_WaitAck
 * ����  ��SCCB �ȴ�Ӧ��
 * ����  ����
 * ���  ������Ϊ:=1��ACK,=0��ACK
 * ע��  ���ڲ�����        
 ********************************************************************/
static int SCCB_WaitAck(void) 	
{
	SCL_L;
	SCCB_delay();
	SDA_H;			
	SCCB_delay();
	SCL_H;
	SCCB_delay();
	if(SDA_read)
	{
      SCL_L;
      return DISABLE;
	}
	SCL_L;
	return ENABLE;
}



 /*******************************************************************
 * ��������SCCB_SendByte
 * ����  �����ݴӸ�λ����λ
 * ����  ��SendByte: ���͵�����
 * ���  ����
 * ע��  ���ڲ�����        
 *********************************************************************/
static void SCCB_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
        SCL_L;
        SCCB_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        SCCB_delay();
		SCL_H;
        SCCB_delay();
    }
    SCL_L;
}




 /******************************************************************
 * ��������SCCB_ReceiveByte
 * ����  �����ݴӸ�λ����λ
 * ����  ����
 * ���  ��SCCB���߷��ص�����
 * ע��  ���ڲ�����        
 *******************************************************************/
static int SCCB_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      SCCB_delay();
	  SCL_H;
      SCCB_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}





 /*****************************************************************************************
 * ��������SCCB_WriteByte
 * ����  ��дһ�ֽ�����
 * ����  ��- WriteAddress: ��д���ַ 	- SendByte: ��д������	- DeviceAddress: ��������
 * ���  ������Ϊ:=1�ɹ�д��,=0ʧ��
 * ע��  ����        
 *****************************************************************************************/           
int SCCB_WriteByte( uint16_t WriteAddress , uint8_t SendByte )
{		
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );                    /* ������ַ */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte((uint8_t)(WriteAddress & 0x00FF));   /* ���õ���ʼ��ַ */      
    SCCB_WaitAck();	
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();   
    SCCB_Stop(); 
    return ENABLE;
}

/******************************************************************************************************************
 * ��������SCCB_ReadByte
 * ����  ����ȡһ������
 * ����  ��- pBuffer: ��Ŷ������� 	- length: ����������	- ReadAddress: ��������ַ		 - DeviceAddress: ��������
 * ���  ������Ϊ:=1�ɹ�����,=0ʧ��
 * ע��  ����        
 **********************************************************************************************************************/           
int SCCB_ReadByte(uint8_t* pBuffer, uint16_t length, uint8_t ReadAddress)
{	
    if(!SCCB_Start())
	{
	    return DISABLE;
	}
    SCCB_SendByte( DEV_ADR );         /* ������ַ */
    if( !SCCB_WaitAck() )
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    SCCB_SendByte( ReadAddress );     /* ���õ���ʼ��ַ */      
    SCCB_WaitAck();	
    SCCB_Stop(); 
	
    if(!SCCB_Start())
	{
		return DISABLE;
	}
    SCCB_SendByte( DEV_ADR + 1 );     /* ������ַ */ 
    if(!SCCB_WaitAck())
	{
		SCCB_Stop(); 
		return DISABLE;
	}
    while(length)
    {
      *pBuffer = SCCB_ReceiveByte();
      if(length == 1)
	  {
		  SCCB_NoAck();
	  }
      else
	  {
		SCCB_Ack(); 
	  }
      pBuffer++;
      length--;
    }
    SCCB_Stop();
    return ENABLE;
}
/*********************************************END OF FILE**********************/
