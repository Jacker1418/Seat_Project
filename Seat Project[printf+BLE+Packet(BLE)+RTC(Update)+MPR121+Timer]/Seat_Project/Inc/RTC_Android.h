#ifndef __RTC_ANDROID_H__
#define __RTC_ANDROID_H__

#include "stm32f1xx_hal_def.h"

#define RTC_CLOCK_SOURCE_LSI

#define timeInterval 60

typedef enum{
	Time_NULL = 0x00,
	Time_Year_Wrong = 0x01,
	Time_Month_Wrong = 0x02,
	Time_Day_Wrong = 0x04,
	Time_Hour_Wrong = 0x08,
	Time_Minute_Wrong = 0x10,
	Time_Second_Wrong = 0x20,
	Time_OK = 0x80,
}Time_Status;

HAL_StatusTypeDef RTC_Android_Init(void);

void RTC_AlarmConfig(struct Date* date);
void RTC_TimeShow(void);
Time_Status checkTime(volatile struct Date *androidTime);

void RTC_getTime(volatile struct Date* date);
void addDate(struct Date* date1, uint8_t interval, volatile struct Date* result);
void addTime(RTC_TimeTypeDef* date1, uint8_t interval);
void getAlarmDiff(struct Date* date1, struct Date* date2, volatile struct Date* result);
void getDateDiff(struct Date* recvTime, struct Date* origin, volatile struct Date* result);
void convertRTCToDate(RTC_DateTypeDef* date, RTC_TimeTypeDef* time, volatile struct Date* result);
void modifyTime(volatile struct Date* destination, volatile struct Date* source);
#endif
