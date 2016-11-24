#include "stm32f1xx_hal.h"

static RTC_HandleTypeDef rtc_android;

extern void Error_Handler(void);

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  /*##-1- Enables the PWR Clock and Enables access to the backup domain ##########*/
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /*##-2- Configue LSE/LSI as RTC clock soucre ###############################*/

  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  /*##-2- Enable RTC peripheral Clocks #######################################*/ 
  /* Enable RTC Clock */ 
  __HAL_RCC_RTC_ENABLE(); 
  
  /*##-4- Configure the NVIC for RTC Alarm ###################################*/
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc){

  if(hrtc->Instance==RTC)
  {
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  }
}

void RTC_Alarm_IRQHandler(void){
  HAL_RTC_AlarmIRQHandler(&rtc_android);
}

HAL_StatusTypeDef RTC_Android_Init(void){
	HAL_StatusTypeDef result;
	
	RTC_DateTypeDef  sdatestructure;
  RTC_TimeTypeDef  stimestructure;
	
	/*##-1- Configure the RTC peripheral #######################################*/
  rtc_android.Instance = RTC;
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Asynch Prediv  = Automatic calculation of prediv for 1 sec timebase
  */

  rtc_android.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	rtc_android.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	result = HAL_RTC_Init(&rtc_android) ;
  if (result != HAL_OK)
  {
		printf("RTC_Android_Init() -> HAL_RTC_Init()\n");
		Error_Handler(); 
    return result;
  }
	
	/*##-1- Configure the Date #################################################*/
  /* Set Date: November 09th 2016 */
  sdatestructure.Year = 0x16;
  sdatestructure.Month = RTC_MONTH_NOVEMBER;
  sdatestructure.Date = 0x09;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
	result = HAL_RTC_SetDate(&rtc_android,&sdatestructure,RTC_FORMAT_BCD);
  if(result != HAL_OK)
  {
		printf("RTC_Android_Init() -> HAL_RTC_SetDate()\n");
		Error_Handler(); 
    return result;
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:20:00 */
  stimestructure.Hours = 0x18;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  
	result = HAL_RTC_SetTime(&rtc_android,&stimestructure,RTC_FORMAT_BCD);
  if(result!= HAL_OK)
  {
		printf("RTC_Android_Init() -> HAL_RTC_SetTime()\n");
		Error_Handler(); 
    return result;
  } 
	
	return result;
}

void RTC_AlarmConfig(struct Date* date)
{
  RTC_DateTypeDef  tempDate;
  RTC_TimeTypeDef  tempTime;
	RTC_AlarmTypeDef tempAlarm;
	
	RTC_DateTypeDef  originDate;
  RTC_TimeTypeDef  originTime;

 
	HAL_RTC_GetDate(&rtc_android, &originDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&rtc_android, &originTime, RTC_FORMAT_BIN);
	
  /*##-1- Configure the Date #################################################*/
	
  tempDate.Year = originDate.Year;
  tempDate.Month = originDate.Month;
  tempDate.Date = originDate.Date;
  tempDate.WeekDay = originDate.WeekDay;
  
  if(HAL_RTC_SetDate(&rtc_android,&tempDate,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  } 
  
  /*##-2- Configure the Time #################################################*/
  tempTime.Hours = originTime.Hours;
  tempTime.Minutes = originTime.Minutes ;
  tempTime.Seconds = originTime.Seconds;
  
  if(HAL_RTC_SetTime(&rtc_android,&tempTime,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }  

  /*##-3- Configure the RTC Alarm peripheral #################################*/
  /* RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
	
	/*
	2가지 호출 트리를 가진다.
	* 1. checkTime()에서 넘어오는 경우
	이 경우에는 매개변수로는 알람이 얼만큼 남았었는지 여부이다. 즉, 수정되는 시간에 맞춰 남은 알람시간만큼 더하여 동작하도록
	하기 위함이다.
	* 2. HAL_RTC_AlarmAEventCallback()에서 넘어오는 경우
	이 경우에는 일반적인 호출트리로 RTC Alarm이 호출된 이후 다시 등록하기 위함이며, 인자로 NULL를 넘겨줌으로써 일반 등록임을 
	알려준다.
	*/
  tempAlarm.Alarm = RTC_ALARM_A;
	if(date == NULL){
		addTime(&originTime, timeInterval);
		tempAlarm.AlarmTime.Hours = originTime.Hours;
		tempAlarm.AlarmTime.Minutes = originTime.Minutes;
		tempAlarm.AlarmTime.Seconds = originTime.Seconds;
	}else{
		if(date->sign == positive){
			addTime(&originTime, timeInterval);
			tempAlarm.AlarmTime.Hours = originTime.Hours ;
			tempAlarm.AlarmTime.Minutes = originTime.Minutes ;
			tempAlarm.AlarmTime.Seconds = originTime.Seconds;
		}else{
			tempAlarm.AlarmTime.Hours = originTime.Hours + date->hour;
			tempAlarm.AlarmTime.Minutes = originTime.Minutes + date->minute;
			tempAlarm.AlarmTime.Seconds = originTime.Seconds + date->second;
		}
	}
	
  if(HAL_RTC_SetAlarm_IT(&rtc_android,&tempAlarm,RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
}
void RTC_TimeShow(void){
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&rtc_android, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&rtc_android, &sdatestructureget, RTC_FORMAT_BIN);
	
	
  /* Display time Format : hh:mm:ss */
  printf("%4d.%02d.%02d %02d:%02d:%02d\n"
					,sdatestructureget.Year + 2000, sdatestructureget.Month, sdatestructureget.Date
					,stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
	
} 

void RTC_getTime(volatile struct Date* date){
	RTC_DateTypeDef originDate;
  RTC_TimeTypeDef originTime;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&rtc_android, &originTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&rtc_android, &originDate, RTC_FORMAT_BIN);
	
	date->year = originDate.Year;
	date->month = originDate.Month;
	date->day = originDate.Date;
	
	date->hour = originTime.Hours;
	date->minute = originTime.Minutes;
}

Time_Status checkTime(volatile struct Date *androidTime){

	RTC_DateTypeDef tempDate;
	RTC_TimeTypeDef tempTime;
	
	RTC_DateTypeDef originDate;
	RTC_TimeTypeDef originTime;
	RTC_AlarmTypeDef originAlarm;
	
	struct Date origin;
	struct Date temp;
	struct Date alarm;
	struct Date resultForAlarm;
	struct Date resultForPacket;
	
	if(androidTime->year < 0){
		return Time_Year_Wrong;
	}else{
		tempDate.Year = androidTime->year;
	}
	
	if(androidTime->month > 12){
		return Time_Month_Wrong;
	}else{
		tempDate.Month = androidTime->month;
	}
	
	if( androidTime->day > 31){
		return Time_Day_Wrong;
	}else{
		tempDate.Date = androidTime->day;
	}
	
	if(androidTime->hour > 23){
		return Time_Hour_Wrong;
	}else{
		tempTime.Hours = androidTime->hour;
	}
	
	if(androidTime->minute > 59){
		return Time_Month_Wrong;
	}else{
		tempTime.Minutes = androidTime->minute;
	}
	
	if(androidTime->second > 59){
		return Time_Second_Wrong;
	}else{
		tempTime.Seconds = androidTime->second;
	}
	
	convertRTCToDate(&tempDate, &tempTime, &temp);

	//현재 시간 구하기
	HAL_RTC_GetDate(&rtc_android, &originDate, RTC_FORMAT_BIN);	
	HAL_RTC_GetTime(&rtc_android, &originTime, RTC_FORMAT_BIN);
	convertRTCToDate(&originDate, &originTime, &origin);

	HAL_RTC_GetAlarm(&rtc_android, &originAlarm, 0, RTC_FORMAT_BIN);
	convertRTCToDate(&originDate, &originAlarm.AlarmTime, &alarm);
	
	//알람과 Packet의 Date를 변경하기 위하여 현재시간과의 차이를 구한다.
	clearDate(&resultForAlarm);
	clearDate(&resultForPacket);
	
	getDateDiff(&alarm , &origin, &resultForAlarm);
	getDateDiff(&temp, &origin, &resultForPacket);
	
	//Packet의 데이터들을 변경한다.
	
	checkTime_Items(&resultForPacket);
	
	HAL_RTC_SetDate(&rtc_android, &tempDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&rtc_android, &tempTime, RTC_FORMAT_BIN);
	
	RTC_AlarmConfig(&resultForAlarm);
	
	return Time_OK;
}

void addDate(struct Date* date1, uint8_t interval, volatile struct Date* result){
	
	uint32_t totalSecond = 0;
	uint32_t dayToSecond = 24 * 60 * 60;
	uint32_t hourToSecond = 60 * 60;
	uint32_t minuteToSecond = 60;
	
  totalSecond = (date1->day * dayToSecond) + (date1->hour * hourToSecond) + (date1->minute * minuteToSecond) + date1->second;
	
	totalSecond += interval;
	
	result->day = totalSecond / dayToSecond;
	result->hour = (totalSecond % dayToSecond) / hourToSecond;
	result->minute = (totalSecond % hourToSecond) / minuteToSecond;
	result->second = totalSecond % minuteToSecond;
}

void addTime(RTC_TimeTypeDef* date1, uint8_t interval){
	
	uint32_t totalSecond = 0;
	uint32_t dayToSecond = 24 * 60 * 60;
	uint32_t hourToSecond = 60 * 60;
	uint32_t minuteToSecond = 60;
	
  totalSecond = (date1->Hours * hourToSecond) + (date1->Minutes * minuteToSecond) + date1->Seconds;
	
	totalSecond += interval;
	
	date1->Hours = (totalSecond % dayToSecond) / hourToSecond;
	date1->Minutes = (totalSecond % hourToSecond) / minuteToSecond;
	date1->Seconds = totalSecond % minuteToSecond;
}
//void getAlarmDiff(struct Date* alarm, struct Date* origin, volatile struct Date* result){
//	
//	uint32_t dayToSecond = 24 * 60 * 60;
//	uint32_t hourToSecond = 60 * 60;
//	uint32_t minuteToSecond = 60;
//	
//	int totalSecond1 = 0;
//	int totalSecond2 = 0;
//	int resultSeconds = 0;
//	
//	totalSecond1 = (alarm->day * dayToSecond) + (alarm->hour * hourToSecond) + (alarm->minute * minuteToSecond) + alarm->second;
//	totalSecond2 = (origin->day * dayToSecond) + (origin->hour * hourToSecond) + (origin->minute * minuteToSecond) + origin->second;

//	resultSeconds = totalSecond2 - totalSecond1;
//	if(resultSeconds < 0 ){
//		result->sign = negative;
//	}else{
//		result->sign = positive;
//	}
//	resultSeconds = abs(resultSeconds);
//	
//	result->day = resultSeconds / dayToSecond;
//	result->hour = (resultSeconds % dayToSecond) / hourToSecond;
//	result->minute = (resultSeconds % hourToSecond) / minuteToSecond;
//	result->second = resultSeconds % minuteToSecond;
//}
void getDateDiff(struct Date* recvTime, struct Date* origin, volatile struct Date* result){
	
	uint32_t dayToSecond = 24 * 60 * 60;
	uint32_t hourToSecond = 60 * 60;
	uint32_t minuteToSecond = 60;
	
	int totalSecond1 = 0;
	int totalSecond2 = 0;
	int resultSeconds = 0;
	
	totalSecond1 = (recvTime->day * dayToSecond) + (recvTime->hour * hourToSecond) + (recvTime->minute * minuteToSecond) + recvTime->second;
	totalSecond2 = (origin->day * dayToSecond) + (origin->hour * hourToSecond) + (origin->minute * minuteToSecond) + origin->second;

	resultSeconds = totalSecond1 - totalSecond2;
	if(resultSeconds < 0 ){
		result->sign = negative;
	}else{
		result->sign = positive;
	}
	resultSeconds = abs(resultSeconds);
	
	result->day = resultSeconds / dayToSecond;
	result->hour = (resultSeconds % dayToSecond) / hourToSecond;
	result->minute = (resultSeconds % hourToSecond) / minuteToSecond;
	result->second = resultSeconds % minuteToSecond;

}

void convertRTCToDate(RTC_DateTypeDef* date, RTC_TimeTypeDef* time, volatile struct Date* result){

	if(date != NULL){
		result->year = date->Year;
		result->month = date->Month;
		result->day = date->Date;
	}else{
		result->year = 0;
		result->month = 0;
		result->day = 0;
	}
	if(time != NULL){
		result->hour = time->Hours;
		result->minute = time->Minutes;
		result->second = time->Seconds;
	}else{
		result->hour = 0;
		result->minute = 0;
		result->second = 0;
	}
}

void modifyTime(volatile struct Date* destination, volatile struct Date* source){
	uint32_t dayToSecond = 24 * 60 * 60;
	uint32_t hourToSecond = 60 * 60;
	uint32_t minuteToSecond = 60;
	
	int totalSecond1 = 0;
	int totalSecond2 = 0;
	int resultSeconds = 0;
	
	totalSecond1 = (destination->day * dayToSecond) + (destination->hour * hourToSecond) + (destination->minute * minuteToSecond) + destination->second;
	totalSecond2 = (source->day * dayToSecond) + (source->hour * hourToSecond) + (source->minute * minuteToSecond) + source->second;

	if(source->sign == positive){
		resultSeconds = totalSecond1 + totalSecond2;
	}else{
		resultSeconds = totalSecond1 - totalSecond2;
	}
	
	destination->day = resultSeconds / dayToSecond;
	destination->hour = (resultSeconds % dayToSecond) / hourToSecond;
	destination->minute = (resultSeconds % hourToSecond) / minuteToSecond;
	destination->second = resultSeconds % minuteToSecond;
}

void printDate(struct Date* date){
	
	printf("%4d. %2d. %2d  %2d:%2d:%2d\n",date->year, date->month, date->day, date->hour, date->minute, date->second);
}
void clearDate(struct Date* date){
	date->year = 0;
	date->month = 0;
	date->day = 0;
	date->hour = 0;
	date->minute = 0;
	date->second = 0;
}
