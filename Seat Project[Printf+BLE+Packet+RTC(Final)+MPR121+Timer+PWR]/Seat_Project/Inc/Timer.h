#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f1xx_hal_def.h"

void CheckMPR121_TIM3_Init(void);

void startTimer(void);
void stopTimer(void);

#endif
