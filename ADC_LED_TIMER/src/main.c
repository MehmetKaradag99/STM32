#include "stm32f4xx.h"
#include <string.h>

/*define macros*/
#define NoImportent 1
#define is_PLL_Clk 0x08
/**********************/


/*some variables*/
uint16_t ADC_Value;
uint32_t GetCounterValue;
/****************/

/*function prototypes*/
static void Error_Handler(void);
static void RCC_Config(void);
static void StartConfigOptions(void);
static void GPIO_Config(void);
static void ADC_Config(void);
static uint16_t ADC_Getvalue(void);
static void Adjust_LED_Brightness(void);
static void Timer_Config(void);
/***************************************/




int main(void)
{

  StartConfigOptions();


  while (1)
  {
	  ADC_Value = ADC_Getvalue();
	  Adjust_LED_Brightness();
	  GetCounterValue = TIM_GetCounter(TIM2);
  }
}





static void RCC_Config()
{

	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);
	while(RCC_WaitForHSEStartUp() != SUCCESS);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != SET);

	RCC_PLLConfig(RCC_PLLSource_HSE,8,50,2,NoImportent);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);

	RCC_PLLCmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY != SET));

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	if(RCC_GetSYSCLKSource() != is_PLL_Clk)
		Error_Handler();

}


static void StartConfigOptions(void)
{
	RCC_Config();
	GPIO_Config();
	Timer_Config();
	ADC_Config();
	ADC_ContinuousModeCmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConv(ADC1);
}

static void GPIO_Config(void)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	// For ADC

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);


	//For LEDs

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);

	//For Timer2

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

}

static void ADC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_InitTypeDef ADC_InitSturct;
	ADC_CommonInitTypeDef ADC_ComInit;

	memset(&ADC_ComInit,0,sizeof(ADC_ComInit));
	memset(&ADC_InitSturct,0,sizeof ADC_InitSturct);

	ADC_InitSturct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitSturct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitSturct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitSturct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1,&ADC_InitSturct);


	ADC_ComInit.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_ComInit.ADC_Mode = ADC_Mode_Independent;
	ADC_ComInit.ADC_Prescaler = ADC_Prescaler_Div2;

	ADC_CommonInit(&ADC_ComInit);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_56Cycles);


}

static uint16_t ADC_Getvalue(void)
{

	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);

	return (uint16_t)ADC_GetConversionValue(ADC1);
}

static void Adjust_LED_Brightness(void)
{
	if(ADC_Value>1000)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_12);
		GPIO_ResetBits(GPIOD,GPIO_Pin_14);
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	}

	if(ADC_Value>2000)
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
	if(ADC_Value<1000)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_14);
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
	}
}

static void Timer_Config(void)
{

	TIM_TimeBaseInitTypeDef Tim_Init;
	memset(&Tim_Init,0,sizeof Tim_Init);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_DeInit(TIM2);

	Tim_Init.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim_Init.TIM_CounterMode = TIM_CounterMode_Up;
	Tim_Init.TIM_Period = 9999;
	Tim_Init.TIM_Prescaler = 49;
	Tim_Init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2,&Tim_Init);

	TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);

	TIM_Cmd(TIM2,ENABLE);

}

static void Error_Handler(void)
{
	while(1);
}
