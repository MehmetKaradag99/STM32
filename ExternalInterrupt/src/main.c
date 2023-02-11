
#include "stm32f4xx.h"
#include <stdbool.h>

uint8_t flag = 1;

GPIO_InitTypeDef gpioInitStuct;
EXTI_InitTypeDef extiInit;
NVIC_InitTypeDef NVIC_Initial;




void gpio_Init(GPIO_TypeDef* gpiox);
void externalIntConfig();
void NVIC_Config();


int main(void)
{
	gpio_Init(GPIOA);
	externalIntConfig();
	NVIC_Config();

  while (1)
  {

  }
}

void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0 == SET))
	{
		if(flag)
		{
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
		flag=0;
		}
		else
		{
			GPIO_ResetBits(GPIOD,GPIO_Pin_13);
			flag=1;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void NVIC_Config()
{
	NVIC_Initial.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Initial.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_Initial);
}

void externalIntConfig()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	extiInit.EXTI_Line = EXTI_Line0;
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
	extiInit.EXTI_LineCmd = ENABLE;

	EXTI_Init(&extiInit);


}


void gpio_Init(GPIO_TypeDef* gpiox)
{

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);


	gpioInitStuct.GPIO_Mode=GPIO_Mode_IN;
	gpioInitStuct.GPIO_Pin = GPIO_Pin_0;
	gpioInitStuct.GPIO_OType=GPIO_OType_PP;
	gpioInitStuct.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(gpiox,&gpioInitStuct);


	gpioInitStuct.GPIO_Mode = GPIO_Mode_OUT;
	gpioInitStuct.GPIO_OType = GPIO_OType_PP;
	gpioInitStuct.GPIO_Pin = GPIO_Pin_13;
	gpioInitStuct.GPIO_Speed = GPIO_Speed_100MHz;
	gpioInitStuct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD,&gpioInitStuct);


}
