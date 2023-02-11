/*
 * myLib.h
 *
 *  Created on: 8 Eki 2022
 *      Author: Ali
 */

#ifndef STM32F4XX_STDPERIPH_DRIVER_INC_MYLIB_H_
#define STM32F4XX_STDPERIPH_DRIVER_INC_MYLIB_H_


#include "stm32f4xx.h"
#include <string.h>


#define delay_ms() while(!TIM_GetCounter(TIM3))
#define LED_Config

// #define LED_Config

#define NoImportent 1
#define is_PLL_Clk 0x08

uint16_t ADC_Value;
uint32_t pulse ;

TIM_TimeBaseInitTypeDef tim_init = {0};
RCC_ClocksTypeDef Rcc;

// delay  ---->> 1ms
void NVIC_Config();
void externalIntConfig();
 void delay_config();
 void Error_Handler();
 void RCC_Config();
 void GPIO_Config();
 void ADC_Config();
 void PWM_Config();
 uint16_t ADC_GetValue();
 uint16_t map(uint32_t in , uint32_t in_min, uint32_t in_max, uint32_t out_min,uint32_t out_max);
 void StartConfig();
 void RunningServo();
 void waitingForInterrupt(void);


#endif /* STM32F4XX_STDPERIPH_DRIVER_INC_MYLIB_H_ */
