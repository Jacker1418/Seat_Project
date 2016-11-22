#include "stm32f1xx_hal.h"

#define OFFSET_X 2
#define OFFSET_Y 2

extern void Error_Handler(void);

static I2C_HandleTypeDef mpr121;

static struct point position[COUNT_KEY] = {
																												{ -1, 1 },
																												{ 0, 1 },
																												{ 1, 1 },
																												{ -1, 0 },
																												{ 0, 0 },
																												{ 1, 0 },
																												{ -1, -1 },
																												{ 0, -1 },
																												{ 1, -1 }
																											};

static int arrWeight[COUNT_KEY] = { 10, // -1, 1
																							 7,  // 0, 1
																							 10, // 1, 1
																							 8,  // -1, 0
																							 3,	 // 0, 0
																							 8,  // 1, 0
																							 10, // -1, -1,
																							 7,  // 0, -1
																							 10  // 1, -1 
																							 };

uint16_t idleSensorValue[COUNT_KEY] = {0, };

void HAL_I2C_MspInit(I2C_HandleTypeDef* mpr121){
	
  GPIO_InitTypeDef GPIO_InitStruct;
	
  if(mpr121->Instance==I2C1)
  {
		
/*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
		
  /* Enable I2Cx clock */
  __HAL_RCC_I2C1_CLK_ENABLE(); 

/*##-2- Configure peripheral GPIO ##########################################*/  
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = GPIO_PIN_7;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
/*##-3- Configure the NVIC for I2C ########################################*/   
  /* NVIC for I2Cx */
  HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* mpr121){

  if(mpr121->Instance==I2C1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
	}
  
}

void I2C_MPR121_Init(void){
	
	mpr121.Instance             = I2C1;
  mpr121.Init.ClockSpeed      = I2C_SPEEDCLOCK;
  mpr121.Init.DutyCycle       = I2C_DUTYCYCLE;
  mpr121.Init.OwnAddress1     = MASTER_ADDRESS;
  mpr121.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  mpr121.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  mpr121.Init.OwnAddress2     = 0xFF;
  mpr121.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  mpr121.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;  
  
  if(HAL_I2C_Init(&mpr121) != HAL_OK)
  {
    /* Initialization Error */
		printf("I2C_mpr121._Init() : ");
    Error_Handler();
  }
}

void I2C1_EV_IRQHandler(void){
	printf("I2Cx_EV_IRQHandler()\n");
  HAL_I2C_EV_IRQHandler(&mpr121);
}

void I2C1_ER_IRQHandler(void){
  HAL_I2C_ER_IRQHandler(&mpr121);
}

HAL_StatusTypeDef MPR121_Configuration(void){
	volatile HAL_StatusTypeDef result = HAL_OK;
	
	//mpr121. Disable
	result = MPR121_Send_Polling(ELE_CFG, 0x00, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	
	// Section A Controls filtering when data is > baseline
	result = MPR121_Send_Polling(MHD_R, 0x01, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(NHD_R, 0x01, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(NCL_R, 0x00, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(FDL_R, 0x00, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	
	// Section B Controls filtering when data is < baseline
	result = MPR121_Send_Polling(MHD_F, 0x01, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(NHD_F, 0x01, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(NCL_F, 0xFF, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(FDL_F, 0x02, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	
	//Section C Set touch and release thresholds for each electrode
	result = MPR121_Send_Polling(ELE0_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE0_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE0_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE1_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE1_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE2_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE2_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE3_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE3_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE4_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE4_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE5_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE5_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE6_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE6_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE7_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE7_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result =MPR121_Send_Polling(ELE8_R, REL_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(ELE8_T, TOU_THRESH, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	
	//Section D Set the Filter Configuration
	result = MPR121_Send_Polling(AFE_CFG, 0x5B, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	result = MPR121_Send_Polling(FIL_CFG, 0x44, 1000) ;
	if(result != HAL_OK){
		return result;
	}
	
	//mpr121. Enable
	result = MPR121_Send_Polling(ELE_CFG, 0x0C, 1000) ;
	if(result != HAL_OK){
		return result;
	}

	return result;
}


HAL_StatusTypeDef MPR121_Send_Polling(uint8_t reg, uint8_t value, uint32_t Timeout){

	uint8_t Size = 2;
	uint8_t data[2] = { reg, value };

	uint8_t* pData = data;
	
  if(mpr121.State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
      return  HAL_ERROR;
    }

    /* Wait until BUSY flag is reset */
    if(I2C_WaitOnFlagUntilTimeout(&mpr121, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY_FLAG) != HAL_OK)
    {
      return HAL_BUSY;
    }

    /* Process Locked */
    __HAL_LOCK(&mpr121);

    /* Disable Pos */
    CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_POS);

    mpr121.State = HAL_I2C_STATE_BUSY_TX;
    mpr121.Mode = HAL_I2C_MODE_MASTER;
    mpr121.ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(&mpr121, MPR121_ADDRESS << 1, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);

    while(Size > 0)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnTXEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
      {
        if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to DR */
      mpr121.Instance->DR = (*pData++);
      Size--;

      if((__HAL_I2C_GET_FLAG(&mpr121, I2C_FLAG_BTF) == SET) && (Size != 0))
      {
        /* Write data to DR */
        mpr121.Instance->DR = (*pData++);
        Size--;
      }
    }

    /* Wait until BTF flag is set */
    if(I2C_WaitOnBTFFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

    /* Generate Stop */
    SET_BIT(mpr121.Instance->CR1, I2C_CR1_STOP);

    mpr121.State = HAL_I2C_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(&mpr121);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef MPR121_Receive_1byte_Polling(volatile uint8_t *pData, uint8_t reg, uint32_t Timeout){
	MPR121_Receive(pData, reg, 1, 1000);
}
HAL_StatusTypeDef MPR121_Receive_2byte_Polling(volatile uint16_t *pData,uint8_t reg , uint32_t Timeout){
	
	HAL_StatusTypeDef result;
	uint8_t buffer[2] = {0.};
	
	volatile uint8_t LSB;
	volatile uint8_t MSB;
	volatile uint16_t temp;
	
	result = MPR121_Receive(buffer, reg, 2, Timeout);
	if(result != HAL_OK){
		return result;
	}else{
		LSB = buffer[0];
		MSB = buffer[1];
		
		temp = MSB;
		temp<<= 8;
		temp |= LSB;
		
		*pData = temp;
	}
	return HAL_OK;
}
HAL_StatusTypeDef MPR121_Receive_All_Key_Polling(volatile struct Touch_Data *data, uint32_t Timeout){
	
	volatile uint8_t arrSensor_Buffer[BUFFER_SIZE];
	HAL_StatusTypeDef result;
	SensorValue_Status resultData;
	
	result = MPR121_Receive(arrSensor_Buffer, 0x00, 22, Timeout);
	if(result != HAL_OK){
		return result;
	}else{
		resultData = extractTouchState(arrSensor_Buffer, data) ;
		if(resultData != Value_OK){
			printf("extractTouchState() Error\n");
		}else{
			//printf("extractTouchState() OK\n");
		}
		
		resultData = extractSensorValue(arrSensor_Buffer, data);
		if(resultData != Value_OK){
			printf("extractSensorValue() Error\n");
		}else{
			//printf("extractSensorValue() OK\n");
		}
		
		if(data->touchState == 0){
			resultData = setSensor_Idle(data);
			if(resultData != Value_OK){
				printf("setSensor_Idle() Error\n");
			}
		}
		resultData = getValueDiff(data);
			if(resultData != Value_OK){
				printf("getValueDiff() Error\n");
			}
		resultData = getPosition(data);
		if(resultData != Value_OK){
			printf("getPosition() Error\n");
		}else{
			//printf("getPosition() OK\n");
		}
	}
	
	return HAL_OK;
}
HAL_StatusTypeDef MPR121_Receive(volatile uint8_t* pData, uint8_t reg, uint32_t Size, uint32_t Timeout){
	
	 if(mpr121.State == HAL_I2C_STATE_READY)
  {
    if((pData == NULL) || (Size == 0))
    {
#ifdef DEBUG
		printf("pData == NULL || Size == 0\n");
#endif
      return  HAL_ERROR;
    }
		
    /* Process Locked */
    __HAL_LOCK(&mpr121);
		
		/* Disable Pos */
    CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_POS);

    mpr121.State = HAL_I2C_STATE_BUSY_TX;
    mpr121.Mode = HAL_I2C_MODE_MASTER;
    mpr121.ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    if(I2C_MasterRequestWrite(&mpr121, MPR121_ADDRESS << 1, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
        return HAL_TIMEOUT;
      }
    }

    /* Clear ADDR flag */
    __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);


		/* Wait until TXE flag is set */
		if(I2C_WaitOnTXEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
		{
			if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
			{
				/* Generate Stop */
				SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
				return HAL_ERROR;
			}
			else
			{
				return HAL_TIMEOUT;
			}
		}

		/* Write data to DR */
		mpr121.Instance->DR = reg;


    /* Wait until BTF flag is set */
    if(I2C_WaitOnBTFFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
        return HAL_ERROR;
      }
      else
      {
        return HAL_TIMEOUT;
      }
    }

		
    /* Send Slave Address */
    if(I2C_MasterRequestRead(&mpr121, MPR121_ADDRESS << 1, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
#ifdef DEBUG
				printf("HAL_I2C_Master_Receive() -> I2C_MasterRequestRead()\n");
#endif
        return HAL_ERROR;
      }
      else
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
        return HAL_TIMEOUT;
      }
    }

    if(Size == 1)
    {
      /* Disable Acknowledge */
      CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_ACK);

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
         software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);

      /* Generate Stop */
      SET_BIT(mpr121.Instance->CR1, I2C_CR1_STOP);
	  
      /* Re-enable IRQs */
      __enable_irq(); 
    }
    else if(Size == 2)
    {
      /* Enable Pos */
      SET_BIT(mpr121.Instance->CR1, I2C_CR1_POS);

      /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
         software sequence must complete before the current byte end of transfer */
      __disable_irq();

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);

      /* Disable Acknowledge */
      CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_ACK);

       /* Re-enable IRQs */
       __enable_irq(); 
    }
    else
    {
      /* Enable Acknowledge */
      SET_BIT(mpr121.Instance->CR1, I2C_CR1_ACK);

      /* Clear ADDR flag */
      __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);
    }

    while(Size > 0)
    {
      if(Size <= 3)
      {
        /* One byte */
        if(Size == 1)
        {
          /* Wait until RXNE flag is set */
          if(I2C_WaitOnRXNEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)      
          {
            if(mpr121.ErrorCode == HAL_I2C_ERROR_TIMEOUT)
            {
              return HAL_TIMEOUT;
            }
            else
            {
#ifdef DEBUG
				printf("HAL_I2C_Master_Receive() -> I2C_WaitOnRXNEFlagUntilTimeout()\n");
#endif
              return HAL_ERROR;
            }
          }

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;
        }
        /* Two bytes */
        else if(Size == 2)
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(&mpr121, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
           __disable_irq();

          /* Generate Stop */
          SET_BIT(mpr121.Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;

          /* Re-enable IRQs */
          __enable_irq();

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;
        }
        /* 3 Last bytes */
        else
        {
          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(&mpr121, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Disable Acknowledge */
          CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_ACK);

          /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
             software sequence must complete before the current byte end of transfer */
          __disable_irq();

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;

          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(&mpr121, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          SET_BIT(mpr121.Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;
	  
          /* Re-enable IRQs */
          __enable_irq(); 

          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;
        }
      }
      else
      {
        /* Wait until RXNE flag is set */
        if(I2C_WaitOnRXNEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)      
        {
          if(mpr121.ErrorCode == HAL_I2C_ERROR_TIMEOUT)
          {
            return HAL_TIMEOUT;
          }
          else
          {
#ifdef DEBUG
			  printf("HAL_I2C_Master_Receive() -> I2C_WaitOnRXNEFlagUntilTimeout()\n");
#endif
            return HAL_ERROR;
          }
        }

        /* Read data from DR */
        (*pData++) = mpr121.Instance->DR;
        Size--;

        if(__HAL_I2C_GET_FLAG(&mpr121, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          (*pData++) = mpr121.Instance->DR;
          Size--;
        }
      }
    }

    mpr121.State = HAL_I2C_STATE_READY;
    mpr121.Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(&mpr121);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
SensorValue_Status extractTouchState(volatile uint8_t* buffer, volatile struct Touch_Data* data){
	volatile uint8_t LSB = 0;
	volatile uint8_t MSB = 0;
	volatile uint16_t result = 0;
	
	if(buffer == NULL){
		return Value_Destination_NULL;
	}
	if(data == NULL){
		return Value_NULL;
	}
	LSB = buffer[0];
	MSB = buffer[1];
	
	result = MSB;
	result <<= 8;
	result |= LSB;
	
	data->touchState = result;
	
	return Value_OK;
}

SensorValue_Status extractSensorValue(volatile uint8_t* buffer, volatile struct Touch_Data* data){
	
	
	uint8_t index = 0;

	uint8_t LSB = 0;
	uint8_t MSB = 0;
	int result = 0;
	
	if(buffer == NULL){
		return Value_Destination_NULL;
	}
	if(data == NULL){
		return Value_NULL;
	}
	
	for(index = SENSOR_VALUE; index < 0x16; index+=2){
		LSB = buffer[index];
		MSB = buffer[index + 1];
		
		result = MSB;
		result <<= 8;
		result |= LSB;

		data->sensorValue[(index - SENSOR_VALUE) / 2] = result ;
	}
	
	return Value_OK;
}

SensorValue_Status setSensor_Idle(struct Touch_Data* data){
	uint8_t index = 0;
	
	if(data == NULL){
		return Value_NULL;
	}
	for(index = 0; index < COUNT_KEY; index++){
		idleSensorValue[index] = data->sensorValue[index];
	}

	return Value_OK;
}

SensorValue_Status getPosition(struct Touch_Data* data){
	
		int sumOfX = 0;
		int sumOfY = 0;

		int sumOfTotalMass = 0;

		if(data == NULL){
			return Value_NULL;
		}
		
		for (int i = 0; i < 9; i++) {
			sumOfX += data->sensorValue[i] * (((position[i].x + OFFSET_X) * arrWeight[i]) );
			sumOfY += data->sensorValue[i] * (((position[i].y + OFFSET_Y) * arrWeight[i]) );

			sumOfTotalMass += data->sensorValue[i];
		}

		data->position.x = (float)sumOfX / (float)(sumOfTotalMass + 1);
		data->position.y = (float)sumOfY / (float)(sumOfTotalMass + 1);
		data->position.x -= OFFSET_X;
		data->position.y -= OFFSET_Y;
		
		return Value_OK;
}

SensorValue_Status getValueDiff(struct Touch_Data* data){

	uint16_t index = 0;
	int result = 0;
	
	if(data == NULL){
		return Value_NULL;
	}
	
	for(index = 0; index < COUNT_KEY; index++){
//		printf("Sensor : %d, Idle : %d\n", idleSensorValue[index], data->sensorValue[index]);
		result = idleSensorValue[index] - data->sensorValue[index];
		
//		printf("result : %d\n", result);
		if(result < 0){
			result = 0;
		}
		data->sensorValue[index] = result;
	}
	
	return Value_OK;
}