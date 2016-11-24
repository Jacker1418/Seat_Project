#ifndef __BATTERY_CHECK_H__
#define __BATTERY_CHECK_H__

#include "stm32f1xx_hal_def.h"

#define MAX_CAPACITY 4700 												//Battery Voltage 3.8V
#define MIN_CAPACITY	2970													//Battery Voltage 2.4V

#define SAFE_POWER_LEVEL 5

HAL_StatusTypeDef Battery_Check_Init(void);

uint8_t getBattery_Capacity(void);
#endif
