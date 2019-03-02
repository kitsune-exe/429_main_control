#ifndef __SCRIPT_H
#define __SCRIPT_H

struct NAVIGATION
{
	double X;
	double Y;
	double Theta;
	double TurnDirection;
	double LinearDirection;
};

struct NAVIGATION test[5] = {
	{	 0,   0,   0,   0,   0},
	{500,   0,  90,   0,   0},
	{500, 500, 180,   0,   0},
	{  0, 500, 270,   0,   0},
	{  0,   0,   0,   0,   0}
};


#endif
