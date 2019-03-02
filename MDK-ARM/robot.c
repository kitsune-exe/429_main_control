#include "robot.h"
#include "script.h"

extern struct ROBOT eurobot2019;
extern double X, Y, Theta;
extern int p;

/*
extern volatile int testtime;
extern double testspeed;
void PIDtester()
{
	if(testtime >= 0 && testtime <= 1500){
		if(testtime <= 600){
			Set_CommandSpeed(&eurobot2019.controlsystem.motorL, testspeed*testtime/300);
			Set_CommandSpeed(&eurobot2019.controlsystem.motorR, testspeed*testtime/300);
		}
		if(testtime >= 900){
			Set_CommandSpeed(&eurobot2019.controlsystem.motorL, testspeed*(1500-testtime)/600);
			Set_CommandSpeed(&eurobot2019.controlsystem.motorR, testspeed*(1500-testtime)/600);			
		}
		if(testtime % 10 == 5){
			Spi_Write(&eurobot2019.hardware.spiToslave, 0, testtime - 5);
			Spi_Write(&eurobot2019.hardware.spiToslave, 1, (int) eurobot2019.controlsystem.motorL.AverageSpeed);
			Spi_Write(&eurobot2019.hardware.spiToslave, 2, (int) eurobot2019.controlsystem.motorL.Duty);
			Spi_TransmitReceive(&eurobot2019.hardware.spiToslave);
		}
		testtime ++;
	}else{
		Set_CommandSpeed(&eurobot2019.controlsystem.motorL, 0);
		Set_CommandSpeed(&eurobot2019.controlsystem.motorR, 0);
	}
}
*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim9)
	{
		
	}
	if(htim == &htim8)
	{
		motor_speed_handler(&eurobot2019.controlsystem.motorL, &eurobot2019.hardware.encoderL, &eurobot2019.hardware.motorL);
		motor_speed_handler(&eurobot2019.controlsystem.motorR, &eurobot2019.hardware.encoderR, &eurobot2019.hardware.motorR);
		//PIDtester();
		
		
		X = test[p].X;
		Y = test[p].Y;
		Theta = test[p].Theta;
		Set_Command_Point(&eurobot2019.controlsystem.position, X, Y, Theta);
		Set_Navigate_Direction(&eurobot2019.controlsystem, 0, 0);
		
		
		Spi_TransmitReceive(&eurobot2019.hardware.spiToslave);
		if(Spi_Crc_Error(&eurobot2019.hardware.spiToslave)){
			MX_SPI2_Init();
		}else{
			Update_Position_Spi(&eurobot2019.controlsystem.position, &eurobot2019.hardware.spiToslave);
		}
		Robot_Go(&eurobot2019.controlsystem);
		eurobot2019.NavigateComplete = Check_NavigateComplete(&eurobot2019.controlsystem);
		
		
		if(eurobot2019.NavigateComplete){
			//eurobot2019.NavigateComplete = 0;
			p ++;
			if(p >= 5) p = 0;
		}
		
	}	
	if(htim == &htim1)
	{
		
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);
}
