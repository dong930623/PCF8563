#ifndef __PCF8563_H
#define	__PCF8563_H

#include "stm32f10x.h"
//Mini STM32������
//IIC ��������
//����ԭ��@ALIENTEK
//2010/6/10 

#define IIC_SCL_PORT              GPIOB
#define IIC_SCL_CLK               RCC_APB2Periph_GPIOB 
#define IIC_SCL_PIN               GPIO_Pin_6

#define IIC_SDA_PORT              GPIOB
#define IIC_SDA_CLK               RCC_APB2Periph_GPIOB  
#define IIC_SDA_PIN               GPIO_Pin_7

extern GPIO_InitTypeDef  GPIO_InitStructure;   	   		   
//IO��������
#define SDA_IN()  {                                           \
									 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  \
									 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; \
                   GPIO_Init(GPIOB, &GPIO_InitStructure);	\
									}
#define SDA_OUT() {                                          \
									 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; \
									 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		  \
									 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	\
                   GPIO_Init(GPIOB, &GPIO_InitStructure);	\
									}


//IO��������	 
//#define IIC_SCL    PCout(12) //SCL
#define Set_IIC_SCL  {GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN);}
#define Clr_IIC_SCL  {GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN);} 
//#define IIC_SDA    PCout(11) //SDA
#define Set_IIC_SDA  {GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);}
#define Clr_IIC_SDA  {GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN);} 
//#define READ_SDA   PCin(11)  //����SDA 
#define READ_SDA    (GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN))

	 


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 
void PCF8563_Init_Register (void);
void PCF8563_rest (void);
int PCF8563_Gettime (void);

#endif

