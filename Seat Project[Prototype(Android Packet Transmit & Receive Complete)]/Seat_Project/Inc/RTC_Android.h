#ifndef __RTC_ANDROID_H__
#define __RTC_ANDROID_H__

#include "stm32f1xx_hal_def.h"

#define RTC_CLOCK_SOURCE_LSI

HAL_StatusTypeDef RTC_Android_Init(void);

void RTC_AlarmConfig(void);
void RTC_TimeShow(void);
void checkTime(volatile struct Date *androidTime);
void RTC_getTime(struct Date* date);
#endif
