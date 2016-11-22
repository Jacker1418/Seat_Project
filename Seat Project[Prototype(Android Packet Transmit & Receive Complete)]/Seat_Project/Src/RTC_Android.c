#include "stm32f1xx_hal.h"

static RTC_HandleTypeDef rtc_android;

extern void Error_Handler(void);

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc){

	RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
	
  if(hrtc->Instance==RTC)
  {
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();

		/*##-2- Configue LSE/LSI as RTC clock soucre ###############################*/

		RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI ;
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

			HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0F, 0);
			HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
		}
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc){

  if(hrtc->Instance==RTC)
  {
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  }
}

void RTC_Alarm_IRQHandler(void){
	printf("RTC_Alarm_IRQHandler()\n");
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
	
	result = HAL_RTC_Init(&rtc_android) ;
  if (result != HAL_OK)
  {
    return result;
  }
	
	/*##-1- Configure the Date #################################################*/
  /* Set Date: RTC_MONTH_NOVEMBER 09 2016 */
  sdatestructure.Year = 0x16;
  sdatestructure.Month = RTC_MONTH_NOVEMBER;
  sdatestructure.Date = 0x09;
  
	result = HAL_RTC_SetDate(&rtc_android,&sdatestructure,RTC_FORMAT_BCD);
  if(result != HAL_OK)
  {
    return result;
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time:18:00:00 */
  stimestructure.Hours = 0x18;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  
	result = HAL_RTC_SetTime(&rtc_android,&stimestructure,RTC_FORMAT_BCD);
  if(result!= HAL_OK)
  {
    return result;
  } 
	
	return result;
}


void RTC_AlarmConfig(void){
	
  RTC_AlarmTypeDef alarm;
 
  /*## Configure the RTC Alarm peripheral #################################*/
  /* Set Alarm to 02:20:30 
     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  alarm.Alarm = RTC_ALARM_A;
	alarm.AlarmTime.Hours = 0x02;
  alarm.AlarmTime.Minutes = 0x20;
  alarm.AlarmTime.Seconds = 0x30;
  
  if(HAL_RTC_SetAlarm_IT(&rtc_android,&salarmstructure,RTC_FORMAT_BCD) != HAL_OK)
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
  printf("%02d:%02d:%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
} 

void RTC_getTime(struct Date* date){
	RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&rtc_android, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&rtc_android, &sdatestructureget, RTC_FORMAT_BIN);
	
	date->year = sdatestructureget.Year;
	date->month = sdatestructureget.Month;
	date->day = sdatestructureget.Date;
	
	date->hour = stimestructureget.Hours;
	date->minute = stimestructureget.Minutes;
}

void checkTime(volatile struct Date *androidTime){

	RTC_DateTypeDef tempDate;
	RTC_TimeTypeDef tempTime;
	
	tempDate.Year = androidTime->year;
	tempDate.Month = androidTime->month;
	tempDate.Date = androidTime->day;
	
	tempTime.Hours = androidTime->hour;
	tempTime.Minutes = androidTime->minute;
	
	HAL_RTC_SetDate(&rtc_android, &tempDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&rtc_android, &tempTime, RTC_FORMAT_BIN);
}
