#include "position.h"

void Set_Command_Point(struct POSITION *pos, double x, double y, double theta)
{
	pos->CommandPoint.X = x;
	pos->CommandPoint.Y = y;
	pos->CommandPoint.Theta = theta;
}

void Update_Position_Spi(struct POSITION *pos, struct SPI *spi)
{
	pos->Points[1].X = pos->Points[0].X;
	pos->Points[1].Y = pos->Points[0].Y;
	pos->Points[1].Theta = pos->Points[0].Theta;
	
	pos->Points[0].X = spi->Rx[0];
	pos->Points[0].Y = spi->Rx[1];
	pos->Points[0].Theta = spi->Rx[2];
}


void Position_Init(struct POSITION *pos)
{
	pos->CommandPoint.X = 0;
	pos->CommandPoint.Y = 0;
	pos->CommandPoint.Theta = 0;
	pos->Points[0].X = 0;
	pos->Points[0].Y = 0;
	pos->Points[0].Theta = 0;
	pos->SpeedL = 0;
	pos->SpeedR = 0;
	pos->Speed = 0;
	pos->Omega = 0;
	pos->UpdateFrequency = 1000;
	pos->EncoderDistance = 213.2;
	pos->WhealDistance = 87.3;
}



