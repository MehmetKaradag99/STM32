/*
 * myLib.c
 *
 *  Created on: 8 Eki 2022
 *      Author: Ali
 */



#include "myLib.h"


void waitingForInterrupt()
{
	__WFI();
}

 void StartConfig()
{
	RCC_DeInit();
	RCC_Config();  //Default -> 168Mhz
    GPIO_Config();
	PWM_Config();
    ADC_Config();
	delay_config();
	externalIntConfig();
	NVIC_Config();
}

 void RunningServo()
{


	  ADC_Value = ADC_GetValue();
	  pulse = (uint32_t)map(ADC_Value,0,255,250,1725);
	  TIM_SetCompare2(TIM2,pulse);
	  TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
	  TIM_SetCounter(TIM3,1);
	  delay_ms();
	  //while(TIM_GetCounter(TIM3)<100);

}


 void RCC_Config()
{

	//50 MHz with HSE

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


 void GPIO_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	// For ADC  portB 1.pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd= GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);



#ifdef LED_Config
	//For LEDs  PortD 12-13-14-15.pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOD,&GPIO_InitStruct);
#endif

	//For PWM portA 1.pin tim-> channel 2

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	// external interrupt

	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(GPIOA,&GPIO_InitStruct);



}
 void ADC_Config()
{

	// 12 bit resolution max 4095

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

	ADC_InitTypeDef ADC_InitSturct;
	ADC_CommonInitTypeDef ADC_ComInit;

	memset(&ADC_ComInit,0,sizeof(ADC_ComInit));
	memset(&ADC_InitSturct,0,sizeof ADC_InitSturct);

	ADC_InitSturct.ADC_Resolution = ADC_Resolution_8b;
	ADC_InitSturct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitSturct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitSturct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1,&ADC_InitSturct);


	ADC_ComInit.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_ComInit.ADC_Mode = ADC_Mode_Independent;
	ADC_ComInit.ADC_Prescaler = ADC_Prescaler_Div4;

	ADC_CommonInit(&ADC_ComInit);

	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_480Cycles);

	ADC_ContinuousModeCmd(ADC1,ENABLE);

	ADC_Cmd(ADC1,ENABLE);

	ADC_SoftwareStartConv(ADC1);


}
 void PWM_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);


	TIM_OCInitTypeDef pwm_init={0};

	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;  // clk freq 84 MHz
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 9999 ;
	tim_init.TIM_Prescaler = 167 ;
	//tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2,&tim_init);

	TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);

	pwm_init.TIM_OCMode = TIM_OCMode_PWM1;
	pwm_init.TIM_OutputState = TIM_OutputState_Enable;
	pwm_init.TIM_OCPolarity=TIM_OCPolarity_High;
	pwm_init.TIM_Pulse = 0;

	TIM_OC2Init(TIM2,&pwm_init);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);


	TIM_Cmd(TIM2,ENABLE);

}
 uint16_t ADC_GetValue()
{
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);

	return (uint16_t)ADC_GetConversionValue(ADC1);
}

 uint16_t map(const uint32_t in , const uint32_t in_min,const uint32_t in_max,const uint32_t out_min,const uint32_t out_max)
{
	return (in-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
}

 void delay_config()
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	tim_init.TIM_ClockDivision = TIM_CKD_DIV4 ;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 10;
	tim_init.TIM_Prescaler = 2099;
	tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM3,&tim_init);
}

void externalIntConfig()
{

	EXTI_InitTypeDef extiInit = {0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	extiInit.EXTI_Line = EXTI_Line0;
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
	extiInit.EXTI_LineCmd = ENABLE;

	EXTI_Init(&extiInit);


}

void NVIC_Config()
{

	NVIC_InitTypeDef NVIC_Initial={0};

	NVIC_Initial.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Initial.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_Initial);
}


 void Error_Handler()
{
	while(1);
}

void EXTI0_IRQHandler()
{

	if(EXTI_GetITStatus(EXTI_Line0 == SET))
	{
		GPIO_ToggleBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	}

	EXTI_ClearITPendingBit(EXTI_Line0);
}


