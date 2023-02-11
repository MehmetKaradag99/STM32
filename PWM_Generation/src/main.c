
#include "stm32f4xx.h"
#include <string.h>

TIM_TimeBaseInitTypeDef tim_init;
void GPIO_Config();
void TimPwmConfig();

int main(void)
{
	GPIO_Config();
	TimPwmConfig();
  while (1)
  {
	  TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	  for(int i = 0 ; i<16000000;++i);
	  TIM_SetCompare1(TIM2,tim_init.TIM_Period * 50/100);
	  TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	  for(int i = 0 ; i<16000000;++i);
	  TIM_SetCompare1(TIM2,tim_init.TIM_Period * 100/100);
	  TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	  for(int i = 0 ; i<16000000;++i);
	  TIM_SetCompare1(TIM2,tim_init.TIM_Period * 0/100);
	  TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);
	  for(int i = 0 ; i<16000000;++i);

  }
}


void GPIO_Config()
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct,0,sizeof(GPIO_InitStruct));

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);
}


void TimPwmConfig()
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);


	TIM_OCInitTypeDef pwm_init={0};
	//memset(&tim_init,0,sizeof(tim_init));

	tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period = 1000-1;
	tim_init.TIM_Prescaler = 15 ;
	tim_init.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2,&tim_init);

	TIM_CounterModeConfig(TIM2,TIM_CounterMode_Up);

	pwm_init.TIM_OCMode = TIM_OCMode_PWM1;
	pwm_init.TIM_OutputState = TIM_OutputState_Enable;
	pwm_init.TIM_Pulse = tim_init.TIM_Period * 25/100;

	TIM_OC1Init(TIM2,&pwm_init);

	TIM_Cmd(TIM2,ENABLE);






}





