/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Private variables ---------------------------------------------------------*/
//############## Bluetooth ################ //
uint8_t TxBuffer[] = "Start Program";
volatile uint8_t RxBuffer[UART_BUFFER_SIZE];
 uint8_t tempBuffer[UART_BUFFER_SIZE];

volatile struct Packet recvPacket;
volatile struct Packet transPacket;

volatile uint8_t flagBluetooth_Recv = myUNSET;

//############## I2C MPR121 ############## //
volatile uint16_t arrSensor_Value[COUNT_KEY];
volatile uint8_t tempData;
volatile uint16_t tempTouchState;
volatile struct Touch_Data sensorData;
volatile struct Packet tempPacket;

volatile uint8_t flagMPR121 = myUNSET;

//############## Real Time Clock & Battery ########### //
RTC_DateTypeDef setDate;
RTC_TimeTypeDef setTime;

volatile uint8_t flagRTC_Android = myUNSET;

//##############  Timer ##################  //

volatile uint8_t flagCheck_MPR121 = myUNSET;

//############## Save Power ############# //
const uint8_t maxCountIdleState = 3;
volatile uint8_t countIdleState = 0;

volatile uint8_t flagStopMode = myUNSET;

// ############# Battery Check ############ //
volatile uint8_t battery_capacity = 0;
volatile uint8_t flagChargeBattery = myUNSET;

//############## Indicator ############### //
volatile uint8_t during_Push_Button = 0;
volatile uint8_t flagPushButon = myUNSET;

//############## Private ################## //
volatile HAL_StatusTypeDef result;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

int main(void)
{
	__IO int index = 0;
	__IO int max = 0;
	__IO Packet_Status resultPacket;
	
  /*  MCU Configuration */
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
	UART_Console_Init();
	Battery_Check_Init();
	UART_Bluetooth_Init();
	RTC_Android_Init();
	I2C_MPR121_Init();
	CheckMPR121_TIM3_Init();
	Save_Power_Init();
	Indicator_Init();

	startPWM();
	
	printf("Peripheral Configuration\n");

	MPR121_Configuration();
  RTC_AlarmConfig(NULL);

	result = Bluetooth_Receive_IT(RxBuffer, BUFFER_SIZE) ;
	if(result != HAL_OK){
		printf("Bluetooth_Receive_IT()\n");
		Error_Handler();
	}
	startTimer();
  
	printf("Start Program\n");

	while (1)
  {
		// ############ Power Save ############### //
		if(flagStopMode == mySET){
			flagStopMode = myUNSET;
			Enter_StopMode();
		}
		//############# Bluetooth #################//
		if(flagBluetooth_Recv == mySET){

			strcpy(tempBuffer, RxBuffer);

			resultPacket = releasePacket(&recvPacket, tempBuffer, strlen(tempBuffer));
			#ifdef DEBUG
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
			#endif
			
			printPacket(&recvPacket);
		  
			if(recvPacket.mode == Normal){
				switch(checkTime(&recvPacket.date)){
					case Time_OK:
					printf("checkTime()->Time_OK\n");
					break;
					case Time_NULL:
					printf("checkTime()->Time_NULL\n");
					break;
					case Time_Year_Wrong:
					printf("checkTime()->Time_Year_Wrong\n");
					break;
					case Time_Month_Wrong:
					printf("checkTime()->Time_Month_Wrong\n");
					break;
					case Time_Day_Wrong:
					printf("checkTime()->Time_Day_Wrong\n");
					break;
					case Time_Hour_Wrong:
					printf("checkTime()->Time_Hour_Wrong\n");
					break;
					case Time_Minute_Wrong:
					printf("checkTime()->Time_Minute_Wrong\n");
					break;
					case Time_Second_Wrong:
					printf("checkTime()->Time_Second_Wrong\n");
					break;
				}
								
				max = howManyItem();
				#ifdef DEBUG
				printf("max : %d\n", max);
				#endif
				for(index = 0; index < max; index++){
					
					Pop(&transPacket);
					
					result = Bluetooth_Send_Packet_Polling(&transPacket, 1000);
					if(result != HAL_OK){
						Error_Handler();
					}
				}
			}else if(recvPacket.mode == Real_Time){
				initializePacket(&transPacket);

				transPacket.mode = Real_Time;
				MPR121_Receive_All_Key_Polling(&sensorData, 1000);

				for(	index = 0; index < COUNT_KEY; index++){
					transPacket.sensorValue[index] = sensorData.sensorValue[index];
					#ifdef DEBUG
					if(index != 0 && index % 3 == 0){
						printf("\n");
					}
					printf("%3d\t", transPacket.sensorValue[index]);
					#endif
				}
				printf("\n");
		
				
				//insert Position
				transPacket.position[0] = sensorData.position.x * 100;
				transPacket.position[1] = sensorData.position.y * 100;
				
				result = Bluetooth_Send_Packet_Polling(&transPacket, 1000);
					if(result != HAL_OK){
						Error_Handler();
					}
				
			}else{
				
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
		}//flagBluetooth_Recv

		
			//############# RTC #################//
		if(flagRTC_Android == mySET){
			
				#ifdef DEBUG
				RTC_TimeShow();
				#endif
			
			result = MPR121_Receive_All_Key_Polling(&sensorData, 1000);
			if(result != HAL_OK){
				printf("MPR121_Receive_All_Key_Polling()\n");
				Error_Handler();
			}else{
				#ifdef DEBUG
				printf("Touch State : %X\n",sensorData.touchState);
				#endif
				if(sensorData.touchState != 0){
						//Packet Clear
					initializePacket(&tempPacket);
								
					//insert Mode
					tempPacket.mode = Normal;
						
					//insert Date
					RTC_getTime(&tempPacket.date);
						
					//insert Sensor Value
					for(	index = 1; index <= COUNT_KEY; index++){
						tempPacket.sensorValue[index - 1] = sensorData.sensorValue[index - 1];
						#ifdef DEBUG
						printf("%3d\t", tempPacket.sensorValue[index - 1]);
						if(index % 3 == 0){
							printf("\n");
						}
						#endif
					}
					printf("\n");
					
					//insert Position
					tempPacket.position[0] = sensorData.position.x * 100;
					tempPacket.position[1] = sensorData.position.y * 100;

					#ifdef DEBUG
					printf("Position : %d, %d\n", tempPacket.position[0], tempPacket.position[1]);
					#endif
					//############# ADC ################//
					battery_capacity = getBattery_Capacity();
					
					//Insert Battery Capacity
					tempPacket.battery = battery_capacity;
					
					//Push Queue
					Push(&tempPacket);
				}else{
						countIdleState++;
					#ifdef DEBUG
						printf("Count Idle State : %d\n", countIdleState);
					#endif
						if(countIdleState >= maxCountIdleState){
							#ifdef DEBUG
							printf("Do not seat Long Time\n");
							#endif
							flagStopMode = mySET;
							continue;
						}
				}
			}
			RTC_AlarmConfig(NULL);
			
			flagRTC_Android = myUNSET;
		}// flagRTC_Android
		
		//################## Timer ######################## //
		if(flagCheck_MPR121 == mySET){
			result = MPR121_Receive_All_Key_Polling(&sensorData, 1000);
			
			#ifdef DEBUG
			//RTC_TimeShow();
			printf("ADC : %d\n", getBattery_Capacity());
			#endif
			flagCheck_MPR121 = myUNSET;
			startTimer();
		}//falgCheck_MPR121

		//################### Indicator ##################### //
		if(flagPushButon == mySET && during_Push_Button == 5){
			flagStopMode = mySET;
			continue;
		}
	
  } // while(1)	
}// main()

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
	flagBluetooth_Recv = mySET;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	flagRTC_Android = mySET;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  {	
	if(htim->Instance == TIM2){
		stopTimer();
		flagCheck_MPR121 = mySET;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
		Exit_StopMode();
	
	// MPR121 IRQ∏¶ ≈Î«— EXTI Wake UP
		if(GPIO_PIN == GPIO_PIN_15){
			countIdleState = 0;
		}
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

