#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
 
 static uint8_t  fac_us=0;							//us��ʱ������	
//LCD�Ļ�����ɫ�ͱ���ɫ	   
uint16_t POINT_COLOR=0x0000;	//������ɫ
uint16_t BACK_COLOR=0xFFFF;  //����ɫ 

//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//ʱ�����	  		 
	SysTick->VAL=0x00;        					//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0X00;      					 //��ռ�����	 
}

//д�Ĵ�������
//data:�Ĵ���ֵ
void LCD_WR_REG(uint16_t data)
{ 
	LCD_RS_CLR;//д��ַ  
 	LCD_CS_CLR; 
	DATAOUT(data); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;   
}
//д���ݺ���
//�������LCD_WR_DATAX��,��ʱ�任�ռ�.
//data:�Ĵ���ֵ
void LCD_WR_DATAX(uint16_t data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
}
//��LCD����
//����ֵ:������ֵ
uint16_t LCD_RD_DATA(void)
{										   
	uint16_t t;
 	GPIOB->CRL=0X88888888; //PB0-7  ��������
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOB->ODR=0X0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;
	if(lcddev.id==0X8989)delay_us(2);//FOR 8989,��ʱ2us					   
	t=DATAIN;  
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  �������
	GPIOB->CRH=0X33333333; //PB8-15 �������
	GPIOB->ODR=0XFFFF;    //ȫ�������
	return t;  
}
//д�Ĵ���
//LCD_Reg:�Ĵ������
//LCD_RegValue:Ҫд���ֵ
void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}   
//���Ĵ���
//LCD_Reg:�Ĵ������
//����ֵ:������ֵ
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{										   
 	LCD_WR_REG(LCD_Reg);  //д��Ҫ���ļĴ�����  
	return LCD_RD_DATA(); 
} 
//��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
} 
//LCDдGRAM
//RGB_Code:��ɫֵ
void LCD_WriteRAM(uint16_t RGB_Code)
{							    
	LCD_WR_DATA(RGB_Code);//дʮ��λGRAM
}
//��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
//ͨ���ú���ת��
//c:GBR��ʽ����ɫֵ
//����ֵ��RGB��ʽ����ɫֵ
uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}	
//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
void opt_delay(uint8_t i)
{
	while(i--);
}
//��ȡ��ĳ�����ɫֵ	 
//x,y:����
//����ֵ:�˵����ɫ
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
 	uint16_t r,g,b;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(x,y);
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X2E);//9341/6804/3510/1963 ���Ͷ�GRAMָ��
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2E00);	//5510 ���Ͷ�GRAMָ��
	else LCD_WR_REG(0X22);      		 			//����IC���Ͷ�GRAMָ��
	GPIOB->CRL=0X88888888; 							//PB0-7  ��������
	GPIOB->CRH=0X88888888; 							//PB8-15 ��������
	GPIOB->ODR=0XFFFF;     							//ȫ�������

	LCD_RS_SET;
	LCD_CS_CLR;	    
	//��ȡ����(��GRAMʱ,��һ��Ϊ�ٶ�)	
	LCD_RD_CLR;		   
	opt_delay(2);									//��ʱ		
 	r=DATAIN;  										//ʵ��������ɫ 
	LCD_RD_SET;
	if(lcddev.id==0X1963)
	{
		LCD_CS_SET;
		GPIOB->CRL=0X33333333; 		//PB0-7  �������
		GPIOB->CRH=0X33333333; 		//PB8-15 �������
		GPIOB->ODR=0XFFFF;    		//ȫ�������  
		return r;					//1963ֱ�Ӷ��Ϳ��� 
 	}
 	//dummy READ
	LCD_RD_CLR;					   
	opt_delay(2);//��ʱ					   
 	r=DATAIN;  	//ʵ��������ɫ
	LCD_RD_SET;
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)	//9341/NT35310/NT35510Ҫ��2�ζ���
	{	 
		LCD_RD_CLR;					   
		opt_delay(2);//��ʱ			   
		b=DATAIN;//��ȡ��ɫֵ  	  
	 	LCD_RD_SET;
		g=r&0XFF;//����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
		g<<=8;
	}else if(lcddev.id==0X6804)
	{
		LCD_RD_CLR;					   
	 	LCD_RD_SET;
		r=DATAIN;//6804�ڶ��ζ�ȡ�Ĳ�����ʵֵ 
	}	 
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; 		//PB0-7  �������
	GPIOB->CRH=0X33333333; 		//PB8-15 �������
	GPIOB->ODR=0XFFFF;    		//ȫ�������  
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//�⼸��ICֱ�ӷ�����ɫֵ
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
	else return LCD_BGR2RGB(r);	//����IC
}		 
//LCD������ʾ
void LCD_DisplayOn(void)
{					   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X29);	//������ʾ
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);	//������ʾ
	else LCD_WriteReg(0X07,0x0173); 				 	//������ʾ
}	 
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X1963)LCD_WR_REG(0X28);	//�ر���ʾ
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);	//�ر���ʾ
	else LCD_WriteReg(0X07,0x0);//�ر���ʾ 
}   
//���ù��λ��
//Xpos:������
//Ypos:������
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(Xpos>>8);LCD_WR_DATA(Xpos&0XFF); 			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(Ypos>>8);LCD_WR_DATA(Ypos&0XFF); 		
	}
} 		 
//����LCD���Զ�ɨ�跽��
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���	   	   
void LCD_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;  
	if((lcddev.dir==1&&lcddev.id!=0X6804&&lcddev.id!=0X1963)||(lcddev.dir==0&&lcddev.id==0X1963))//����ʱ����6804��1963���ı�ɨ�跽������ʱ1963�ı䷽��
	{			   
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510||lcddev.id==0X1963)//9341/6804/5310/5510/1963,���⴦��
	{
		switch(dir)
		{
			case L2R_U2D://������,���ϵ���
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://������,���µ���
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://���ҵ���,���µ���
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://���ϵ���,������
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://���µ���,������
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://���µ���,���ҵ���
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510)&&(lcddev.id!=0X1963))regval|=0X08;//5310/5510/1963����ҪBGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804��BIT6��9341�ķ���	   
		LCD_WriteReg(dirreg,regval);
		if(lcddev.id!=0X1963)//1963�������괦��
		{
			if(regval&0X20)
			{
				if(lcddev.width<lcddev.height)//����X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}else  
			{
				if(lcddev.width>lcddev.height)//����X,Y
				{
					temp=lcddev.width;
					lcddev.width=lcddev.height;
					lcddev.height=temp;
				}
			}  
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((lcddev.width-1)>>8); 
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((lcddev.width-1)&0XFF); 
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0); 
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((lcddev.height-1)>>8); 
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((lcddev.height-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.width-1)>>8);LCD_WR_DATA((lcddev.width-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd); 
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
		}
  	}else 
	{
		switch(dir)
		{
			case L2R_U2D://������,���ϵ���
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://������,���µ���
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://���ҵ���,���ϵ���
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://���ҵ���,���µ���
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://���ϵ���,������
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://���ϵ���,���ҵ���
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://���µ���,������
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://���µ���,���ҵ���
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		} 
		dirreg=0X03;
		regval|=1<<12; 
		LCD_WriteReg(dirreg,regval);
	}
}     
//����
//x,y:����
//POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR); 
}	 
//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(x>>8);LCD_WR_DATA(x&0XFF);  			 
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(y>>8);LCD_WR_DATA(y&0XFF); 		 	 
	}

	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(lcddev.wramcmd);//дָ��  
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET; 
	LCD_WR_DATA(color);		//д����
}
//SSD1963 ��������
//pwm:����ȼ�,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(uint8_t pwm)
{	
	LCD_WR_REG(0xBE);	//����PWM���
	LCD_WR_DATA(0x05);	//1����PWMƵ��
	LCD_WR_DATA(pwm*2.55);//2����PWMռ�ձ�
	LCD_WR_DATA(0x01);	//3����C
	LCD_WR_DATA(0xFF);	//4����D
	LCD_WR_DATA(0x00);	//5����E
	LCD_WR_DATA(0x00);	//6����F
}
//����LCD��ʾ����
//dir:0,������1,����
void LCD_Display_Dir(uint8_t dir)
{
	if(dir==0)			//����
	{
		lcddev.dir=0;	//����
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}

	}
	else 				//����
	{	  				
		lcddev.dir=1;	//����
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}

		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}	 
//���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height. 
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{    
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval; 
	uint16_t twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804||(lcddev.dir==1&&lcddev.id==0X1963))
	{
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(sx>>8); 
		LCD_WR_DATA(sx&0XFF);	 
		LCD_WR_DATA(twidth>>8); 
		LCD_WR_DATA(twidth&0XFF);  
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(sy>>8); 
		LCD_WR_DATA(sy&0XFF); 
		LCD_WR_DATA(theight>>8); 
		LCD_WR_DATA(theight&0XFF); 
	}

	else	//��������IC
	{
		if(lcddev.dir==1)//����
		{
			//����ֵ
			hsaval=sy;				
			heaval=theight;
			vsaval=lcddev.width-twidth-1;
			veaval=lcddev.width-sx-1;				
		}else
		{ 
			hsaval=sx;				
			heaval=twidth;
			vsaval=sy;
			veaval=theight;
		} 
		hsareg=0X50;heareg=0X51;//ˮƽ���򴰿ڼĴ���
		vsareg=0X52;veareg=0X53;//��ֱ���򴰿ڼĴ���	   							  
		//���üĴ���ֵ
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);		
		LCD_SetCursor(sx,sy);	//���ù��λ��
	}
}
//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ������ALIENTEK��Ʒ��LCDҺ����
//������ռ�ýϴ�flash,�û����Ը����Լ���ʵ�����,ɾ��δ�õ���LCD��ʼ������.�Խ�ʡ�ռ�.
void LCD_Init(void)
{ 

HAL_Delay(50); // delay 50 ms 
	LCD_WriteReg(0x0000,0x0001);
	HAL_Delay(50); // delay 50 ms 
  	lcddev.id = LCD_ReadReg(0x0000);   
	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//����ID����ȷ,����lcddev.id==0X9300�жϣ���Ϊ9341��δ����λ������»ᱻ����9300
	{	
 		//����9341 ID�Ķ�ȡ		
		LCD_WR_REG(0XD3);				   
		LCD_RD_DATA(); 				//dummy read 	
 		LCD_RD_DATA();   	    	//����0X00
  		lcddev.id=LCD_RD_DATA();   	//��ȡ93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();  	//��ȡ41 	   			   

	}
// 	printf(" LCD ID:%x\r\n",lcddev.id); //��ӡLCD ID  
	if(lcddev.id==0X9341)	//9341��ʼ��
	{	 
		LCD_WR_REG(0xCF);  
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0xC1); 
		LCD_WR_DATAX(0X30); 
		LCD_WR_REG(0xED);  
		LCD_WR_DATAX(0x64); 
		LCD_WR_DATAX(0x03); 
		LCD_WR_DATAX(0X12); 
		LCD_WR_DATAX(0X81); 
		LCD_WR_REG(0xE8);  
		LCD_WR_DATAX(0x85); 
		LCD_WR_DATAX(0x10); 
		LCD_WR_DATAX(0x7A); 
		LCD_WR_REG(0xCB);  
		LCD_WR_DATAX(0x39); 
		LCD_WR_DATAX(0x2C); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x34); 
		LCD_WR_DATAX(0x02); 
		LCD_WR_REG(0xF7);  
		LCD_WR_DATAX(0x20); 
		LCD_WR_REG(0xEA);  
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATAX(0x1B);   //VRH[5:0] 
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATAX(0x01);   //SAP[2:0];BT[3:0] 
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATAX(0x30); 	 //3F
		LCD_WR_DATAX(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATAX(0XB7); 
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATAX(0x48); 
		LCD_WR_REG(0x3A);   
		LCD_WR_DATAX(0x55); 
		LCD_WR_REG(0xB1);   
		LCD_WR_DATAX(0x00);   
		LCD_WR_DATAX(0x1A); 
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATAX(0x0A); 
		LCD_WR_DATAX(0xA2); 
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATAX(0x00); 
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATAX(0x01); 
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x2A); 
		LCD_WR_DATAX(0x28); 
		LCD_WR_DATAX(0x08); 
		LCD_WR_DATAX(0x0E); 
		LCD_WR_DATAX(0x08); 
		LCD_WR_DATAX(0x54); 
		LCD_WR_DATAX(0XA9); 
		LCD_WR_DATAX(0x43); 
		LCD_WR_DATAX(0x0A); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x00); 		 
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATAX(0x00); 
		LCD_WR_DATAX(0x15); 
		LCD_WR_DATAX(0x17); 
		LCD_WR_DATAX(0x07); 
		LCD_WR_DATAX(0x11); 
		LCD_WR_DATAX(0x06); 
		LCD_WR_DATAX(0x2B); 
		LCD_WR_DATAX(0x56); 
		LCD_WR_DATAX(0x3C); 
		LCD_WR_DATAX(0x05); 
		LCD_WR_DATAX(0x10); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_DATAX(0x3F); 
		LCD_WR_DATAX(0x3F); 
		LCD_WR_DATAX(0x0F); 
		LCD_WR_REG(0x2B); 
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x01);
		LCD_WR_DATAX(0x3f);
		LCD_WR_REG(0x2A); 
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0x00);
		LCD_WR_DATAX(0xef);	 
		LCD_WR_REG(0x11); //Exit Sleep
		HAL_Delay(120);
		LCD_WR_REG(0x29); //display on	
	}

	LCD_Display_Dir(0);		 	//Ĭ��Ϊ����
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10, GPIO_PIN_SET);					//��������
	LCD_Clear(WHITE);
}  		  
  
//��������
//color:Ҫ���������ɫ
void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//�õ��ܵ���
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804������ʱ�����⴦��  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_SetCursor(0x00,0x0000);		//���ù��λ��  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else LCD_SetCursor(0x00,0x0000);	//���ù��λ�� 
	LCD_WriteRAM_Prepare();     		//��ʼд��GRAM	  	  
	for(index=0;index<totalpoint;index++)LCD_WR_DATA(color);	
}  
//��ָ�����������ָ����ɫ
//�����С:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:Ҫ������ɫ
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	uint16_t temp;
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	//6804������ʱ�����⴦��  
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;	  
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		LCD_Fill(sx,sy,ex,ey,color);  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				//���ù��λ�� 
			LCD_WriteRAM_Prepare();     			//��ʼд��GRAM	  
			for(j=0;j<xlen;j++)LCD_WR_DATA(color);	//���ù��λ�� 	    
		}
	}
}  
//��ָ�����������ָ����ɫ��			 
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
//color:Ҫ������ɫ
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//�õ����Ŀ��
	height=ey-sy+1;			//�߶�
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	//���ù��λ�� 
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM
		for(j=0;j<width;j++)LCD_WR_DATA(color[i*width+j]);//д������ 
	}	  
} 
//����
//x1,y1:�������
//x2,y2:�յ�����  
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow,uCol);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//������	  
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0           
		LCD_DrawPoint(x0+b,y0+a);             //4               
		LCD_DrawPoint(x0+a,y0+b);             //6 
		LCD_DrawPoint(x0-a,y0+b);             //1       
 		LCD_DrawPoint(x0-b,y0+a);             
		LCD_DrawPoint(x0-a,y0-b);             //2             
  		LCD_DrawPoint(x0-b,y0-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:�����С 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  							  
    uint8_t temp,t1,t;
	uint16_t y0=y;
	uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[num][t];	//����1608����
		else if(size==24)temp=asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//��������
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//m^n����
//����ֵ:m^n�η�.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}			 
//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//��ʾ����,��λΪ0,������ʾ
//x,y:�������
//num:��ֵ(0~999999999);	 
//len:����(��Ҫ��ʾ��λ��)
//size:�����С
//mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}
