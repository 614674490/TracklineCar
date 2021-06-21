#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define BLACK 1      //�·��Ǻ�ɫ����
#define WHITE 0      //�·��ǵذ�

#define FORWARD             LEFT==WHITE&&MIDDLE==BLACK&&RIGHT==WHITE||(LEFT==WHITE&&MIDDLE==WHITE&&RIGHT==WHITE)     //ǰ��
#define LEFT_BIGGER_TURN    LEFT==BLACK&&MIDDLE==BLACK&&RIGHT==WHITE      //��Сת
#define LEFT_LITTLE_TURN    LEFT==BLACK&&MIDDLE==WHITE&&RIGHT==WHITE      //���ת
#define RIGHT_BIGGER_TURN   LEFT==WHITE&&MIDDLE==BLACK&&RIGHT==BLACK      //��Сת
#define RIGHT_LITTLE_TURN   LEFT==WHITE&&MIDDLE==WHITE&&RIGHT==BLACK      //�Ҵ�ת
#define STOP                LEFT==BLACK&&MIDDLE==BLACK&&RIGHT==BLACK      //ֹͣ

#define MAX_SPEED                 4000       //����ٶ�   10000       
#define SLOW_SPEED                1000       //����ٶ�   10000   
#define FORWARD_SPEED             2000       //ǰ���ٶ�
#define LITTLE_TURN_SPEED         6000       //Сת�ٶ�
#define BIGGER_TURN_SPEED         8000       //��ת�ٶ�

typedef enum
{
	READY=0,
	NORMAL=1,
	SPECIAL=2,
}WALK_STATUS;
void TIM3_PWM_Init(void);
void TIM4_PWM_Init(void);
void set_motor_forward_back_speed(int lspeed,int rspeed);
void set_motor_left_right_speed(int lspeed,int rspeed);
void choose_fbspeed(int fspeed,int bspeed);
void set_speed(int speed1,int speed2,int speed3,int speed4);
void TRAC_Control(void);
extern float pitch,roll,yaw,x_gyro,y_gyro,z_gyro;  
extern u8 yaw_switch;
extern WALK_STATUS walk_status;
#endif
