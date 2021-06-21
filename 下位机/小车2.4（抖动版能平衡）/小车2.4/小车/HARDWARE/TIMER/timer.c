#include "timer.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
#include "pid.h"
#include "pwm.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
u32 time=0;
u32 js_tim2;
u8 Init_Flag=0;
u32 pitch_count=0;
float pitch_offset=0;
float yaw_offset=0;
extern float mv_distance,mv_theta;
#define FOWORD_SPEED  4000
int foword_speed=0;
int seesaw_flag=0;
u8 ban_flag=0;
int ban_time=0;
int run_flag=0,stop_time=0,fb_flag=1;
int speed_offset=0;
float y_threshold=0;
int max_speed=0;
float seesaw_threshold=0;

#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\


void TIM2_Int_Init(u16 arr,u16 psc)   //�Զ���װ�غ�Ԥ��Ƶϵ��
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);   //ʹ�ܶ�ʱ��2
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;            //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;         //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);         //ʹ�ܶ�ʱ��2�ĸ����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;   //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;          //��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2,ENABLE);                               //ʹ�ܶ�ʱ��2
}

void ShakeSet(int count_time,int period,int run_time,float pitch_threshold,int lock_speed)
{
	if(count_time%period>run_time||(pitch>-pitch_threshold&&pitch<pitch_threshold))  //10.2�� 	400 100 	9.5  			/500 80 4550���
		{
			if(pitch>-7.5&&pitch<7.5)set_motor_forward_back_speed(0,0);	
			else set_motor_forward_back_speed(lock_speed,lock_speed);		  
			stop_time++;
			if(stop_time>205)
			seesaw_flag=0;
		}
		else if(count_time%period<=run_time&&!(pitch>-pitch_threshold&&pitch<pitch_threshold))
		{
			if(pitch>5)	set_speed(foword_speed,0,0,foword_speed);
			else if(pitch<-5)set_speed(foword_speed,0,0,foword_speed);
			stop_time=0;
		}
		else
			set_motor_forward_back_speed(lock_speed,lock_speed);	
}

void TIM2_IRQHandler(void)   //TIM2�ж�
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		  TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
			js_tim2++;
		
		if(pitch>5)fb_flag=0;		
		else if(pitch<-5)fb_flag=1;
		
		if(seesaw_flag)ban_time++;//�����ΰ���
      if(Init_Flag)
			{
				time++;//������ʱ

				if(js_tim2%50==0)//���ΰ��Լ�Ѱ������
				{
	
					if(seesaw_flag==0)
					{
							foword_speed=4100;
							PID_Control(mv_distance,44,&Openmv_Position);//����OPENMV�����߼��  λ��ʽ
							VAL_LIMIT(Openmv_Position.pid_out,-foword_speed,foword_speed);

							if(mv_distance<14)
									set_motor_forward_back_speed(3000,-3000);	
							else
								set_motor_forward_back_speed(foword_speed-Openmv_Position.pid_out,foword_speed+Openmv_Position.pid_out);	//λ��ʽ
					}
					else if(seesaw_flag)
					{
							PID_Control(pitch,pitch_offset,&CrossSeesaw_Pitch_Position);//����OPENMV�����߼��  λ��ʽ
							PID_Control(y_gyro,CrossSeesaw_Pitch_Position.pid_out,&CrossSeesaw_Pitch_Speed);//����OPENMV�����߼��  λ��ʽ
							foword_speed=CrossSeesaw_Pitch_Speed.pid_out;
							VAL_LIMIT(foword_speed,-6600,6600);
							/****************************/
							PID_Control(mv_distance,32,&Openmv_Position);//����OPENMV�����߼��  λ��ʽ
							VAL_LIMIT(Openmv_Position.pid_out,-4200,4200);

						if(ban_time>525)
						{
							y_threshold=9;
							max_speed=10000;
							seesaw_threshold=9.97;
							VAL_LIMIT(pitch,-10,10);
								if(fb_flag)//��ǰ
								{
									if(y_gyro<y_threshold&&y_gyro>-y_threshold)
									{
									  ShakeSet(js_tim2,400,180,seesaw_threshold,1200);
									}
//									else if(y_gyro>=y_threshold)
//									{
//											speed_offset=y_gyro*max_speed;
//										  VAL_LIMIT(speed_offset,-4100,4100);
//											set_speed(-speed_offset,0,0,-speed_offset);
//									}
//									else
//									{
//											speed_offset=y_gyro*max_speed;
//										  VAL_LIMIT(speed_offset,-4100,4100);
//											set_speed(speed_offset,0,0,speed_offset);
//									}
									else
									{
											speed_offset=y_gyro*max_speed;
										  VAL_LIMIT(speed_offset,-4200,4200);
										
										  set_speed(-speed_offset,0,0,-speed_offset);
									}
										
								}
								else
								{
									if(y_gyro<y_threshold&&y_gyro>-y_threshold)
									{
										ShakeSet(js_tim2,400,180,seesaw_threshold,-1200);
									}									
//									else if(y_gyro>=y_threshold)
//									{
//											speed_offset=y_gyro*max_speed;
//										  VAL_LIMIT(speed_offset,-4100,4100);
//											set_speed(-speed_offset,0,0,-speed_offset);
//									}
//									else
//									{
//											speed_offset=y_gyro*max_speed;
//										  VAL_LIMIT(speed_offset,-4100,4100);
//											set_speed(speed_offset,0,0,speed_offset);
//									}
									else
									{
											speed_offset=y_gyro*max_speed;
										  VAL_LIMIT(speed_offset,-4100,4100);
										
										  set_speed(-speed_offset,0,0,-speed_offset);
									}
								}						
						}
						else
							set_motor_forward_back_speed(foword_speed-Openmv_Position.pid_out,foword_speed+Openmv_Position.pid_out);	//λ��ʽ		
					}
				}
	}
			else
     		set_motor_forward_back_speed(0,0);	
			
	}
}


