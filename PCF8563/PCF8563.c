/***************************************************************************
** 版权所有:  FudaBD Copyright (c) 2013-2017  ********************             
** 文件名称:  PCF8563.c
** 内容摘要:  PCF8563 读写函数
** 当前版本:  v1.0
** 作    者:  Donny
** 创建日期: 2017年10月12日
** 修改记录: 
** 修改日期: 
** 版本号  : 
** 修改人  : 
** 修改内容: 
***************************************************************************/

#include "PCF8563.h"
#include "delay.h" 

void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
						 
	//GPIOC->CRH&=0XFFF00FFF;//PC11/12 推挽输出
	//GPIOC->CRH|=0X00033000;	   
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure); 
	Set_IIC_SCL;
	Set_IIC_SDA;
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	Clr_IIC_SCL;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
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
	Clr_IIC_SCL;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
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
//不产生ACK应答		    
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
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    Clr_IIC_SCL;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
    txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		Set_IIC_SCL;
		delay_us(2); 
		Clr_IIC_SCL;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
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
		IIC_NAck();//发送nACK
	else
		IIC_Ack(); //发送ACK   
	return receive;
}
//在PCF5368指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 PCF5368_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
	IIC_Start();   
	IIC_Send_Byte(0XA2);	   //发送写命令
	IIC_Wait_Ack();   
  IIC_Send_Byte(ReadAddr);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA3);           //进入接收模式			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在PCF5368指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void PCF5368_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();   
	IIC_Send_Byte(0XA2);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);//发送高地址
	IIC_Wait_Ack();		   										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
} 
/***************************************************************************//**
 * @brief
 *  看门狗传感器初始化
 *
 ******************************************************************************/
void PCF8563_Init_Register (void)
{
	IIC_Init();	 
	//停止定时器计数  
	PCF5368_WriteOneByte (0x00, 0x20); 
	//定时器产生脉冲中断
	PCF5368_WriteOneByte (0x01, 0x11); 
	//倒数定时器 60分钟
	PCF5368_WriteOneByte ( 0x0E, 0x83); 
	PCF5368_WriteOneByte ( 0x0F, 60); 
	PCF5368_WriteOneByte ( 0x00, 0x00);  	
}
/***************************************************************************//**
 * @brief
 *    获取倒计时的时间
 *
 ******************************************************************************/
int PCF8563_Gettime (void)
{    
	return PCF5368_ReadOneByte(0x0F) & 0xff;		 
}
/***************************************************************************//**
 * @brief
 *    复位倒计时的时间
 *
 ******************************************************************************/
void PCF8563_rest (void)
{   
	//停止定时器计数  
	PCF5368_WriteOneByte (0x00, 0x20); 
	//定时器产生脉冲中断
	PCF5368_WriteOneByte (0x01, 0x11); 
	//定时器产生中断控制和频度控制  1 0 秒   11  分
	PCF5368_WriteOneByte (0x0E, 0x83); 
	PCF5368_WriteOneByte (0x0F, 0xFF); 
	PCF5368_WriteOneByte (0x00, 0x00); 		 
}