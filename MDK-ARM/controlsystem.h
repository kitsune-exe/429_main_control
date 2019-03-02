#ifndef __CONTROLSYSTEM_H
#define __CONTROLSYSTEM_H

#include "motorcontrol.h"
#include "position.h"
#include "turn.h"
#include "linear.h"

enum MODE
{
	Stop = 0,
	Idle,
	Turn,
	Linear
};

struct CONTROLSYSTEM
{
	enum MODE						Mode;
	struct MOTORCONTROL motorL;
	struct MOTORCONTROL motorR;
	struct POSITION			position;
	struct TURN					turn;
	struct LINEAR				linear;
	int 								UpdateFrequency;						// 1kHz
	volatile double			ThetaError;									// degree
	volatile double			TurnSpeed;									// mm/s
	volatile double 		DistanceError;							// mm
	volatile double			LinearSpeed;								// mm/s
};


void ControlSystem_Init(struct CONTROLSYSTEM *system);

void Robot_Go(struct CONTROLSYSTEM *system);

void Set_Navigate_Direction(struct CONTROLSYSTEM *system, short turndir, short lineardir);

int Check_NavigateComplete(struct CONTROLSYSTEM *system);

#endif
