#include "turn.h"

void Turn_Init(struct TURN *turn)
{	
	turn->Direction = 0;
	turn->MaxOmega = 15;
	turn->MinOmega = 0;
	turn->ErrorMargin = 0.3;
	turn->OvershootMargin = 10;
	turn->AccelerateTheta = 20;
	turn->DecelerateTheta = 50;
	turn->DistanceMargin = 30;
	turn->Omega =	0;
}

double Get_Turn_ThetaError(struct TURN *turn, struct POSITION *position)
{
	volatile double err = (position->CommandPoint.Theta) - (position->Points[0].Theta);
	if(err < 0) 	err += 360;
	return err;
}

void Calculate_Turn_Omega(struct TURN *turn, int updatefreq, double thetaerror)
{
	volatile double e = turn->ErrorMargin;
	volatile double o = turn->OvershootMargin;
	volatile double a = turn->AccelerateTheta;
	volatile double d = turn->DecelerateTheta;
	volatile double err = thetaerror;
	if(turn->Direction == -1) err = 360-err;
	
	if((err < e) || (360-e < err)){
		//inside error margin
		turn->Omega = 0;
	}else if(360-o < err){
		//inside overshoot margin, negative turn
		err = 360-err;
		turn->Omega = -(turn->Direction)*(turn->MinOmega + (err/d)*(turn->MaxOmega - turn->MinOmega));
	}else{
		//positive turn
		if((err < d) && (fabs(turn->Omega) > (turn->MinOmega + (err/d)*(turn->MaxOmega - turn->MinOmega)))){
			turn->Omega = (turn->Direction)*(turn->MinOmega + (err/d)*(turn->MaxOmega - turn->MinOmega));
		}else{
			turn->Omega = (turn->Omega) + 
										(turn->Direction)*(0.5*pow(turn->MaxOmega,2)/a)/updatefreq;
			if(turn->Omega > turn->MaxOmega) turn->Omega = turn->MaxOmega;
			else if(turn->Omega < -(turn->MaxOmega)) turn->Omega = -(turn->MaxOmega);
		}
	}
}

double Get_Turn_TurnSpeed(struct TURN *turn, struct POSITION *pos, int updatefreq, double thetaerror)
{
	if(turn->Direction == 0){
		// decide turn direction
		if(thetaerror < 180) 	turn->Direction = 1;
		else 									turn->Direction = -1;
	}
	Calculate_Turn_Omega(turn, updatefreq, thetaerror);
	return (turn->Omega)*(pos->WhealDistance)/(4*PI);
}
