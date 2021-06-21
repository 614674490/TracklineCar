#include "led.h"
#include "delay.h"
#include "sys.h"
#include "pwm.h"
#include "usart.h"
#include "oled.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"   
#include "public.h"
#include "stm32f10x.h"
#include "math.h"
#include "timer.h"
#include "pid.h"
#include "timer.h"
//SCL A5����//SDA A4����//ADO A3����
//��·PWM���������TIM3���ĸ�ͨ�� ����A6A7Ϊ���PWM���B0B1Ϊʸ��������

u8 left_flag=0;
u8 right_flag=0;
u8 middle_flag=0;
unsigned char speed_ge,speed_shi,speed_bai;           //�ٶȸ�ʮ��
unsigned char distance_ge,distance_shi,distance_bai;  //·�̸�ʮ��  
int count=0;
int flag=0;
extern int seesaw_flag,stop_time;

int main(void)
{	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	LED_Init();
  WHEEL_GPIO_Init();
  MPU_GPIO_Init();
	TRAC_GPIO_Init();
	OLED_GPIO_Init();
	
	delay_init();
	uart1_init(115200);
	uart2_init(9600);    //��Ƭ�� �ֻ�ͨ��
	OLED_Init();
	OLED_P8x16Str(30,0,"Init...");  
	TIM3_PWM_Init();
	TIM4_PWM_Init();
	TIM2_Int_Init(9,7199); //����Ϊ1ms
	
	MPU_Init();
	PID_Init();
	

	while(mpu_dmp_init());
	delay_ms(1000);
	
	mpu_dmp_get_data(&pitch,&roll,&yaw,&x_gyro,&y_gyro,&z_gyro);
	pitch_offset=pitch;
	yaw_offset=yaw;
	yaw_switch=1;
	OLED_Display_Init();
	led=1;                //��־�����ʼ�����
	while(!Init_Flag){};  //�ȴ��ֻ���������ָ��
//  Init_Flag=1;
  while(1)
	{	
		if(mpu_dmp_get_data(&pitch,&roll,&yaw,&x_gyro,&y_gyro,&z_gyro)==0&&Init_Flag)	 //��ʼ������һ�ȡ������������
		{
			left_flag= PAin(0);
			right_flag= PAin(2);
			middle_flag= PAin(1);
			OLED_Display();			
			if((pitch>5||pitch<-5)&&stop_time<=205)   //�ж��Ƿ�����ΰ�
			{
				seesaw_flag=1;
			}
			if(RIGHT==BLACK&&LEFT==WHITE&&flag==0)
			{
				flag=1;
			}
			if(flag==1)
			{ 
				if(RIGHT==WHITE) 
					 {
						 flag=0;
						 Init_Flag=1;
					 }
				else if(RIGHT==BLACK&&LEFT==BLACK&&seesaw_flag==0)
					 {
						flag=2;
						Init_Flag=0;
						time=0;
						USART_SendData(USART2,'S');   // ����2 ���͸��ֻ� �ֻ�ֹͣ��ʱ
						led=1;
						set_motor_forward_back_speed(0,0);	
					 }			
			}				
		}		
	}
}
