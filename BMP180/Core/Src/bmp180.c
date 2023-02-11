/*
 * bmp180.c
 *
 *  Created on: Oct 11, 2022
 *      Author: Ali
 */

#ifndef SRC_BMP180_C_
#define SRC_BMP180_C_

#include "bmp180.h"

//some variables
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;

/********************/
long UT = 0;
short oss = 0;
long UP = 0;
long X1 = 0;
long X2 = 0;
long X3 = 0;
long B3 = 0;
long B5 = 0;
long T = 0;
unsigned long B4 = 0;
long B6 = 0;
unsigned long B7 = 0;

float Temperature=0;

extern I2C_HandleTypeDef hi2c1;

static void error_handler();

void BMP180_init(void)
{
	if(HAL_I2C_IsDeviceReady(&hi2c1, BMP180_SLAVE_ADDR_WR, 1,100000) !=  HAL_OK)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
		error_handler();
	}
	BMP180_Get_Calibration();
}

static void error_handler()
{
	while(1);
}

void BMP180_Get_Calibration(void)
{
	uint8_t CalibratinVals[CALIBRATION_SIZE] = {0};
	uint8_t i=0;

	uint16_t BMPregAddr = (uint16_t)0xAA;

	HAL_I2C_Mem_Read(&hi2c1, BMP180_SLAVE_ADDR_WR, BMPregAddr, 1, CalibratinVals, CALIBRATION_SIZE, 100000);

	AC1 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	AC2 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	AC3 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	AC4 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	AC5 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	AC6 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	B1 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	B2 = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	MB = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	MC = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;
	MD = (uint16_t)(CalibratinVals[i] << 8 | CalibratinVals[i+1]) ;
	i+=2;

	BMP180_Read_UncompensatedTempValue();
}

void BMP180_Read_UncompensatedTempValue(void)
{
	uint8_t ReadMemReg = 0xF6;
	uint8_t data = 0x2E;
	uint8_t ReadMemRegVals[2] = {0};
	uint8_t worttenReg[] = {0xF4};

	HAL_I2C_Mem_Write(&hi2c1, BMP180_SLAVE_ADDR_WR, 0xF4, 1,&data ,1, 100000);
	HAL_Delay(5);

	HAL_I2C_Mem_Read(&hi2c1, BMP180_SLAVE_ADDR_WR, 0xF6, 1, ReadMemRegVals, 2, 100000);

	UT= (uint16_t)((ReadMemRegVals[0]<<8) | (ReadMemRegVals[1]));

}

float BMP180_TemperatureValue(void)
{

	X1 = ((UT-AC6)*AC5)/32768;
	X2 = ((MC*2048)/(X1+MD));
	B5 = (X1+X2);
	T =	 ((B5+8)/16);

	Temperature =(float)T/10;
	return Temperature;
}

#endif /* SRC_BMP180_C_ */

