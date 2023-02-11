#include "stm32f4xx.h"

void gpio_Init();
void externalIntConfig();
void NVIC_Config();
static void LED_IRQhandler();
static void config();

LED_t LED = LED12;

int main(void)
{
	config();
  while (1);
}

static void config()
{
	gpio_Init();
	externalIntConfig();
	NVIC_Config();
    GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void EXTI0_IRQHandler()
{
	LED_IRQhandler();
}

void NVIC_Config()
{
	NVIC_InitTypeDef NVIC_Initial={0};

	NVIC_Initial.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Initial.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_Initial);
}

static void LED_IRQhandler()
{
	if(EXTI_GetITStatus(EXTI_Line0 == SET))
	{
		if(LED == LED12)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
			LED = LED13;
		}
		else if(LED == LED13)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
			LED = LED14;
		}
		else if(LED == LED14)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_14);
			LED = LED15;
		}
		else if(LED == LED15)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_15);
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void externalIntConfig()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	EXTI_InitTypeDef extiInit={0};

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	extiInit.EXTI_Line = EXTI_Line0;
	extiInit.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInit.EXTI_Trigger = EXTI_Trigger_Rising;
	extiInit.EXTI_LineCmd = ENABLE;

	EXTI_Init(&extiInit);


}


void gpio_Init(GPIO_TypeDef* gpiox)
{
	GPIO_InitTypeDef gpioInitStuct={0};

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);


	gpioInitStuct.GPIO_Mode=GPIO_Mode_IN;
	gpioInitStuct.GPIO_Pin = GPIO_Pin_0;
	gpioInitStuct.GPIO_OType=GPIO_OType_PP;
	gpioInitStuct.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(GPIOA,&gpioInitStuct);


	gpioInitStuct.GPIO_Mode = GPIO_Mode_OUT;
	gpioInitStuct.GPIO_OType = GPIO_OType_PP;
	gpioInitStuct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	gpioInitStuct.GPIO_Speed = GPIO_Speed_100MHz;
	gpioInitStuct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD,&gpioInitStuct);


}


