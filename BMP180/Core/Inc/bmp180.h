/*
 * bmp180.h
 *
 *  Created on: Oct 11, 2022
 *      Author: Ali
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_

#include "main.h"


//SLAVE ADDR
#define BMP180_SLAVE_ADDR_WR	0xEE
#define BMP180_SLAVE_ADDR_Read	0xEF
#define wrottenVal              0x2E

extern short AC1;
extern short AC2;
extern short AC3;
extern unsigned short AC4;
extern unsigned short AC5;
extern unsigned short AC6;
extern short B1;
extern short B2;
extern short MB;
extern short MC;
extern short MD;

/********************/
extern long UT;
extern short oss;
extern long UP;
extern long X1;
extern long X2;
extern long X3;
extern long B3;
extern long B5;
extern long T;
extern unsigned long B4;
extern long B6;
extern unsigned long B7;

//some define macro
#define CALIBRATION_SIZE	22

//some functions
void BMP180_init(void);
void BMP180_Get_Calibration(void);
void BMP180_Read_UncompensatedTempValue(void);
float BMP180_TemperatureValue(void);




#endif /* INC_BMP180_H_ */

