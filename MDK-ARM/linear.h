#ifndef __LINEAR_H
#define __LINEAR_H

#include "motorcontrol.h"
#include "position.h"

struct LINEAR
{
	volatile short			LinearDirection;						// direction: 1 Forward, -1 Reverse, 0 auto
	volatile short			TurnDirection;							// direction: 1 left, -1 right, 0 auto
	double 							MaxSpeed;							// mm/s
	double 							MinSpeed;							// mm/s
	double 							AccelerateDistance;		// mm
	double 							DecelerateDistance;		// mm
	double 							ErrorMargin;					// mm
	double 							MaxThetaError;				// degree
	double 							MaxOmega;							// degree/s
	volatile double			Omega;											// degree/s
	volatile double			Speed;											// mm/s
};

void Linear_Init(struct LINEAR *linear);

double Get_Linear_ThetaError(struct LINEAR *linear, struct POSITION *pos);

double Get_Linear_DistanceError(struct POSITION *pos);

void Calculate_Linear_Omega(struct LINEAR *linear, int updatefreq, double thetaerror);

double Get_Linear_TurnSpeed(struct LINEAR *linear, struct POSITION *pos, int updatefreq, 
														double distanceerror, double thetaerror);

double Get_Linear_LinearSpeed(struct LINEAR *linear, int updatefreq, double distanceerror, double thetaerror);

#endif
