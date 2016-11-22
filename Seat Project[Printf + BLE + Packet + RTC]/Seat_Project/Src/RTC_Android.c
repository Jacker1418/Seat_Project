#include "stm32f1xx_hal.h"

static RTC_HandleTypeDef rtc_android;

extern void Error_Handler(void);

//void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc){

//	RCC_OscInitTypeDef        RCC_OscInitStruct;
//  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
//	
//  if(hrtc->Instance==RTC)
//  {
//		/*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
//    __HAL_RCC_PWR_CLK_ENABLE();
//		HAL_PWR_EnableBkUpAccess();

//		__HAL_RCC_RTC_ENABLE(); 
//		
//			HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x0F, 0);
//			HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
//	}
//}
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTC clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /*##-2- Configue LSE/LSI as RTC clock soucre ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE  
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)  
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
#else
#error Please select the RTC Clock source inside the main.h file
#endif /*RTC_CLOCK_SOURCE_LSE*/
  
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


//void RTC_AlarmConfig(){
//	
//  RTC_AlarmTypeDef alarm;
//	RTC_TimeTypeDef time;
//	
//  /*## Configure the RTC Alarm peripheral #################################*/
//	HAL_RTC_GetTime(&rtc_android, &time, RTC_FORMAT_BIN);
//	
//	//HAL_RTC_SetTime(&rtc_android, &time, RTC_FORMAT_BIN);
//	
//  alarm.Alarm = RTC_ALARM_A;
//	alarm.AlarmTime.Hours = time.Hours;
//  alarm.AlarmTime.Minutes = time.Minutes;
//  alarm.AlarmTime.Seconds = (time.Seconds + timeInterval) % 60;
//  
//  if(HAL_RTC_SetAlarm_IT(&rtc_android,&alarm,RTC_FORMAT_BCD) != HAL_OK)
//  {
//    /* Initialization Error */
//		printf("RTC_AlarmConfig() -> HAL_RTC_SetAlarm_IT()\n");
//    Error_Handler(); 
//  }
//}
void RTC_AlarmConfig(void)
{
  RTC_DateTypeDef  tempDate;
  RTC_TimeTypeDef  tempTime;
	RTC_DateTypeDef  originDate;
  RTC_TimeTypeDef  originTime;
  RTC_AlarmTypeDef tempAlarm;
 
	HAL_RTC_GetDate(&rtc_android, &originDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&rtc_android, &originTime, RTC_FORMAT_BCD);
	
  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
	
  tempDate.Year = originDate.Year;
  tempDate.Month = originDate.Month;
  tempDate.Date = originDate.Date;
  tempDate.WeekDay = originDate.WeekDay;
  
  if(HAL_RTC_SetDate(&rtc_android,&tempDate,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:20:00 */
  tempTime.Hours = originTime.Hours;
  tempTime.Minutes = originTime.Minutes ;
  tempTime.Seconds = originTime.Seconds;
  
  if(HAL_RTC_SetTime(&rtc_android,&tempTime,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }  

  /*##-3- Configure the RTC Alarm peripheral #################################*/
  /* Set Alarm to 02:20:30 
     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
  tempAlarm.Alarm = RTC_ALARM_A;
	tempAlarm.AlarmTime.Hours = originTime.Hours;
  tempAlarm.AlarmTime.Minutes = originTime.Minutes;
  tempAlarm.AlarmTime.Seconds = (originTime.Seconds + timeInterval) % 60;
  
  if(HAL_RTC_SetAlarm_IT(&rtc_android,&tempAlarm,RTC_FORMAT_BCD) != HAL_OK)
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

void RTC_getTime(struct Date* date){
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

void checkTime(volatile struct Date *androidTime){

	RTC_DateTypeDef tempDate;
	RTC_TimeTypeDef tempTime;
	
	tempDate.Year = androidTime->year;
	tempDate.Month = androidTime->month;
	tempDate.Date = androidTime->day;
	
	tempTime.Hours = androidTime->hour;
	tempTime.Minutes = androidTime->minute;
	
	HAL_RTC_SetDate(&rtc_android, &tempDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&rtc_android, &tempTime, RTC_FORMAT_BCD);
	
	RTC_AlarmConfig();
}
