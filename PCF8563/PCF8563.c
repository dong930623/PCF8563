/***************************************************************************
** ��Ȩ����:  FudaBD Copyright (c) 2013-2017  ********************             
** �ļ�����:  PCF8563.c
** ����ժҪ:  PCF8563 ��д����
** ��ǰ�汾:  v1.0
** ��    ��:  Donny
** ��������: 2017��10��12��
** �޸ļ�¼: 
** �޸�����: 
** �汾��  : 
** �޸���  : 
** �޸�����: 
***************************************************************************/

#include "PCF8563.h"
#include "delay.h" 

void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
						 
	//GPIOC->CRH&=0XFFF00FFF;//PC11/12 �������
	//GPIOC->CRH|=0X00033000;	   
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure); 
	Set_IIC_SCL;
	Set_IIC_SDA;
}
//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	Clr_IIC_SCL;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	Set_IIC_SDA;delay_us(1);	   
	Set_IIC_SCL;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	Clr_IIC_SCL;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Clr_IIC_SDA;
	delay_us(2);
	Set_IIC_SCL;
	delay_us(2);
	Clr_IIC_SCL;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Set_IIC_SDA;
	delay_us(2);
	Set_IIC_SCL;
	delay_us(2);
	Clr_IIC_SCL;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    Clr_IIC_SCL;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
    txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		Set_IIC_SCL;
		delay_us(2); 
		Clr_IIC_SCL;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
		Clr_IIC_SCL; 
		delay_us(2);
		Set_IIC_SCL;
		receive<<=1;
		if(READ_SDA)receive++;   
		delay_us(1); 
	}					 
	if (!ack)
		IIC_NAck();//����nACK
	else
		IIC_Ack(); //����ACK   
	return receive;
}
//��PCF5368ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 PCF5368_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();   
	IIC_Send_Byte(0XA2);	   //����д����
	IIC_Wait_Ack();   
  IIC_Send_Byte(ReadAddr);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA3);           //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��PCF5368ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void PCF5368_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();   
	IIC_Send_Byte(0XA2);	    //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);//���͸ߵ�ַ
	IIC_Wait_Ack();		   										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
} 
/***************************************************************************//**
 * @brief
 *  ���Ź���������ʼ��
 *
 ******************************************************************************/
void PCF8563_Init_Register (void)
{
	IIC_Init();	 
	//ֹͣ��ʱ������  
	PCF5368_WriteOneByte (0x00, 0x20); 
	//��ʱ�����������ж�
	PCF5368_WriteOneByte (0x01, 0x11); 
	//������ʱ�� 60����
	PCF5368_WriteOneByte ( 0x0E, 0x83); 
	PCF5368_WriteOneByte ( 0x0F, 60); 
	PCF5368_WriteOneByte ( 0x00, 0x00);  	
}
/***************************************************************************//**
 * @brief
 *    ��ȡ����ʱ��ʱ��
 *
 ******************************************************************************/
int PCF8563_Gettime (void)
{    
	return PCF5368_ReadOneByte(0x0F) & 0xff;		 
}
/***************************************************************************//**
 * @brief
 *    ��λ����ʱ��ʱ��
 *
 ******************************************************************************/
void PCF8563_rest (void)
{   
	//ֹͣ��ʱ������  
	PCF5368_WriteOneByte (0x00, 0x20); 
	//��ʱ�����������ж�
	PCF5368_WriteOneByte (0x01, 0x11); 
	//��ʱ�������жϿ��ƺ�Ƶ�ȿ���  1 0 ��   11  ��
	PCF5368_WriteOneByte (0x0E, 0x83); 
	PCF5368_WriteOneByte (0x0F, 0xFF); 
	PCF5368_WriteOneByte (0x00, 0x00); 		 
}