#include "stm32f1xx_hal.h"

static ADC_HandleTypeDef battery_check;
static ADC_ChannelConfTypeDef channelADC_Battery;
static GPIO_InitTypeDef GPIO_Initstruct;

void HAL_ADC_MspInit(ADC_HandleTypeDef *adc){
	
	if(adc->Instance == ADC1){
		
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_Initstruct.Pin = GPIO_PIN_6;
		GPIO_Initstruct.Mode = GPIO_MODE_ANALOG;
		
		HAL_GPIO_Init(GPIOA, &GPIO_Initstruct);

	}
}
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{

  if(hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
  }
}
HAL_StatusTypeDef Battery_Check_Init(void){
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	battery_check.Instance = ADC1;
	battery_check.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	battery_check.Init.ScanConvMode = ADC_SCAN_DISABLE;
	battery_check.Init.ContinuousConvMode = DISABLE;
	battery_check.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	battery_check.Init.NbrOfConversion = 1;
	
	if(HAL_ADC_Init(&battery_check) != HAL_OK){
		printf("HAL_ADC_Init() Error\n");
	}
	
	channelADC_Battery.Channel = ADC_CHANNEL_6;
	channelADC_Battery.Rank = ADC_REGULAR_RANK_1;
	channelADC_Battery.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	
	if(HAL_ADC_ConfigChannel(&battery_check, &channelADC_Battery) != HAL_OK){
		printf("HAL_ADC_ConfigChannel() Error\n");
	}
	
	HAL_ADCEx_Calibration_Start(&battery_check);
}

uint8_t getBattery_Capacity(void){
	int result = 0;
	
	HAL_ADC_Start(&battery_check);
	
	if(HAL_ADC_PollForConversion(&battery_check, 1000) != HAL_OK){
		printf("HAL_ADC_PollForConversion() Error\n");
	}
	
	result = (int)((((float)(HAL_ADC_GetValue(&battery_check) * 2) - MIN_CAPACITY) / (MAX_CAPACITY - MIN_CAPACITY)) * 100);
	if(result > 100){
		result = 101;
	}else if(result <= 0){
		result = 0;
	}
	
	return (uint8_t)result;
}

