/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
//#include "Uart.h"
#include "I2C_MPR121.h"

/* Private variables ---------------------------------------------------------*/
//############## Bluetooth ################ //
uint8_t TxBuffer[] = "Hello World\n";
volatile uint8_t RxBuffer[UART_BUFFER_SIZE];
 uint8_t tempBuffer[UART_BUFFER_SIZE];

volatile struct Packet recvPacket;
volatile struct Packet *transPacket;

volatile uint8_t flagBluetooth_Recv = myUNSET;

//############## I2C MPR121 ############## //
volatile uint16_t arrSensor_Value[COUNT_KEY];

//############## Real Time Clock ########### //
RTC_HandleTypeDef rtc;

RTC_DateTypeDef setDate;
RTC_TimeTypeDef setTime;
//############## Private ################## //
volatile HAL_StatusTypeDef result;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void checkTime(volatile struct Date* androidTime);

int main(void)
{
  /*  MCU Configuration */
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
	UART_Console_Init();
	UART_Bluetooth_Init();
	
	I2C_MPR121_Init();
	
//	result = MPR121_Configuration();
//	if(result != HAL_OK){
//		Error_Handler();
//	}
	
	printf("Hello World\r\n");

  result = Bluetooth_Send_Polling(TxBuffer, 1000) ;
	if(result != HAL_OK){
		printf("Bluetooth_Send_Polling()\n");
		Error_Handler();
	}
	result = Bluetooth_Receive_IT(RxBuffer, 64) ;
	if(result != HAL_OK){
		printf("Bluetooth_Receive_IT()\n");
		Error_Handler();
	}
  while (1)
  {
//		
//		//MPR121_Receive_1byte_Polling(&touchState, 0x00, 1000);
//		
//		result = MPR121_Receive_All_Key_Polling(arrSensor_Value, 2000) ;
//		if(result != HAL_OK){
//			Error_Handler();
//		}else{
//			
//			//printf("%x\n",arrSensor_Value[0]);
//			
//			printf("===== Sensor Value =====\n");
//			for(index = 0; index < COUNT_KEY; index++){
//				if(index != 0 && index % 3 == 0){
//					printf("\n");
//				}
//				printf("%-3d ", arrSensor_Value[index]);
//			}
//			printf("\n");
//		}
//		
		//############# Bluetooth #################//
		if(flagBluetooth_Recv == mySET){
			
			result = Bluetooth_Send_Polling(tempBuffer, 1000) ;
			if(result != HAL_OK){
				printf("Bluetooth_Send_Polling()\n");
				Error_Handler();
			}
			
			result = Bluetooth_Receive_IT(RxBuffer, UART_BUFFER_SIZE) ;
			if(result != HAL_OK){
				if(result == HAL_BUSY){
					//Non
					printf(" USART is BUSY\n");
				}else{
					printf("Bluetooth_Receive_IT()\n");
					Error_Handler();
				}
			}
			flagBluetooth_Recv = myUNSET;
		}
		
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
  /* Set transmission flag: transfer complete */
	int i = 0;
	__IO uint16_t index = 0;
	Packet_Status resultPacket;
	
	
	//printf("HAL_UART_RxCpltCallback\n%s\n", RxBuffer);
	printf("RxCount : %d\n", UartHandle->RxCount);
	
	RxBuffer[UartHandle->RxCount] = '\0';
	strcpy(tempBuffer, RxBuffer);
	
  for(i = 0; tempBuffer[i] != '\0'; i++){
		printf("%x ",tempBuffer[i]);
	}
	tempBuffer[i] = END;
	printf("\n");
	/*
	resultPacket = releasePacket(&recvPacket, tempBuffer, index);
	switch(resultPacket){
		case Packet_NULL:
			printf("Packet_NULL\n");
		break;
		case Message_NULL:
			printf("Message_NULL\n");
		break;
		case Packet_Dropout:
			printf("Packet_Dropout\n");
		break;
		case Packet_Ok:
			printf("Packet_Ok\n");
		break;
	}
	checkTime(&recvPacket.date);
	
	printPacket(&recvPacket);
	*/
	flagBluetooth_Recv = mySET;
}


/* System Clock Configuration*/
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



void Error_Handler(void){
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  if(result == HAL_BUSY){
		printf("HAL_BUSY\n");
	}else if(result == HAL_ERROR){
		printf("HAL_ERROR\n");
		
		if(mpr121.ErrorCode == HAL_I2C_ERROR_NONE){
			printf("Error Code : HAL_I2C_ERROR_NONE\n");
		}else if(mpr121.ErrorCode == HAL_I2C_ERROR_BERR){
			printf("Error Code : HAL_I2C_ERROR_BERR\n");
		}else if(mpr121.ErrorCode == HAL_I2C_ERROR_ARLO){
			printf("Error Code : HAL_I2C_ERROR_ARLO\n");
		}else if(mpr121.ErrorCode == HAL_I2C_ERROR_AF){
			printf("Error Code : HAL_I2C_ERROR_AF\n");
		}else if(mpr121.ErrorCode == HAL_I2C_ERROR_TIMEOUT){
			printf("Error Code : HAL_I2C_ERROR_TIMEOUT\n");
		}
		
	}else if(result == HAL_TIMEOUT){
		printf("HAL_TIMEOUT\n");
	}
}

void checkTime(volatile struct Date *androidTime){

	RTC_DateTypeDef tempDate;
	RTC_TimeTypeDef tempTime;
	
	tempDate.Year = androidTime->year;
	tempDate.Month = androidTime->month;
	tempDate.Date = androidTime->day;
	
	tempTime.Hours = androidTime->hour;
	tempTime.Minutes = androidTime->minute;
	
	HAL_RTC_SetDate(&rtc, &tempDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&rtc, &tempTime, RTC_FORMAT_BIN);
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

	//printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}

#endif

