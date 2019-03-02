#include "linear.h"

void Linear_Init(struct LINEAR *linear)
{
	linear->LinearDirection = 0;
	linear->TurnDirection = 0;
	linear->MaxSpeed = 300;
	linear->MinSpeed = 15;
	linear->AccelerateDistance = 100;
	linear->DecelerateDistance = 150;
	linear->ErrorMargin = 1;
	linear->MaxThetaError = 30;
	linear->MaxOmega = 15;
	linear->Omega = 0;
	linear->Speed = 0;
}

double Get_Linear_ThetaError(struct LINEAR *linear, struct POSITION *pos)
{
	volatile double dx = (pos->CommandPoint.X) - (pos->Points[0].X);
	volatile double dy = (pos->CommandPoint.Y) - (pos->Points[0].Y);
	volatile double theta = atan2l(dy, dx)*(180/PI);
	volatile double err = theta - (pos->Points[0].Theta);
	err += 1080;
	err = fmod(err, 360.0);
	
	if(linear->LinearDirection == 0){
		if(err>90 && err<270){
			linear->LinearDirection = -1;
			err += 180;
			err = fmod(err, 360.0);
		}else{
			linear->LinearDirection = 1;
		}
	}
	return err;
}

double Get_Linear_DistanceError(struct POSITION *pos)
{
	return sqrt( pow((pos->CommandPoint.X) - (pos->Points[0].X),2)
						 + pow((pos->CommandPoint.Y) - (pos->Points[0].Y),2));
}

void Calculate_Linear_Omega(struct LINEAR *linear, int updatefreq, double thetaerror)
{
	volatile double a = linear->MaxThetaError;
	volatile double err = thetaerror;
	if(linear->TurnDirection == -1) err = 360-err;
	
	//positive turn
	if((err < a) && (fabs(linear->Omega) > (err/a)*(linear->MaxOmega))){
		linear->Omega = (linear->TurnDirection)*(err/a)*(linear->MaxOmega);
	}else{
		linear->Omega = (linear->Omega) + 
										(linear->TurnDirection)*(0.5*pow(linear->MaxOmega,2)/a)/updatefreq;
		if(linear->Omega > linear->MaxOmega) linear->Omega = linear->MaxOmega;
		else if(linear->Omega < -(linear->MaxOmega)) linear->Omega = -(linear->MaxOmega);
	}
}

double Get_Linear_TurnSpeed(struct LINEAR *linear, struct POSITION *pos, int updatefreq, double distanceerror, double thetaerror)
{
	if(distanceerror < linear->ErrorMargin){
		//inside distance margin
		return 0;
	}else{
		if(thetaerror < 180) 	linear->TurnDirection = 1;
		else 									linear->TurnDirection = -1;
		Calculate_Linear_Omega(linear, updatefreq, thetaerror);
		return (linear->Omega)*(pos->WhealDistance)/(4*PI);
	}
}

double Get_Linear_LinearSpeed(struct LINEAR *linear, int updatefreq, double distanceerror, double thetaerror)
{
	volatile double t = linear->MaxThetaError;
	volatile double e = linear->ErrorMargin;
	volatile double a = linear->AccelerateDistance;
	volatile double err = distanceerror;
	if(err < e){
		//inside error margin
		linear->Speed = 0;
	}
	else if((thetaerror > t) && (360-t > thetaerror)){
		//outside max theta error margin, reduce speed
		linear->Speed = (linear->Speed)
										- (linear->LinearDirection)*(0.5*pow(linear->MaxSpeed,2)/a)/updatefreq;
		if((linear->LinearDirection)*(linear->Speed) < 0)
			linear->Speed = 0;
	}else{
		//positive speed
		if((err < a) && (fabs(linear->Speed) > (err/a)*(linear->MaxSpeed))){
			//breaking
			linear->Speed = (linear->LinearDirection)*((linear->MinSpeed) + (err/a)*(linear->MaxSpeed - linear->MinSpeed));
		}else{
			//accelerating
			linear->Speed = (linear->Speed)
											+ (linear->LinearDirection)*(0.5*pow(linear->MaxSpeed,2)/a)/updatefreq;
			if(linear->Speed > linear->MaxSpeed)					linear->Speed = linear->MaxSpeed;
			else if(linear->Speed < -(linear->MaxSpeed))	linear->Speed = -(linear->MaxSpeed);
		}
	}
	return linear->Speed;
}











