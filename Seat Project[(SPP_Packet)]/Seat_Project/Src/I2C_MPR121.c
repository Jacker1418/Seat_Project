#include "I2C_MPR121.h"

extern void Error_Handler(void);

void HAL_I2C_MspInit(I2C_HandleTypeDef* mpr121){
	
  GPIO_InitTypeDef GPIO_InitStruct;
	
  if(mpr121->Instance==I2C1)
  {
    /**I2C1 GPIO Configuration    
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
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
  mpr121.Init.OwnAddress2     = 0x01;
  mpr121.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  mpr121.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;  
  
  if(HAL_I2C_Init(&mpr121) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
}


HAL_StatusTypeDef MPR121_Configuration(void){
	volatile HAL_StatusTypeDef result = HAL_OK;
	
	//MPR121 Disable
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
	result = MPR121_Send_Polling(ELE8_R, REL_THRESH, 1000) ;
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
	
	//MPR121 Enable
	result = MPR121_Send_Polling(ELE_CFG, 0x0C, 1000) ;
	if(result != HAL_OK){
		return result;
	}

	return result;
}


HAL_StatusTypeDef MPR121_Send_Polling(uint8_t reg, uint8_t value, uint32_t Timeout){
	uint8_t length = 1;
	
	 if(mpr121.State == HAL_I2C_STATE_READY)
  {
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
    if(I2C_MasterRequestWrite(&mpr121, MPR121_ADDRESS, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Process Unlocked */
        __HAL_UNLOCK(&mpr121);
				printf("I2C_MasterRequestWrite Error\n");
				Error_Handler();
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

    while(length > 0)
    {
      /* Wait until TXE flag is set */
      if(I2C_WaitOnTXEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
      {
        if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
        {
          /* Generate Stop */
          SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
					printf("I2C_WaitOnTXEFlagUntilTimeout Error\n");
					Error_Handler();
          return HAL_ERROR;
        }
        else
        {
          return HAL_TIMEOUT;
        }
      }

      /* Write data to DR */
      mpr121.Instance->DR = value;
      length--;

//			
//      if((__HAL_I2C_GET_FLAG(&mpr121, I2C_FLAG_BTF) == SET) && (length != 0))
//      {
//        /* Write data to DR */
//        mpr121.Instance->DR = value;
//        length--;
//      }
//		
    }

    /* Wait until BTF flag is set */
    if(I2C_WaitOnBTFFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
    {
      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
      {
        /* Generate Stop */
        SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
				printf("I2C_WaitOnBTFFlagUntilTimeout Error\n");
				Error_Handler();
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
HAL_StatusTypeDef MPR121_Receive_1byte_Polling(volatile uint16_t *arrSensorValue, uint8_t reg, uint32_t Timeout){
	return MPR121_Receive(arrSensorValue, reg, 1, Timeout);
}
HAL_StatusTypeDef MPR121_Receive_2byte_Polling(volatile uint16_t *arrSensorValue,uint8_t reg , uint32_t Timeout){
	return MPR121_Receive(arrSensorValue, reg, 2, Timeout);
}
HAL_StatusTypeDef MPR121_Receive_All_Key_Polling(volatile uint16_t *arrSensorValue , uint32_t Timeout){
	return MPR121_Receive(arrSensorValue, SENSOR_VALUE, BUFFER_SIZE, Timeout);
}
HAL_StatusTypeDef MPR121_Receive(volatile uint16_t* arrSensorValue, uint8_t reg, uint32_t length, uint32_t Timeout){
	
	volatile uint8_t* buffer = arrSensor_Buffer;
	
	if(mpr121.State == HAL_I2C_STATE_READY)
  {
    if((buffer == NULL) || (length == 0))
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

//		//######## #1 1. Send Address + 2. Reg Address  = Master Request Write #############
//		 /* Disable Pos */
//    CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_POS);

//    mpr121.State = HAL_I2C_STATE_BUSY_TX;
//    mpr121.Mode = HAL_I2C_MODE_MASTER;
//    mpr121.ErrorCode = HAL_I2C_ERROR_NONE;
//		
//		 if(I2C_MasterRequestWrite(&mpr121, MPR121_ADDRESS, Timeout) != HAL_OK)
//    {
//      if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
//      {
//        /* Process Unlocked */
//        __HAL_UNLOCK(&mpr121);
//        return HAL_ERROR;
//      }
//      else
//      {
//        /* Process Unlocked */
//        __HAL_UNLOCK(&mpr121);
//        return HAL_TIMEOUT;
//      }
//    }

//    /* Clear ADDR flag */
//    __HAL_I2C_CLEAR_ADDRFLAG(&mpr121);
//		
//		/* Write data to DR */
//      mpr121.Instance->DR = reg;

//		if(I2C_WaitOnTXEFlagUntilTimeout(&mpr121, Timeout) != HAL_OK)
//		{
//			if(mpr121.ErrorCode == HAL_I2C_ERROR_AF)
//			{
//				/* Generate Stop */
//				SET_BIT(mpr121.Instance->CR1,I2C_CR1_STOP);
//				return HAL_ERROR;
//			}
//			else
//			{
//				return HAL_TIMEOUT;
//			}
//		}
//		
		//######## #2 1. Send Address + 2. Receive Data  = Master Request Read #############
    /* Disable Pos */
    CLEAR_BIT(mpr121.Instance->CR1, I2C_CR1_POS);

    mpr121.State = HAL_I2C_STATE_BUSY_RX;
    mpr121.Mode = HAL_I2C_MODE_MASTER;
    mpr121.ErrorCode = HAL_I2C_ERROR_NONE;

    /* Send Slave Address */
    if(I2C_MasterRequestRead(&mpr121, MPR121_ADDRESS, Timeout) != HAL_OK)
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
		
    if(length == 1)
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
    else if(length == 2)
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

    while(length > 0)
    {
      if(length <= 3)
      {
        /* One byte */
        if(length == 1)
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
              return HAL_ERROR;
            }
          }

          /* Read data from DR */
          (*buffer++) = mpr121.Instance->DR;
          length--;
        }
        /* Two bytes */
        else if(length == 2)
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
          (*buffer++) = mpr121.Instance->DR;
          length--;

          /* Re-enable IRQs */
          __enable_irq();

          /* Read data from DR */
          (*buffer++) = mpr121.Instance->DR;
          length--;
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
          (*buffer++) = mpr121.Instance->DR;
          length--;

          /* Wait until BTF flag is set */
          if(I2C_WaitOnFlagUntilTimeout(&mpr121, I2C_FLAG_BTF, RESET, Timeout) != HAL_OK)
          {
            return HAL_TIMEOUT;
          }

          /* Generate Stop */
          SET_BIT(mpr121.Instance->CR1, I2C_CR1_STOP);

          /* Read data from DR */
          (*buffer++) = mpr121.Instance->DR;
          length--;
	  
          /* Re-enable IRQs */
          __enable_irq(); 

          /* Read data from DR */
          (*buffer++) = mpr121.Instance->DR;
          length--;
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
            return HAL_ERROR;
          }
        }

        /* Read data from DR */
        (*buffer++) = mpr121.Instance->DR;
        length--;

        if(__HAL_I2C_GET_FLAG(&mpr121, I2C_FLAG_BTF) == SET)
        {
          /* Read data from DR */
          (*buffer++) = mpr121.Instance->DR;
          length--;
        }
      }
    }

    mpr121.State = HAL_I2C_STATE_READY;
    mpr121.Mode = HAL_I2C_MODE_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(&mpr121);
		
		extractTouchState();
		extractiSensorValue(arrSensorValue);
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}
void extractTouchState(void){
	uint8_t LSB = 0;
	uint8_t MSB = 0;
	volatile uint16_t result = 0;
	
	LSB = arrSensor_Buffer[0];
	MSB = arrSensor_Buffer[1];
	
	result = MSB;
	result <<= 8;
	result |= LSB;
	
	touchState = result;
}

SensorValue_Status extractiSensorValue(volatile uint16_t *arrSensorValue){
	
	uint8_t index = 0;
	uint8_t LSB = 0;
	uint8_t MSB = 0;
	uint16_t result = 0;
	
	if(arrSensorValue == NULL){
		return Value_Destination_NULL;
	}
	
	for(index = SENSOR_VALUE; index < BUFFER_SIZE; index+=2){
		LSB = arrSensor_Buffer[index / 2];
		MSB = arrSensor_Buffer[(index + 1) / 2];
		
		result = MSB;
		result <<= 8;
		result |= LSB;
		
		arrSensorValue[(index - SENSOR_VALUE)/2] = result;
	}
	
	return Value_OK;
}

uint16_t whichTouch(void){
	return touchState;
}
