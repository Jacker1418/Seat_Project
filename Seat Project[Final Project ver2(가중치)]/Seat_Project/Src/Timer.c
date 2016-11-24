#include "stm32f1xx_hal.h"

extern void Error_Handler(void);

static TIM_HandleTypeDef check_MPR121_Idle;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *timer){
	
}
void TIM2_IRQHandler(void)
{	
		HAL_TIM_IRQHandler(&check_MPR121_Idle);	
}

void CheckMPR121_TIM3_Init(void)
{
		__HAL_RCC_TIM2_CLK_ENABLE();
	
  check_MPR121_Idle.Instance = TIM2;
  check_MPR121_Idle.Init.Prescaler = 7999;
  check_MPR121_Idle.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  check_MPR121_Idle.Init.Period = 999;
  check_MPR121_Idle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&check_MPR121_Idle) != HAL_OK)
  {
    Error_Handler();
  }

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

}

void startTimer(void){
	
	HAL_TIM_Base_Start_IT(&check_MPR121_Idle);
}

void stopTimer(void){
	HAL_TIM_Base_Stop_IT(&check_MPR121_Idle);
	//CLEAR_BIT(check_MPR121_Idle.Instance->SR, TIM_SR_UIF  );
}

