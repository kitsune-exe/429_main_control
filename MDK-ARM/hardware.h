#ifndef __HARDWARE_H
#define __HARDWARE_H

/* 	In hardware.h & hardware.c define:
		Components of the robot
		Geometry constants of the robot
		Pin & Timer connection of circuit
		Basic operation of components         */
		
#include "gpio.h"
#include "tim.h"
#include "crc.h"
#include "spi.h"

//---------------------------------------------------------------------------
struct MONSTERMOTOR
{
	GPIO_TypeDef 			*Cw_port;
	uint16_t 					Cw_pin;
	GPIO_TypeDef 			*Ccw_port;
	uint16_t 					Ccw_pin;
	TIM_HandleTypeDef *Timer;
	uint32_t 					Channel;
};
void MonsterMotor_Init(struct MONSTERMOTOR* motor);
void MonsterMotor_Write(struct MONSTERMOTOR* motor, double duty);


//---------------------------------------------------------------------------
struct ENCODER
{
	TIM_HandleTypeDef *Timer;
	uint32_t 					Channel1;
	uint32_t 					Channel2;
	double						Radius;
	double						ReductionRatio;
	double 						Resolution;
	short							Direction;					// +1 or -1
};
void Encoder_Init_16bit(struct ENCODER *encoder);
void Encoder_Init_32bit(struct ENCODER *encoder);
unsigned int Encoder_Read(struct ENCODER *encoder);


//---------------------------------------------------------------------------
struct SPI
{
	SPI_HandleTypeDef *Spi;
	GPIO_TypeDef 			*Nss_port;
	uint16_t 					Nss_pin;
	uint16_t					length;
	int32_t	 					Tx[7];
	int32_t	 					Rx[7];
	volatile int			ErrorCount;
	volatile int 			TransmitCount;
	volatile double 	SuccessRate;
};
void Spi_Init(struct SPI *spi);
void Spi_Write(struct SPI *spi, uint8_t n, uint32_t value);
void Spi_TransmitReceive(struct SPI *spi);
int Spi_Crc_Error(struct SPI *spi);


//---------------------------------------------------------------------------
struct HARDWARE
{
	struct MONSTERMOTOR motorR;
	struct MONSTERMOTOR motorL;
	struct ENCODER encoderR;
	struct ENCODER encoderL;
	struct SPI spiToslave;
};
void Hardware_Init(struct HARDWARE *hardware);
void Hardware_Mapping(struct HARDWARE *hardware);




#endif
