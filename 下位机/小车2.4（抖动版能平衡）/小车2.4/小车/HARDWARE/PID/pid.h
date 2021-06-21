#ifndef __PID_H
#define __PID_H

#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\

typedef enum
{
	NONE=0,
	FORWARD_BACK_PID_CONTROL=1,
	LEFT_RIGHT_PID_CONTROL=2,
	FORWARD_BACK_LEFT_RIGHT_PID_CONTROL=3,
}PID_CONTROL_MODE;
//ǰ�� ���� ����PID���Ʊ�־λ
#define FORWARD_BACK_PID_CONTROL     1
#define LEFT_RIGHT_PID_CONTROL       0

typedef struct PID_PARAMETER
{
	int Expect_Value;                    //�趨��
	int Actual_Value;                    //������
	float error_now;             
	float error_last;             
	float error_inter;             
	float Kp,Ki,Kd;            
	int pid_out;               
}PID;
typedef struct PID_INCREASE
{
	float current;     //��ǰ ���ڷ���ʵ�����ֵ
	float expect;      //����ֵ
	float Kp,Ki,Kd;
	float error_now;
	float error_next;
	float error_last;
	float increament;
}PID_ADD;   //����ʽPID

extern PID CrossSeesaw_Pitch_Position,CrossSeesaw_Pitch_Speed;
extern PID CrossSeesaw_Yaw_Position,CrossSeesaw_Yaw_Speed;
extern PID_CONTROL_MODE pid_control_mode;
extern PID Openmv_Position,Openmv_theta;
extern PID_ADD Openmv_Increment;
void PID_Control(float current, float expected,PID* motor_type);
void PID_Init(void);
float PID_Increment(float current,float expect,PID_ADD* motor);//���ʻ����Ƶ�PID����  ����ʽPID
void PID_Control_Status_Check(void);
#endif

