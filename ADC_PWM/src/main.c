#include "stm32f4xx.h"
#include <string.h>

#define NoImportent 1
#define is_PLL_Clk 0x08

uint16_t ADC_Value;

TIM_TimeBaseInitTypeDef tim_init;
RCC_ClocksTypeDef Rcc;

static void Error_Handler();
static void RCC_Config();
static void GPIO_Config();
static void ADC_Config();
static void PWM_Config();
static uint16_t ADC_GetValue();
static void Adjust_LED_Brightness();

int main(void)
{

	RCC_Config();
    GPIO_Config();
    ADC_Config();
	PWM_Config();
	RCC_GetClocksFreq(&Rcc);

  while (1)
  {
	  ADC_Value = ADC_GetValue();
	  Adjust_LED_Brightness();
  }
}


static void RCC_Config()
{

	//50 MHz with HSE

	RCC_HSEConfig(RCC_HSE_ON);
	FLASH_PrefetchBufferCmd(ENABLE);
	//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_1);
	while(RCC_WaitForHSEStartUp() != SUCCESS);
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != SET);


	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div2);
	RCC_PLLConfig(RCC_PLLSource_HSE,4,50,2,NoImportent);

	RCC_PLLCmd(ENABLE);

	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY != SET));

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

	while(RCC_GetSYSCLKSource() != is_PLL_Clk);
}


static void GPIO_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	// For ADC  portA 0.pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	//For LEDs  PortD 12-13-14-15.pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);

	//For PWM portA 1.pin

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);



}
static void ADC_Config()
{

	// 12 bit resolution max 4095

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
	ADC_ComInit.ADC_Prescaler = ADC_Prescaler_Div8;

	ADC_CommonInit(&ADC_ComInit);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_56Cycles);

	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConv(ADC1);


}
static void PWM_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);


	TIM_OCInitTypeDef pwm_init={0};

	tim_init.TIM_ClockDivision = TIM_CKD_DIV4;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 4095-1;
	tim_init.TIM_Prescaler = 0 ;
	tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2,&tim_init);

	TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);

	pwm_init.TIM_OCMode = TIM_OCMode_PWM1;
	pwm_init.TIM_OutputState = TIM_OutputState_Enable;
	pwm_init.TIM_Pulse = 0;

	TIM_OC2Init(TIM2,&pwm_init);

	TIM_Cmd(TIM2,ENABLE);
}
static uint16_t ADC_GetValue()
{
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);

	return (uint16_t)ADC_GetConversionValue(ADC1);
}
static void Adjust_LED_Brightness()
{
	 TIM_SetCompare2(TIM2,ADC_Value);
	 TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
}
static void Error_Handler()
{
	while(1);
}
