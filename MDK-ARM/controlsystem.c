#include "controlsystem.h"

void ControlSystem_Init(struct CONTROLSYSTEM *system)
{
	system->Mode = Stop;
	MotorControl_Init(&system->motorL);
	MotorControl_Init(&system->motorR);
	Position_Init(&system->position);
	Turn_Init(&system->turn);
	Linear_Init(&system->linear);
	system->UpdateFrequency = 1000;
	system->ThetaError = 0;
	system->TurnSpeed = 0;
	system->DistanceError = 0;
	system->LinearSpeed = 0;
}

void Robot_Go(struct CONTROLSYSTEM *system)
{
	if(system->Mode == Stop){
		system->DistanceError = Get_Linear_DistanceError(&system->position);
		system->ThetaError = Get_Turn_ThetaError(&system->turn, &system->position);
		system->LinearSpeed = 0;
		system->TurnSpeed = 0;
		
	}else if(system->Mode == Idle){
		system->LinearSpeed = 0;
		system->TurnSpeed = 0;
		system->motorL.IntegratedError = 0;
		system->motorR.IntegratedError = 0;
		
	}else if(system->Mode == Turn){
		system->LinearSpeed = 0;
		system->DistanceError = Get_Linear_DistanceError(&system->position);
		if(system->DistanceError > system->turn.DistanceMargin){
			system->Mode = Linear;
			Robot_Go(system);
			return;
		}
		system->ThetaError = Get_Turn_ThetaError(&system->turn, &system->position);
		system->TurnSpeed = Get_Turn_TurnSpeed(&system->turn, &system->position, 
																						system->UpdateFrequency, system->ThetaError);
		
	}else if(system->Mode == Linear){
		system->DistanceError = Get_Linear_DistanceError(&system->position);
		if(system->DistanceError < system->linear.ErrorMargin){
			system->Mode = Turn;
			Robot_Go(system);
			return;
		}
		system->ThetaError = Get_Linear_ThetaError(&system->linear, &system->position);
		system->TurnSpeed = Get_Linear_TurnSpeed(&system->linear, &system->position, system->UpdateFrequency, 
																							system->DistanceError, system->ThetaError);
		system->LinearSpeed = Get_Linear_LinearSpeed(&system->linear, system->UpdateFrequency, 
																									system->DistanceError, system->ThetaError);
	}
	
	Set_CommandSpeed(&system->motorL, (system->LinearSpeed) - (system->TurnSpeed));
	Set_CommandSpeed(&system->motorR, (system->LinearSpeed) + (system->TurnSpeed));
}

void Set_Navigate_Direction(struct CONTROLSYSTEM *system, short turndir, short lineardir)
{
	system->turn.Direction = turndir;
	system->linear.LinearDirection = lineardir;
}

int Check_NavigateComplete(struct CONTROLSYSTEM *system){
	if((system->Mode == Turn) && (system->DistanceError < system->turn.DistanceMargin) &&
		 (system->ThetaError < system->turn.ErrorMargin || system->ThetaError > 360-system->turn.ErrorMargin)){
		// complete
		return 1; 
	}else{
		// incomplete
		return 0;
	}
}
