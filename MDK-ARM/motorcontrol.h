#ifndef __MOTORCONTROL_H
#define __MOTORCONTROL_H

/* 	In speedfeedback.h & speedfeedback.c define:
		a simple velocity control system for a motor with
		internal encoder feedback & monster motor shield				*/

#include "hardware.h"

#define SpeedsN 11
#define ErrorsN 11

#define PI 3.14159265359

struct MOTORCONTROL
{
	volatile double 				CommandSpeed;
	volatile unsigned int 	EncoderReadings[2];
	volatile double 				Speeds[SpeedsN];
	volatile double 				AverageSpeed;
	volatile double 				Errors[ErrorsN];
	volatile double 				IntegratedError;
	volatile double					DerivativedError;
	volatile double 				Duty;
	int 										UpdateFrequency;
	double									MaxSpeed;
	double									MaxDuty;
	double									Pgain;
	double									Igain;
	double									Dgain;
};

void MotorControl_Init(struct MOTORCONTROL *motor);

void Set_CommandSpeed(struct MOTORCONTROL *motor, double cmdspeed);
	
void Update_Motor_Speed(struct MOTORCONTROL *motor, struct ENCODER *encoder);

void Update_Speed_Error(struct MOTORCONTROL *motor);
	
void Update_Motor_PWM(struct MOTORCONTROL *motor, struct MONSTERMOTOR *monster);
	
void motor_speed_handler(struct MOTORCONTROL *motor, struct ENCODER *encoder, struct MONSTERMOTOR *monster);

#endif
