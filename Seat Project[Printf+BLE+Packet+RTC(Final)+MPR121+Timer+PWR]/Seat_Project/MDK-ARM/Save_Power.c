#include "stm32f1xx_hal.h"
static volatile GPIO_InitTypeDef GPIO_InitStruct;

void Save_Power_Init(void){
	__HAL_RCC_PWR_CLK_ENABLE();
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull 		= GPIO_NOPULL; 					
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);		
		
	
}

void Enter_StopMode(void){
	
	printf("Enter_StopMode()\n");
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);						

	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void Exit_StopMode(void){
	printf("Exit_StopMode()\n");
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{	
		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);	
}