#include "stm32f4xx.h"
#include <string.h>

#define UNUSED(x)   (void)x

void delay();
void RCC_Config();
void gpio_config();
void i2c_init();
void delayConfig();
void BMP180_GetCalibration();
void BMP180_Read_UncompensatedTempValue(void);
float BMP180_TemperatureValue(void);
void error_handler();

float temperature=0;

#define BMPAddrWrite (uint8_t)0xEE
#define BMPAddrRead (uint8_t)0xEF

#define Matched 1
#define notMatched 0
#define NoImportent 1
#define is_PLL_Clk 0x08

uint8_t wait_ms = 5;

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

float Temperature = 0;




RCC_ClocksTypeDef rcc; // for debug


int main(void)
{
	//RCC_DeInit();
	RCC_Config();
	gpio_config();
	i2c_init();
	delayConfig();
	//RCC_GetClocksFreq(&rcc);

  while (1)
  {
	  BMP180_GetCalibration();
	  BMP180_Read_UncompensatedTempValue();
	  temperature = BMP180_TemperatureValue();
	  UNUSED(temperature);
  }
}

void RCC_Config()
{

	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	FLASH_PrefetchBufferCmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_2);
	while(RCC_WaitForHSEStartUp() != SUCCESS);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != SET);


	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div2);
	RCC_PLLConfig(RCC_PLLSource_HSE,8,168,2,4);

	RCC_PLLCmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY != SET));

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while(RCC_GetSYSCLKSource() != is_PLL_Clk);

}

void delayConfig()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseInitTypeDef tim_init;
	memset(&tim_init,0,sizeof(tim_init));

	tim_init.TIM_ClockDivision = TIM_CKD_DIV2 ;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 0xFF;
	tim_init.TIM_Prescaler =41999 ;
	//tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM3,&tim_init);

	TIM_Cmd(TIM3,ENABLE);
}

void delay()
{
	TIM_SetCounter(TIM3,0);
	while(TIM_GetCounter(TIM3) != (wait_ms));
}

void gpio_config()
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);  //for LED
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //for i2c
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  // for timer

	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	// i2c sda ->B7 scl->B6
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;


	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);

	//timer

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

}
void i2c_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	I2C_InitTypeDef i2c_init;
	memset(&i2c_init,0,sizeof(i2c_init));

	//I2C_DeInit(I2C1);

	i2c_init.I2C_Ack = I2C_Ack_Enable;
	i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	i2c_init.I2C_ClockSpeed = 400000;
	i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c_init.I2C_Mode = I2C_Mode_I2C;
	I2C_StretchClockCmd(I2C1,ENABLE);

	I2C_Init(I2C1,&i2c_init);

	I2C_Cmd(I2C1,ENABLE);

}


void BMP180_GetCalibration()
{
	      uint8_t CALIBRATION_SIZE = 22;
	      uint8_t bmp180_calibArray[CALIBRATION_SIZE];
	      uint8_t i = 0 ;
	      uint8_t BMPRegAddr = (uint8_t)0xAA;


		  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		  I2C_GenerateSTART(I2C1,ENABLE);
		  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));


	      I2C_Send7bitAddress(I2C1, BMPAddrWrite, I2C_Direction_Transmitter);
	      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	      I2C_Cmd(I2C1,ENABLE);
	      while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
	      I2C_SendData(I2C1,BMPRegAddr);
	      while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	      I2C_GenerateSTART(I2C1,ENABLE);

	      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	      I2C_Send7bitAddress(I2C1, BMPAddrRead, I2C_Direction_Receiver);
	      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){}

	  	  while(CALIBRATION_SIZE)
	  	  {
	        if(CALIBRATION_SIZE == 1)
	        {
	          I2C_AcknowledgeConfig(I2C1,DISABLE);
	          I2C_GenerateSTOP(I2C1,ENABLE);
	        }

	         while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE));
	         if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
	         {
	           bmp180_calibArray[i] = I2C_ReceiveData(I2C1);
	           ++i;
	  	     }
	         CALIBRATION_SIZE--;
	  	  }

	      i = 0;

	AC1 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	AC2 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	AC3 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	AC4 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	AC5 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	AC6 = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	B1 	= (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	B2  = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	MB	= (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	MC 	= (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;
	MD  = (uint16_t)((bmp180_calibArray[i]<<8)|(bmp180_calibArray[i+1])); i+=2;

	I2C_AcknowledgeConfig(I2C1, ENABLE);

	BMP180_Read_UncompensatedTempValue();

}

void BMP180_Read_UncompensatedTempValue(void)
{
    uint8_t ReadMemReg = 0xF6;
	uint8_t data = 0x2E;
	uint8_t ReadMemRegVals[2];
	uint8_t worttenReg[] = {0xF4};
	uint8_t i = 0;
	uint8_t DataSize = 2;

	  //I2C_Cmd(I2C1,ENABLE);

	  //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	  I2C_GenerateSTART(I2C1,ENABLE);

	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	  I2C_Send7bitAddress(I2C1, BMPAddrWrite, I2C_Direction_Transmitter);
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	  //I2C_Cmd(I2C1,ENABLE);

	  while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
	  I2C_SendData(I2C1,0xF4);
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	 // while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	  //I2C_GenerateSTART(I2C1,ENABLE);  /// it is repeated start!!


	  //while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	  //I2C_Send7bitAddress(I2C1, BMPAddrWrite, I2C_Direction_Transmitter);
	  //while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){}

	  while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
	  I2C_SendData(I2C1,data);
	  //I2C1->DR = 0x2E;
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	  I2C_GenerateSTOP(I2C1,ENABLE);

	  // wait for 5ms
	  delay();
	  //for(int j = 0 ; j<840000 ; j++);


	  //I2C_Cmd(I2C1,ENABLE);


	  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	  I2C_GenerateSTART(I2C1,ENABLE);

	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	  I2C_Send7bitAddress(I2C1, BMPAddrWrite, I2C_Direction_Transmitter);
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	  I2C_Cmd(I2C1,ENABLE);
	  while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
	  I2C_SendData(I2C1,ReadMemReg);
	  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	  ///////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////
	  I2C_GenerateSTART(I2C1,ENABLE);

	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	  I2C_Send7bitAddress(I2C1, BMPAddrRead, I2C_Direction_Receiver);
	  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


	  while(DataSize)
	  {
		if(DataSize == 1)
		{
		  I2C_AcknowledgeConfig(I2C1,DISABLE);
		  I2C_GenerateSTOP(I2C1,ENABLE);
		}

		 while(!I2C_GetFlagStatus(I2C1,I2C_FLAG_RXNE));
		 if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		 {
			 ReadMemRegVals[i] = I2C_ReceiveData(I2C1);
			 ++i;
		 }
		 DataSize--;
	  }

	  UT = (uint16_t)((ReadMemRegVals[0]<<8) | (ReadMemRegVals[1]));

	  I2C_AcknowledgeConfig(I2C1,ENABLE);

	  BMP180_TemperatureValue();

}


float BMP180_TemperatureValue(void)
{

	X1 = ((UT-AC6)*AC5)/32768;
	X2 = ((MC*2048)/(X1+MD));
	B5 = (X1+X2);
	T =	 ((B5+8)/16);

	Temperature =(float)T/10.0;
	return Temperature;
}

void error_handler()
{
	GPIO_SetBits(GPIOD,GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	while(1);
}


