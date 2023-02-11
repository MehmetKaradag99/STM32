#include "stm32f4xx.h"
#include <string.h>
#include <stdbool.h>



///***TYPEDEF DEFINITIONS***///
typedef bool boolean;



///***DEFINE MACROS***///
#define Succes 1
#define NoImportent 1
#define is_PLL_Clk 0x08
//#define delay() while(TIM_GetCounter(TIM2)-83)
#define __weak __attribute__((weak))



///***GLOBAL VARIABLES***///
boolean __isMeasurementReady = false;
boolean __isDeviceReady = true;
float Distance;
uint32_t test;
RCC_ClocksTypeDef getRcc;  //for debug



///***FUNCTION PROTOTYPES***///
static void startConfig();
static void TimConfig();
static void RCC_Config();
static void gpioConfig();
static void interruptConfig();
static void startDevice();
static void MeasureDistance();
void HCSR04_CallbackEvents();
static void delay();
static void IRQ_Handler();






int main(void)
{
	startConfig();
  while(1)
  {
	  if(__isDeviceReady == true)
		  startDevice();
  }
}











static void startConfig()
{
	RCC_DeInit();
	RCC_Config();  //  -->> 84Mhz (tim and sysclk)
	gpioConfig();
	interruptConfig();
	TimConfig();
}

static void gpioConfig()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	//interrupt gpioa pin0

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	/*//timer gpioa pin1

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);

	GPIO_Init(GPIOA,&GPIO_InitStruct);*/

	//set trig gpioa pin2

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

}

static void interruptConfig()
{
	EXTI_InitTypeDef extiInit = {0};
	NVIC_InitTypeDef NVIC_Initial={0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	extiInit.EXTI_Line = EXTI_Line0;
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	extiInit.EXTI_LineCmd = ENABLE;

	EXTI_Init(&extiInit);

	NVIC_Initial.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Initial.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_Initial);

}

static void TimConfig()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_TimeBaseInitTypeDef tim_init = {0};

	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 0xFFFFFFFF-1;
	tim_init.TIM_Prescaler = 83 ;
	//tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2,&tim_init);

	TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);

	TIM_Cmd(TIM2,ENABLE);
}

static void RCC_Config()
{

	    RCC_HSEConfig(RCC_HSE_ON);
		FLASH_PrefetchBufferCmd(ENABLE);
		//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_5);
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

static void startDevice()
{
	__isDeviceReady = false;
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
	delay();
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);;
}

static void MeasureDistance()
{
	if(__isMeasurementReady != true)
	{
		__isMeasurementReady = true;
		TIM_SetCounter(TIM2,0);
	}
	else
	{
		Distance = (float) TIM_GetCounter(TIM2)/58;
		__isMeasurementReady = false;
		__isDeviceReady = true;
		//HCSR04_CallbackEvents();

	}
}

void EXTI0_IRQHandler()
{
	IRQ_Handler();
}

__weak void HCSR04_CallbackEvents()
{
	//TODO something
}

static void delay()
{
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2)<15);
}

static void IRQ_Handler()
{
	if(EXTI_GetITStatus(EXTI_Line0))
	{
		if(EXTI_GetFlagStatus(EXTI_Line0))
		{
			MeasureDistance();
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}
