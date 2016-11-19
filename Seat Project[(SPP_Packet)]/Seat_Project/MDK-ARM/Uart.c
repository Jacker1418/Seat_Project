#include "stm32f1xx_hal.h"

extern void Error_Handler(void);

static UART_HandleTypeDef console;
static UART_HandleTypeDef bluetooth;

/*############## MSP Override ###############*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart){

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  else if(huart->Instance==USART3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(USART3_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart){

  if(huart->Instance==USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
  }
  else if(huart->Instance==USART3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);
  }

}


/*############## User Perpherial Init ###########*/
void UART_Console_Init(void){
	
	console.Instance = USART2;
  console.Init.BaudRate = 9600;
  console.Init.WordLength = UART_WORDLENGTH_8B;
  console.Init.StopBits = UART_STOPBITS_1;
  console.Init.Parity = UART_PARITY_NONE;
  console.Init.Mode = UART_MODE_TX_RX;
  console.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  if (HAL_UART_Init(&console) != HAL_OK)
  {
    Error_Handler();
  }
}
void UART_Bluetooth_Init(void){
	
	bluetooth.Instance = USART3;
  bluetooth.Init.BaudRate = 9600;
  bluetooth.Init.WordLength = UART_WORDLENGTH_8B;
  bluetooth.Init.StopBits = UART_STOPBITS_1;
  bluetooth.Init.Parity = UART_PARITY_NONE;
  bluetooth.Init.Mode = UART_MODE_TX_RX;
  bluetooth.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  if (HAL_UART_Init(&bluetooth) != HAL_OK)
  {
    Error_Handler();
  }
	
	Queue_Init();
}
void Queue_Init(void){
	head = 0;
	tail = 0;
	countQueue = 0;
}


/*############## Printf Connect ###############*/
PUTCHAR_PROTOTYPE{
	
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  HAL_UART_Transmit(&console, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


/*############## Queue Method ##############*/
Queue_Status Pop(struct Packet* packet){

	if (head != tail) {
		packet = &queuePacket[head];
		head = (head + 1) % QUEUE_SIZE;
		countQueue++;
		
		return Queue_OK;
	}
	return Queue_Empty;
}
Queue_Status Push(UART_Modes mode, RTC_HandleTypeDef* rtc, uint16_t* sensorValues, float* position){
	
	uint8_t index = (tail + 1) % QUEUE_SIZE;

	if (head != index) {
		makePacket(&queuePacket[tail], mode, rtc, sensorValues, position);
		tail = (tail + 1) % QUEUE_SIZE;
		countQueue--;
		
		return Queue_OK;
	}
	return Queue_Full;
}

uint8_t howManyItem(void){
	return countQueue;
}
Queue_Status checkTime_Items(RTC_HandleTypeDef* myRTC, struct Date* androidRTC){

		uint8_t index = 0;
	
		RTC_DateTypeDef date;
		RTC_TimeTypeDef time;
	
		/* Get the RTC current Time */
		HAL_RTC_GetTime(myRTC, &time, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(myRTC, &date, RTC_FORMAT_BIN);
	
		for(index = head; index != tail; index = (index + 1) % QUEUE_SIZE){
			
		}
		return Queue_OK;
}
/*############## Bluetooth Transmit Receive Method ############*/
HAL_StatusTypeDef Bluetooth_Send_Packet_Polling(struct Packet* packet ,uint32_t Timeout){
	uint16_t* tmp;
  uint32_t tmp_state = 0;
	
	uint8_t arrayPacket[UART_BUFFER_SIZE];
	uint32_t Size = 0;
	uint8_t* pData;
	
	uint8_t flag = 0;
	
	uint8_t tempData = 0;
	
  tmp_state = bluetooth.State;
	
	
  if((tmp_state == HAL_UART_STATE_READY) || (tmp_state == HAL_UART_STATE_BUSY_RX))
  {
		if(packet == NULL){
			return HAL_ERROR;
		}
		
		if(packetToArray(arrayPacket, packet) != Packet_Ok){
			return HAL_ERROR;
		}
		
		pData = arrayPacket;
		Size = UART_BUFFER_SIZE;
		
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(&bluetooth);

    bluetooth.ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a non-blocking receive process is ongoing or not */
    if(bluetooth.State == HAL_UART_STATE_BUSY_RX) 
    {
      bluetooth.State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      bluetooth.State = HAL_UART_STATE_BUSY_TX;
    }

    bluetooth.TxXferSize = Size;
    bluetooth.TxXferCount = Size;
    while(bluetooth.TxXferCount > 0 && flag == 0)
    {
      bluetooth.TxXferCount--;
      if(bluetooth.Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) pData;
        bluetooth.Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(bluetooth.Init.Parity == UART_PARITY_NONE)
        {
          pData +=2;
        }
        else
        { 
          pData +=1;
        }
      } 
      else
      {
        if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
				tempData =  (*pData++ & (uint8_t)0xFF);
        bluetooth.Instance->DR = tempData;
				if(tempData == 0xCC){
					flag = 1;
				}
      }
    }

    if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TC, RESET, Timeout) != HAL_OK)
    { 
      return HAL_TIMEOUT;
    }

    /* Check if a non-blocking receive process is ongoing or not */
    if(bluetooth.State == HAL_UART_STATE_BUSY_TX_RX) 
    {
      bluetooth.State = HAL_UART_STATE_BUSY_RX;
    }
    else
    {
      bluetooth.State = HAL_UART_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&bluetooth);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef Bluetooth_Send_Polling(uint8_t* buffer, uint32_t Timeout){
	uint16_t* tmp;
  uint32_t tmp_state = 0;
	
	uint32_t Size = 0;
	
	uint8_t flag = 0;
	uint8_t tempData = 0;
	
  tmp_state = bluetooth.State;
	
  if((tmp_state == HAL_UART_STATE_READY) || (tmp_state == HAL_UART_STATE_BUSY_RX))
  {
		buffer = buffer;
		Size = 64;
		
    if((buffer == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(&bluetooth);

		/* Reset USART */
		__HAL_UART_DISABLE(&bluetooth);
		__HAL_UART_ENABLE(&bluetooth);
		
    bluetooth.ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a non-blocking receive process is ongoing or not */
    if(bluetooth.State == HAL_UART_STATE_BUSY_RX) 
    {
      bluetooth.State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      bluetooth.State = HAL_UART_STATE_BUSY_TX;
    }

    bluetooth.TxXferSize = Size;
    bluetooth.TxXferCount = Size;
		bluetooth.TxCount = 0;
		
    while(bluetooth.TxXferCount > 0 && flag == 0)
    {
      bluetooth.TxXferCount--;
      if(bluetooth.Init.WordLength == UART_WORDLENGTH_9B)
      {
        if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
        tmp = (uint16_t*) buffer;
        bluetooth.Instance->DR = (*tmp & (uint16_t)0x01FF);
        if(bluetooth.Init.Parity == UART_PARITY_NONE)
        {
          buffer +=2;
        }
        else
        { 
          buffer +=1;
        }
      } 
      else
      {
        if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
        {
          return HAL_TIMEOUT;
        }
				tempData =  (*buffer++ & (uint8_t)0xFF);
        bluetooth.Instance->DR = tempData;
				bluetooth.TxCount++;
				if(tempData == END){
					if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TXE, RESET, Timeout) != HAL_OK)
					{
						return HAL_TIMEOUT;
					}
					bluetooth.Instance->DR = '\r';
					break;
				}
      }
    }

    if(UART_WaitOnFlagUntilTimeout(&bluetooth, UART_FLAG_TC, RESET, Timeout) != HAL_OK)
    { 
      return HAL_TIMEOUT;
    }

    /* Check if a non-blocking receive process is ongoing or not */
    if(bluetooth.State == HAL_UART_STATE_BUSY_TX_RX) 
    {
      bluetooth.State = HAL_UART_STATE_BUSY_RX;
    }
    else
    {
      bluetooth.State = HAL_UART_STATE_READY;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&bluetooth);
		
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef Bluetooth_Receive_IT(volatile uint8_t* buffer, uint32_t length){
	uint32_t tmp_state = 0;
  
  tmp_state = bluetooth.State;
  if((tmp_state == HAL_UART_STATE_READY) || (tmp_state == HAL_UART_STATE_BUSY_TX))
  {
    if((buffer == NULL ) || (length == 0))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(&bluetooth);
		
		/* Reset USART */
		__HAL_UART_DISABLE(&bluetooth);
		__HAL_UART_ENABLE(&bluetooth);

    bluetooth.pRxBuffPtr = buffer;
    bluetooth.RxXferSize = length;
    bluetooth.RxXferCount = length;
		bluetooth.RxCount = 0;

    bluetooth.ErrorCode = HAL_UART_ERROR_NONE;
    /* Check if a transmit process is ongoing or not */
    if(bluetooth.State == HAL_UART_STATE_BUSY_TX)
    {
      bluetooth.State = HAL_UART_STATE_BUSY_TX_RX;
    }
    else
    {
      bluetooth.State = HAL_UART_STATE_BUSY_RX;
    }

    /* Process Unlocked */
    __HAL_UNLOCK(&bluetooth);

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&bluetooth, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&bluetooth, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&bluetooth, UART_IT_RXNE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/*############## UART IRQ Handler ############*/
void USART3_IRQHandler(void){
  HAL_UART_IRQHandler(&bluetooth);
	CLEAR_BIT(bluetooth.Instance->SR, USART_SR_RXNE);
}

/*############## Packet Method ###############*/
Packet_Status makePacket(volatile struct Packet* packet, UART_Modes mode, RTC_HandleTypeDef* rtc, uint16_t* sensorValues, float* position){

	int i = 0;
	int length = 1;
	
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
  
	if(mode == Normal){
		
		if(packet == NULL){
			return Packet_NULL;
		}else if(rtc == NULL){
			return RTC_NULL;
		}else if(sensorValues == NULL){
			return Packet_Value_NULL;
		}else if(position == NULL){
			return Packet_Value_NULL;
		}
		
		packet->mode = mode;
		
		/* Get the RTC current Time */
		HAL_RTC_GetTime(rtc, &time, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(rtc, &date, RTC_FORMAT_BIN);
		
		packet->date.year = date.Year;
		packet->date.month = date.Month;
		packet->date.day = date.Date;
		
		packet->date.hour = time.Hours;
		packet->date.minute = time.Minutes;
		
		length += sizeof(packet->date);
		
	  for(i = 0; i < 9; i++){
			if(sensorValues[i] == START){
				sensorValues[i]++;
			}else if(sensorValues[i] == END){
				sensorValues[i]--;
			}
			packet->sensorValue[i] = sensorValues[i];
		}
		length += sizeof(packet->sensorValue);
		
		for(i = 0; i < 2; i++){
			packet->position[i] = position[i];
		}
		length += sizeof(packet->position);
		
		#ifdef USE_CRC
		
		
		#endif
		packet->length = length;
		
	}else if(mode == Real_Time){
		
		if(packet == NULL){
			return Packet_NULL;
		}else if(sensorValues == NULL){
			return Packet_Value_NULL;
		}else if(position == NULL){
			return Packet_Value_NULL;
		}
		
		packet->mode = mode;
		length += sizeof(packet->length);
		
		for(i = 0; i < 9; i++){
			if(sensorValues[i] == START){
				sensorValues[i]++;
			}else if(sensorValues[i] == END){
				sensorValues[i]--;
			}
			packet->sensorValue[i] = sensorValues[i];
		}
		length += sizeof(packet->sensorValue);
		
		for(i = 0; i < 2; i++){
			packet->position[i] = position[i];
		}
		length += sizeof(packet->position);
		
		#ifdef USE_CRC
		
		
		#endif
		packet->length = length;
		
	}else if(mode == Response){
		return Packet_Mode_NULL;
	}else{
		return Packet_Mode_NULL;
	}
	
	return Packet_Ok;
}

Packet_Status releasePacket(volatile struct Packet* packet, uint8_t* message, uint8_t length){
	
	uint8_t index = 0;
	
	if(packet == NULL){
		return Packet_NULL;
	}else if(message == NULL){
		return Message_NULL;
	}
	
	switch(message[0]){
		case Normal:
			if(length == message[Length]){
				packet->mode = message[Mode];
				
				packet->date.year = message[Year];
				packet->date.month = message[Month];
				packet->date.day = message[Day];
				packet->date.hour = message[Hour];
				packet->date.minute = message[Minute];
				
			}else{
				packet->mode = None;
				return Packet_Dropout;
			}
		break;
		case Real_Time:
			
			#ifdef USE_CRC
					index = Length - 6;
			#else
					index = Length - 5;
			#endif
			if(length == message[index]){
				
				packet->mode = message[Mode];
			}else{
				packet->mode = None;
				return Packet_Dropout;
			}
		break;
		default:
			packet->mode = None;
		break;
	}
	
	return Packet_Ok;
}


Packet_Status packetToArray(uint8_t* destination, struct Packet* packet){

	uint16_t index = 0;
	
	if(packet == NULL || destination == NULL){
		return Packet_NULL;
	}else{
		destination[index++] = START;
		
		if(packet->mode == None){
			return Packet_Mode_NULL;
		}
		destination[index++] = packet->mode;
		
		if(packet->mode == Normal){
		
			if(messageToDate(destination, packet, &index) != Packet_Ok){
				return Packet_Date_NULL;
			}
			if(messageToSensorValue(destination, packet, &index) != Packet_Ok){
				return Packet_Value_NULL;
			}
			messageToPosition(destination, packet, &index);
			#ifdef USE_CRC
			if(messageToCRC(destination, packet, &index) != Packet_Ok){
				return Packet_CRC_NULL;
			}
			#endif
			if(messageToLength(destination, packet, &index) != Packet_Ok){
				return Packet_Length_NULL;
			}
			
		}else if(packet->mode == Real_Time){
			
			if(messageToSensorValue(destination, packet, &index) != Packet_Ok){
				return Packet_Value_NULL;
			}
			
			messageToPosition(destination, packet, &index);
			
			#ifdef USE_CRC
			if(convertCRC(destination, packet, &index) != Packet_Ok){
				return Packet_CRC_NULL;
			}
			#endif
			
			if(messageToLength(destination, packet, &index) != Packet_Ok){
				return Packet_Length_NULL;
			}
			
		}else if(packet->mode == Response){
			#ifdef USE_CRC
			if(messageToCRC(destination, packet, &index) != Packet_Ok){
				return Packet_CRC_NULL;
			}
			#endif
			if(messageToLength(destination, packet, &index) != Packet_Ok){
				return Packet_Length_NULL;
			}
		}
		
		destination[index++] = END;
		destination[index] = '\n';
		
		return Packet_Ok;
	}
}

Packet_Status messageToDate(uint8_t* message, struct Packet* packet, uint16_t* index){
	
		if(packet->date.year == 0 & packet->date.month == 0 & packet->date.day == 0 
			& packet->date.hour == 0 & packet->date.minute == 0){
			return Packet_Date_NULL;
		}else{
			message[*index++] = packet->date.year;
			message[*index++] = packet->date.month;
			message[*index++] = packet->date.day;
			message[*index++] = packet->date.hour;
			message[*index++] = packet->date.minute;
		}
		
		return Packet_Ok;
}

Packet_Status messageToLength(uint8_t* message, struct Packet* packet, uint16_t* index){
	
		if(packet->length == 0){
			return Packet_Length_NULL;
		}else{
			message[*index++] = packet->length;
		}
		return Packet_Ok;
}
Packet_Status messageToSensorValue(uint8_t* message, struct Packet* packet, uint16_t* index){
	
	uint16_t i = 0;
	uint16_t indexTemp = 0;
	uint16_t length = 0;
	int temp = 0;
	
	for(i = 0; i < 9; i++ ){
			if(packet->sensorValue[i] != 0){
				break;
			}
	}
	
	if(i == 9){
		return Packet_Value_NULL;
	}else{
		indexTemp = *index;
		length = sizeof(packet->sensorValue) / sizeof(uint8_t);
		length += indexTemp;
		for(; *index < length ; *index++){
			temp = (*index - indexTemp) / 2;
			message[*index] = (packet->sensorValue[temp] >> (8 * (temp % 2))) & 0xFF;
		}
	}
		return Packet_Ok;
}

Packet_Status messageToPosition(uint8_t* message, struct Packet* packet, uint16_t* index){

		uint16_t indexTemp = 0;
		uint16_t length = 0;
	
		indexTemp = *index;
		length = sizeof(float);
		length += indexTemp;
		
		for(; *index < length; *index++){
			message[*index] = ((uint32_t)packet->position[0] >> (8 * (*index - indexTemp))) & 0xFF;
		}
		indexTemp = *index;
		length = sizeof(float);
		length += indexTemp;
		for(; *index < length; *index++){
			message[*index] = ((uint32_t)packet->position[1] >> (8 * (*index - indexTemp))) & 0xFF;
		}
		
		return Packet_Ok;
}

#ifdef USE_CRC
Packet_Status messageToCRC(uint8_t* message, struct Packet* packet, uint16_t* index){
	
	uint16_t indexTemp = 0;
	uint16_t length = 0;
	if(packet->crc == 0){
		return Packet_CRC_NULL;
	}else{
		
		indexTemp = *index;
		length = sizeof(packet->crc);
		length += indexTemp;
		
		for(; *index < length; *index++){
			message[*index] = (packet->crc >> (8 * (*index - indexTemp))) & 0xFF ;
		}
	}
		return Packet_Ok;
}
#endif
void printPacket(volatile struct Packet *packet){
	if(packet == NULL){
		printf("printPacket() -> packet is null\n");
	}
	
	switch(packet->mode){
		case Normal:
			printf("Mode : Normal\n");
		break;
		case Real_Time:
			printf("Mode : Real Time\n");
		break;
		default:
			printf("printPacket() -> mode is wrong\n");
		break;
	}
	
	printf("Time : %4d.%2d.%2d %2d:%2d\n", packet->date.year, packet->date.month, packet->date.day, packet->date.hour, packet->date.minute );
	
	printf("Length : %2d\n", packet->length);
}
