#include "motorcontrol.h"

void Set_CommandSpeed(struct MOTORCONTROL *motor, double cmdspeed)
{
	if(cmdspeed > motor->MaxSpeed) motor->CommandSpeed = motor->MaxSpeed;
	else if(cmdspeed < -(motor->MaxSpeed))motor->CommandSpeed = -(motor->MaxSpeed);
	else motor->CommandSpeed = cmdspeed;
}

void Update_Motor_Speed(struct MOTORCONTROL *motor, struct ENCODER *encoder)
{
	motor->EncoderReadings[1] = motor->EncoderReadings[0]; 
	motor->EncoderReadings[0] = Encoder_Read(encoder);
	
	double s = 0;
	for(int i = SpeedsN-1; i > 0; i--){
		motor->Speeds[i] = motor->Speeds[i-1];
		s += motor->Speeds[i]/SpeedsN;
	}
	int delta = (int) (encoder->Direction)*(motor->EncoderReadings[0] - motor->EncoderReadings[1]);
	// for 16 bit timer --------------------------
	if(delta > 32768)	delta = delta - 65536;
	else if(delta < -32768) delta = 65536 + delta;
	//--------------------------------------------
	motor->Speeds[0] = delta*2*PI*(encoder->Radius)*(motor->UpdateFrequency)/(encoder->Resolution)/(encoder->ReductionRatio);
	motor->AverageSpeed = s + motor->Speeds[0]/SpeedsN;
}

void Update_Speed_Error(struct MOTORCONTROL *motor)
{
	volatile double avg_error = 0;
	for(int i = ErrorsN-1; i > 0; i--){
		motor->Errors[i] = motor->Errors[i-1];
		avg_error += motor->Errors[i]/ErrorsN;
	}
	motor->Errors[0] = (motor->CommandSpeed - motor->Speeds[0])*((motor->MaxDuty)/(motor->MaxSpeed));
	avg_error += motor->Errors[0]/ErrorsN;
	
	motor->IntegratedError = motor->IntegratedError + (motor->Errors[0])/(motor->UpdateFrequency);
	
	// calculate derivative using linear regression
	volatile double a = 0, b = 0;
	for(int i = 0; i < ErrorsN; i++){
		a += ((i - (ErrorsN-1)/2)/(motor->UpdateFrequency)) * (motor->Errors[i] - avg_error);
		b += ((i - (ErrorsN-1)/2)/(motor->UpdateFrequency))^2;
	}
	motor->DerivativedError = a / b;
}

void Update_Motor_PWM(struct MOTORCONTROL *motor, struct MONSTERMOTOR *monster)
{
	motor->Duty = (motor->Pgain)*(motor->Errors[0]) + 
								(motor->Igain)*(motor->IntegratedError) + 
								(motor->Dgain)*(motor->DerivativedError);
	
	if(motor->Duty > motor->MaxDuty) motor->Duty = motor->MaxDuty;
	else if(motor->Duty < -(motor->MaxDuty)) motor->Duty = -(motor->MaxDuty);
	MonsterMotor_Write(monster, motor->Duty);
}

void motor_speed_handler(struct MOTORCONTROL *motor, struct ENCODER *encoder, struct MONSTERMOTOR *monster)
{
	Update_Motor_Speed(motor, encoder);
	Update_Speed_Error(motor);
	Update_Motor_PWM(motor, monster);
	//MonsterMotor_Write(monster, 100);								// for testing
}

void MotorControl_Init(struct MOTORCONTROL *motor)
{
	motor->UpdateFrequency = 1000;
	motor->CommandSpeed = 0;
	motor->Duty = 0;
	motor->EncoderReadings[0] = 32767;
	motor->EncoderReadings[1] = 32767;
	motor->MaxDuty = 100;
	motor->MaxSpeed = 2000;
	motor->IntegratedError = 0;
	motor->DerivativedError = 0;
	motor->Pgain = 5;
	motor->Igain = 1000;
	motor->Dgain = 5;
	
	for(int i=0; i<10; i++){
		motor->Errors[i] = 0;
		motor->Errors[i] = 0;
	}
}
