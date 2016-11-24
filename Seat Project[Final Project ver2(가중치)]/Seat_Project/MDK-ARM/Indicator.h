#ifndef __INDICATOR_H__
#define __INDICATOR_H__

#include "stm32f1xx_hal_def.h"

typedef enum{
	Interrupt,
	Button
}funcButton;

HAL_StatusTypeDef Indicator_Init(void);

void startPWM(void);
void stopPWM(void);

#endif
