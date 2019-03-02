#include "hardware.h"

void Hardware_Init(struct HARDWARE *hardware)
{
	Hardware_Mapping(hardware);
	MonsterMotor_Init(&hardware->motorR);
	MonsterMotor_Init(&hardware->motorL);
	Encoder_Init_16bit(&hardware->encoderR);
	Encoder_Init_16bit(&hardware->encoderL);
	Spi_Init(&hardware->spiToslave);
}

void Hardware_Mapping(struct HARDWARE *hardware)
{
	/* 	Cw & Ccw had been modified to be the direction of:
			Cw  -> go foward 
			Ccw -> go backward of the robot,
			It might not be the same as the definition of Monster Motor Shield  	*/
	hardware->motorL.Cw_port =		GPIOD;
	hardware->motorL.Cw_pin = 		GPIO_PIN_4;
	hardware->motorL.Ccw_port = 	GPIOD;
	hardware->motorL.Ccw_pin = 		GPIO_PIN_3;
	hardware->motorL.Timer = 			&htim3;
	hardware->motorL.Channel = 		TIM_CHANNEL_1;
	
	hardware->motorR.Cw_port = 		GPIOD;
	hardware->motorR.Cw_pin = 		GPIO_PIN_6;
	hardware->motorR.Ccw_port = 	GPIOD;
	hardware->motorR.Ccw_pin = 		GPIO_PIN_5;
	hardware->motorR.Timer = 			&htim4;
	hardware->motorR.Channel = 		TIM_CHANNEL_1;
	
	hardware->encoderL.Timer = 					&htim2;
	hardware->encoderL.Channel1 = 			TIM_CHANNEL_1;
	hardware->encoderL.Channel2 = 			TIM_CHANNEL_2;
	hardware->encoderL.Radius = 				35.0;		
	hardware->encoderL.ReductionRatio = 10 * 1.25;
	hardware->encoderL.Resolution = 		2048;
	hardware->encoderL.Direction = 			1;
	
	hardware->encoderR.Timer = 					&htim5;
	hardware->encoderR.Channel1 = 			TIM_CHANNEL_1;
	hardware->encoderR.Channel2 = 			TIM_CHANNEL_2;
	hardware->encoderR.Radius = 				35.0;
	hardware->encoderR.ReductionRatio = 10 * 1.25;
	hardware->encoderR.Resolution = 		2048;
	hardware->encoderR.Direction = 			1;
	
	/*
	hardware->spiToslave.Spi = 				&hspi2;
	hardware->spiToslave.Nss_port = 	GPIOB;
	hardware->spiToslave.Nss_pin = 		GPIO_PIN_15;
	hardware->spiToslave.length = 		6;
	hardware->spiToslave.ErrorCount = 0;
	*/
	hardware->spiToslave.Spi = 				&hspi5;
	hardware->spiToslave.Nss_port = 	GPIOF;
	hardware->spiToslave.Nss_pin = 		GPIO_PIN_10;
	hardware->spiToslave.length = 		6;
	hardware->spiToslave.ErrorCount = 0;
	
}

void Encoder_Init_16bit(struct ENCODER *encoder)
{
	HAL_TIM_Encoder_Start(encoder->Timer, encoder->Channel1);
	HAL_TIM_Encoder_Start(encoder->Timer, encoder->Channel2);
	encoder->Timer->Instance->CNT = 32767;
}

void Encoder_Init_32bit(struct ENCODER *encoder)
{
	HAL_TIM_Encoder_Start(encoder->Timer, encoder->Channel1);
	HAL_TIM_Encoder_Start(encoder->Timer, encoder->Channel2);
	encoder->Timer->Instance->CNT = 2147483647;
}

unsigned int Encoder_Read(struct ENCODER *encoder)
{
	volatile unsigned int cnt;
	cnt = encoder->Timer->Instance->CNT;
	return cnt;
}

void MonsterMotor_Init(struct MONSTERMOTOR* motor)
{
	__HAL_TIM_SetCompare(motor->Timer, motor->Channel, 0);
	HAL_TIM_PWM_Start(motor->Timer, motor->Channel);
}

void MonsterMotor_Write(struct MONSTERMOTOR* motor, double duty)
{
	if(duty > 0){
		HAL_GPIO_WritePin(motor->Cw_port, motor->Cw_pin, GPIO_PIN_SET);   
		HAL_GPIO_WritePin(motor->Ccw_port, motor->Ccw_pin, GPIO_PIN_RESET);   
		__HAL_TIM_SetCompare(motor->Timer, motor->Channel, (int)90*duty);
	}else if(duty < 0){
		HAL_GPIO_WritePin(motor->Cw_port, motor->Cw_pin, GPIO_PIN_RESET);   
		HAL_GPIO_WritePin(motor->Ccw_port, motor->Ccw_pin, GPIO_PIN_SET);   
		__HAL_TIM_SetCompare(motor->Timer, motor->Channel, (int)-90*duty);
	}else{
		HAL_GPIO_WritePin(motor->Cw_port, motor->Cw_pin, GPIO_PIN_RESET);   
		HAL_GPIO_WritePin(motor->Ccw_port, motor->Ccw_pin, GPIO_PIN_RESET);   
	}
}

void Spi_Init(struct SPI *spi)
{
	HAL_GPIO_WritePin(spi->Nss_port, spi->Nss_pin, GPIO_PIN_SET);
}

void Spi_Write(struct SPI *spi, uint8_t n, uint32_t value)
{
	spi->Tx[n] = value;
}

void Spi_TransmitReceive(struct SPI *spi)
{
	HAL_GPIO_WritePin(spi->Nss_port, spi->Nss_pin, GPIO_PIN_RESET);
	spi->Tx[(spi->length)] = HAL_CRC_Calculate(&hcrc,(uint32_t *) &spi->Tx[0], 1*(spi->length));
	HAL_SPI_TransmitReceive(spi->Spi, (uint8_t*)spi->Tx, (uint8_t*)spi->Rx, 4*((spi->length)+1), 2);
	HAL_GPIO_WritePin(spi->Nss_port, spi->Nss_pin, GPIO_PIN_SET);
}

int Spi_Crc_Error(struct SPI *spi)
{
	uint32_t crc = HAL_CRC_Calculate(&hcrc,(uint32_t *) &spi->Rx[0], 1*(spi->length));
	uint32_t rx = spi->Rx[(spi->length)];
	if(crc == rx){
		spi->TransmitCount ++;
		return 0;
	}else{
		spi->TransmitCount ++;
		spi->ErrorCount ++;
		if(spi->TransmitCount > 1000){
			spi->SuccessRate = 100*(spi->TransmitCount - spi->ErrorCount)/(double)(spi->TransmitCount);
			spi->ErrorCount = 0;
			spi->TransmitCount = 0;
		}
		return 1;
	}
}
