#include "pwm.h"
#include "led.h"
#include "delay.h" 
#include "timer.h"
#include "pid.h"
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
int dis;
float pitch,roll,yaw,x_gyro,y_gyro,z_gyro;    //�����Ǵ����������Ƕ� �Լ��������ٶ�
u8 yaw_switch=0;
WALK_STATUS walk_status=NORMAL;
//PWM���� 200ms -> 20000
void TIM3_PWM_Init(void)
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
    /* ��ʱ��3���� */                                            
    TIM_TimeBaseStructure.TIM_Period =9999; //��ֵ �������ֵ  //50hz  10000
    TIM_TimeBaseStructure.TIM_Prescaler = 71;   //��Ƶֵ       //50hz                          
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                              
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//����ģʽΪ���ϼ���               
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
    /* PWM1 ���� */  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//pwm1ģʽ                       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = 0;                                  
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High; 
	
		TIM_OC1Init(TIM3, &TIM_OCInitStructure); //A6
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���
	
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); //A7
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ��� 
		
		TIM_OC3Init(TIM3, &TIM_OCInitStructure); //B0
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��TIM3��CCR3�ϵ�Ԥװ�ؼĴ��� 
		
		TIM_OC4Init(TIM3, &TIM_OCInitStructure); //B1
		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); //ʹ��TIM3��CCR4�ϵ�Ԥװ�ؼĴ��� 
		
		/*   �ж�����   */
			/* TIM3 enable counter */  
		TIM_Cmd(TIM3, ENABLE);                                           
}

void TIM4_PWM_Init(void)
{  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  
    /* ��ʱ��4���� */                                            
    TIM_TimeBaseStructure.TIM_Period =9999; //��ֵ �������ֵ  //50hz  10000
    TIM_TimeBaseStructure.TIM_Prescaler = 71;   //��Ƶֵ       //50hz                          
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                              
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//����ģʽΪ���ϼ���               
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  
    /* PWM1 ���� */  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//pwm1ģʽ                       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
    TIM_OCInitStructure.TIM_Pulse = 0;                                  
    TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High; 
	
		TIM_OC1Init(TIM4, &TIM_OCInitStructure); //B6
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4��CCR1�ϵ�Ԥװ�ؼĴ���
	
    TIM_OC2Init(TIM4, &TIM_OCInitStructure); //B7
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ��� 
		
		TIM_OC3Init(TIM4, &TIM_OCInitStructure); //B8
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4��CCR3�ϵ�Ԥװ�ؼĴ��� 
		
		TIM_OC4Init(TIM4, &TIM_OCInitStructure); //B9
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); //ʹ��TIM4��CCR4�ϵ�Ԥװ�ؼĴ��� 
		
			/* TIM4 enable counter */  
		TIM_Cmd(TIM4, ENABLE);                                           
}


//������ ǰ��     С���� ����
void set_motor_forward_back_speed(int lspeed,int rspeed)
{
	if(lspeed>=0)
	{					
		TIM3->CCR1 =lspeed;//TIM3  12
	  TIM3->CCR2 =0;
		TIM4->CCR1 =lspeed;
	  TIM4->CCR2 =0;
	}
	else
	{
	  TIM3->CCR1 = 0;
	  TIM3->CCR2 = -lspeed;
		TIM4->CCR1 = 0;
	  TIM4->CCR2 = -lspeed;
	}
	
	if(rspeed>=0)
	{					
		TIM3->CCR3 =rspeed;
	  TIM3->CCR4 =0;
		TIM4->CCR3 =rspeed;
	  TIM4->CCR4 =0;
	}
	else
	{
	  TIM3->CCR3 = 0;
	  TIM3->CCR4 = -rspeed;
		TIM4->CCR3 = 0;	
	  TIM4->CCR4 = -rspeed;
	}
}

void set_speed(int speed1,int speed2,int speed3,int speed4)
{
   if(speed1>0)
	 {
		TIM4->CCR1 =speed1;					//��ǰ +
	  TIM4->CCR2 =0;
	 }
	 else
	 {
		TIM4->CCR1 =0;					
	  TIM4->CCR2 =-speed1;
	 }
	 
	    if(speed2>0)
	 {
		TIM4->CCR3 =speed2;					//��ǰ +
	  TIM4->CCR4 =0;
	 }
	 else
	 {
		TIM4->CCR3 =0;				
	  TIM4->CCR4 =-speed2;
	 }
	 
	 	    if(speed3>0)
	 {
		TIM3->CCR1 =speed3;					//��� +
	  TIM3->CCR2 =0;
	 }
	 else
	 {
		TIM3->CCR1 =0;					
	  TIM3->CCR2 =-speed3;
	 }
	 
	 	    if(speed4>0)
	 {
		TIM3->CCR3 =speed4;					//�Һ� +
	  TIM3->CCR4 =0;
	 }
	 else
	 {
		TIM3->CCR3 =0;					
	  TIM3->CCR4 =-speed4;
	 }
		 
		

}

void choose_fbspeed(int fspeed,int bspeed)
{
		if(fspeed>=0)
	{					
		TIM3->CCR1 =fspeed;
	  TIM3->CCR2 =0;
		TIM3->CCR3 =fspeed;
	  TIM3->CCR4 =0;
	}
	else
	{
		TIM3->CCR1 =0;
	  TIM3->CCR2 =-fspeed;
		TIM3->CCR3 =0;
	  TIM3->CCR4 =-fspeed;
	}
	
	if(bspeed>=0)
	{					
		TIM4->CCR1 =fspeed;
	  TIM4->CCR2 =0;
		TIM4->CCR3 =fspeed;
	  TIM4->CCR4 =0;
	}
	else
	{
		TIM4->CCR1 =0;
	  TIM4->CCR2 =-fspeed;
		TIM4->CCR3 =0;
	  TIM4->CCR4 =-fspeed;
	}

}

//��ת: ������0
//��ת: ��С��0
void set_motor_left_right_speed(int lspeed,int rspeed)
{
	if(lspeed>=0)
	{					
		TIM3->CCR1 =lspeed;
	  TIM3->CCR2 =0;
		TIM4->CCR1 =0;
	  TIM4->CCR2 =lspeed;
	}
	else
	{
	  TIM3->CCR1 = 0;
	  TIM3->CCR2 = -lspeed;
		TIM4->CCR1 = -lspeed;
	  TIM4->CCR2 = 0;
	}
	
	if(rspeed>=0)
	{					
		TIM3->CCR3 =0;
	  TIM3->CCR4 =rspeed;
		TIM4->CCR3 =rspeed;
	  TIM4->CCR4 =0;
	}
	else
	{
	  TIM3->CCR3 = -rspeed;
	  TIM3->CCR4 = 0 ;
		TIM4->CCR3 = 0;
	  TIM4->CCR4 = -rspeed ;
	}
}

//ѭ�����Ƴ���
//void TRAC_Control(void)
//{
//	
//	if(LEFT_LITTLE_TURN)  //��Сת
//	{
//		yaw_switch=0;  //���yaw�����仯 ��Ҫ���¶����ֵ
//		set_motor_left_right_speed(-LITTLE_TURN_SPEED,-LITTLE_TURN_SPEED);
//	}
//	else if(LEFT_BIGGER_TURN)  //���ת
//	{
//		yaw_switch=0;
//		set_motor_left_right_speed(-BIGGER_TURN_SPEED,-BIGGER_TURN_SPEED);
//	}
//	else if(RIGHT_LITTLE_TURN)  //��Сת
//	{
//		yaw_switch=0;
//		set_motor_left_right_speed(LITTLE_TURN_SPEED,LITTLE_TURN_SPEED);
//	}
//	else if(RIGHT_BIGGER_TURN)  //�Ҵ�ת
//	{
//		yaw_switch=0;
//		set_motor_left_right_speed(BIGGER_TURN_SPEED,BIGGER_TURN_SPEED);
//	}
//	else if(STOP)
//	{
//		yaw_switch=0;
//		set_motor_left_right_speed(0,0);
//		set_motor_forward_back_speed(0,0);
//	}
//	else if(FORWARD)             //ǰ��
//	{
//		set_motor_forward_back_speed(FORWARD_SPEED,FORWARD_SPEED);
//	}
//	else   //��������� ����ǰ��
//	{
//		yaw_switch=0;
//		if(__fabs(pitch)<5)
//		{
//			set_motor_forward_back_speed(SLOW_SPEED,SLOW_SPEED);
//		}
//	}
//}




