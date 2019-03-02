#ifndef __TURN_H
#define __TURN_H

#include "motorcontrol.h"
#include "position.h"

struct TURN
{
	volatile short			Direction;							// direction: 1 left, -1 right, 0 auto
	double 							MaxOmega;								// degree/s
	double 							MinOmega;								// degree/s
	double							ErrorMargin;						// degree
	double							OvershootMargin;				// degree
	double							AccelerateTheta;				// degree
	double							DecelerateTheta;				// degree
	double							DistanceMargin;					// mm
	volatile double			Omega;									// degree/s
//	volatile double			ThetaError;   					// 0~360 degree
};

void Turn_Init(struct TURN *turn);

double Get_Turn_ThetaError(struct TURN *turn, struct POSITION *position);

void Calculate_Turn_Omega(struct TURN *turn, int updatefreq, double thetaerror);

double Get_Turn_TurnSpeed(struct TURN *turn, struct POSITION *pos, int updatefreq, double thetaerror);



#endif
