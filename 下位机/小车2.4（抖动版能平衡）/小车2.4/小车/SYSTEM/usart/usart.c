#include "sys.h"
#include "usart.h"	  
#include "string.h"
#include "timer.h"
#include "led.h"
#include "stm32f10x_dma.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{   
//  USART_SendData(USART1,(u8)ch);
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
//	return ch;
//	
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 openmv_buff[USART_REC_LEN];
float mv_distance=0,mv_theta=0;
char timecount_flag[2];

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart1_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	//DMA1ͨ��5����
//	DMA_DeInit(DMA1_Channel5);
//	//�����ַ
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
//	//�ڴ��ַ
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)openmv_buff;
//	//dma���䷽����
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	//����DMA�ڴ���ʱ�������ĳ���
//	DMA_InitStructure.DMA_BufferSize = USART_REC_LEN;
//	//����DMA���������ģʽ��һ������
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	//����DMA���ڴ����ģʽ
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	//���������ֳ�
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	//�ڴ������ֳ�
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	//����DMA�Ĵ���ģʽ
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	//����DMA�����ȼ���
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	//����DMA��2��memory�еı����������
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel5,&DMA_InitStructure);
// 
//	//ʹ��ͨ��5
//	DMA_Cmd(DMA1_Channel5,ENABLE);

	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
 //�ж�����
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	//����DMA��ʽ����
//	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void uart2_init(u32 bound){
  //GPIO�˿�����

	GPIO_InitTypeDef GPIO_InitStrue;
	USART_InitTypeDef USART_InitStrue;
	NVIC_InitTypeDef NVIC_InitStrue;
	
	// ����ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);  //��λ����2 -> ����û��
	
	// ��ʼ�� ���ڶ�ӦIO��  TX-PA2  RX-PA3
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStrue.GPIO_Pin=GPIO_Pin_3;
  GPIO_Init(GPIOA,&GPIO_InitStrue);
	
	// ��ʼ�� ����ģʽ״̬
	USART_InitStrue.USART_BaudRate=bound; // ������
	USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None; // Ӳ��������
	USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx; // ���� ���� ģʽ��ʹ��
	USART_InitStrue.USART_Parity=USART_Parity_No; // û����żУ��
	USART_InitStrue.USART_StopBits=USART_StopBits_1; // һλֹͣλ
	USART_InitStrue.USART_WordLength=USART_WordLength_8b; // ÿ�η������ݿ��Ϊ8λ
	USART_Init(USART2,&USART_InitStrue);
	
	USART_Cmd(USART2,ENABLE);//ʹ�ܴ���
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//���������ж�
	
	// ��ʼ�� �ж����ȼ�
	NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStrue);

}

int i = 0;
void USART1_IRQHandler(void)                	//����1�жϷ������
{
		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
				mv_distance=USART_ReceiveData(USART1);  // ����2 ����
			
			USART_ClearITPendingBit(USART1,USART_IT_RXNE );

		}
//	u8 Res;
//	uint32_t temp = 0;
//	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//	{
////		Res= USART_ReceiveData(USART1);  // ����2 ����
////		openmv_buff[i]=Res;
////		i++;
////		if(i==4)
////			i=0;
//		DMA_ClearFlag( DMA1_FLAG_TC5 );  
//		temp = USART1->SR;
//		temp = USART1->DR;                  //��USART_IT_IDLE��־
//		DMA_Cmd(DMA1_Channel5,DISABLE);
//		if(openmv_buff[0]==0xAA&&openmv_buff[3]==0x55)
//		{
//			mv_distance = (int16_t)openmv_buff[1];
//			mv_theta = (int16_t)openmv_buff[2];
//		}
//		DMA_SetCurrDataCounter(DMA1_Channel5,USART_REC_LEN);
//		//��DMA
//		DMA_Cmd(DMA1_Channel5,ENABLE);
//		//USART_ClearITPendingBit(USART1,USART_IT_RXNE );
//  } 
}


void USART2_IRQHandler(void) // ����2�жϷ�����
{
	char res=' ';
	if(USART_GetITStatus(USART2,USART_IT_RXNE)) // �жϱ�־
	{
		res= USART_ReceiveData(USART2);  // ����2 ����
		if(res=='B')  //�ֻ���������ָ��
		{
			USART_SendData(USART2,'B');   // ����2 ���͸��ֻ� �ֻ���ʼ��ʱ
			led=0;
			Init_Flag=1;
			res=' ';
		}
		else if(res=='S')
		{
			USART_SendData(USART2,'S');   // ����2 ���͸��ֻ� �ֻ�ֹͣ��ʱ
			time=0;
			led=1;
			Init_Flag=0;
			res=' ';
		}
	  USART_ClearITPendingBit(USART2,USART_IT_RXNE );
	}
}

#endif	
