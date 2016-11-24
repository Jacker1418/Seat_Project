#include "stm32f1xx_hal.h"

static TIM_HandleTypeDef duringPush;
static TIM_HandleTypeDef controlLed;

static TIM_OC_InitTypeDef controlLed_OC;

static 	GPIO_InitTypeDef button;

HAL_StatusTypeDef Indicator_Init(void){
	
	GPIO_InitTypeDef button;
	GPIO_InitTypeDef pinPWM;
	HAL_StatusTypeDef result;

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	
	pinPWM.Pin = GPIO_PIN_8;
	pinPWM.Mode = GPIO_MODE_AF_PP;
	pinPWM.Pull = GPIO_PULLUP;
	pinPWM.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &pinPWM);
	
	controlLed.Instance = TIM4;
	controlLed.Init.CounterMode = TIM_COUNTERMODE_UP;
	controlLed.Init.Prescaler = 0;
	controlLed.Init.Period = 9999;
	controlLed.Init.ClockDivision = 0;
	
	controlLed_OC.OCMode = TIM_OCMODE_PWM1;
	controlLed_OC.Pulse = 4000;
	controlLed_OC.OCPolarity = TIM_OCPOLARITY_HIGH;
	
	if((result = HAL_TIM_PWM_Init(&controlLed)) != HAL_OK){
		printf("HAL_TIM_PWM_Init() Error\n");
		return result;
	}
	
	HAL_TIM_PWM_ConfigChannel(&controlLed, &controlLed_OC, TIM_CHANNEL_3);
	return HAL_OK;
}


void startPWM(void){
	controlLed_OC.Pulse = 4000;
	
	HAL_TIM_PWM_ConfigChannel(&controlLed, &controlLed_OC, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&controlLed, TIM_CHANNEL_3);
}

void stopPWM(void){
	controlLed_OC.Pulse = 1;
	
	HAL_TIM_PWM_ConfigChannel(&controlLed, &controlLed_OC, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(&controlLed, TIM_CHANNEL_3);
}

