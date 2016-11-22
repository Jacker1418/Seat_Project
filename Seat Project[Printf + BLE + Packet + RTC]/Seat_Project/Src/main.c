/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
//#include "Uart.h"
#include "I2C_MPR121.h"

/* Private variables ---------------------------------------------------------*/
//############## Bluetooth ################ //
uint8_t TxBuffer[] = "Hello World\r";
volatile uint8_t RxBuffer[UART_BUFFER_SIZE];
 uint8_t tempBuffer[UART_BUFFER_SIZE];

volatile struct Packet recvPacket;
volatile struct Packet transPacket;

volatile uint8_t flagBluetooth_Recv = myUNSET;

//############## I2C MPR121 ############## //
volatile uint16_t arrSensor_Value[COUNT_KEY];

//############## Real Time Clock ########### //
RTC_HandleTypeDef rtc;

RTC_DateTypeDef setDate;
RTC_TimeTypeDef setTime;

volatile uint8_t flagRTC_Android = myUNSET;
//############## Private ################## //
volatile HAL_StatusTypeDef result;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
void checkTime(volatile struct Date* androidTime);

int main(void)
{
	__IO int index = 0;
  /*  MCU Configuration */
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
	UART_Console_Init();
	UART_Bluetooth_Init();
	
	//I2C_MPR121_Init();
	
//	result = MPR121_Configuration();
//	if(result != HAL_OK){
//		Error_Handler();
//	}
	
	RTC_Android_Init();
	
	RTC_AlarmConfig();
	
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
			initializePacket(&transPacket);
			
			transPacket.mode = recvPacket.mode;
			
			if(transPacket.mode == Normal){
				RTC_getTime(&transPacket.date);
			}
			
			extractiSensorValue(transPacket.sensorValue);
			
		  //get Position 함수가 필요하다 하지만 지금은 만들지 않았다.
			
			//대대적인 함수 수정 요망
			//makePacket(&transPacket); 
			//result = Bluetooth_Send_Polling(tempBuffer, 1000) ;
			
			result = Bluetooth_Send_Packet_Polling(&transPacket, 1000);
			if(result != HAL_OK){
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
	__IO uint16_t index = 0;
	Packet_Status resultPacket;
	
	RxBuffer[UartHandle->RxCount] = '\0';
	strcpy(tempBuffer, RxBuffer);
//	
//  for(index = 0; tempBuffer[index] != '\0' ; index++){
//		printf("%x ",tempBuffer[index]);
//	}
//	tempBuffer[index] = END;
//	printf("\n");
//	
	resultPacket = releasePacket(&recvPacket, tempBuffer, UartHandle->RxCount);
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
			checkTime(&recvPacket.date);
			
		break;
	}
	printPacket(&recvPacket);
	flagBluetooth_Recv = mySET;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	RTC_TimeShow();
	RTC_AlarmConfig();
}


/* System Clock Configuration*/
//void SystemClock_Config(void){

//  RCC_OscInitTypeDef RCC_OscInitStruct;
//  RCC_ClkInitTypeDef RCC_ClkInitStruct;
//  RCC_PeriphCLKInitTypeDef PeriphClkInit;

//  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
//  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//  RCC_OscInitStruct.HSICalibrationValue = 16;
//  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
//                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
//  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
//  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
//  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
//  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//  {
//    Error_Handler();
//  }

//  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

//  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

//  /* SysTick_IRQn interrupt configuration */
//  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Configure PLL ------------------------------------------------------*/
  /* PLL configuration: PLLCLK = (HSI / 2) * PLLMUL = (8 / 2) * 16 = 64 MHz */
  /* PREDIV1 configuration: PREDIV1CLK = PLLCLK / HSEPredivValue = 64 / 1 = 64 MHz */
  /* Enable HSI and activate PLL with HSi_DIV2 as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSI;
  oscinitstruct.HSEState        = RCC_HSE_OFF;
  oscinitstruct.LSEState        = RCC_LSE_OFF;
  oscinitstruct.HSIState        = RCC_HSI_ON;
  oscinitstruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  oscinitstruct.HSEPredivValue    = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSI_DIV2;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

void Error_Handler(void){
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  if(result == HAL_BUSY){
		printf("HAL_BUSY\n");
	}else if(result == HAL_ERROR){
		printf("HAL_ERROR\n");
	}else if(result == HAL_TIMEOUT){
		printf("HAL_TIMEOUT\n");
	}
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

