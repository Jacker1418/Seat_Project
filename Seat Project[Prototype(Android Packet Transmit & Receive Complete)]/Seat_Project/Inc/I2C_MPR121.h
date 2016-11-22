#ifndef __I2C_MPR121_H__
#define __I2C_MPR121_H__

#include "stm32f1xx_hal_def.h"

#define MASTER_ADDRESS        					0x30F
#define MPR121_ADDRESS 				 					0x5A

/* I2C SPEEDCLOCK define to max value: 100K KHz on STM32F1xx*/
#define I2C_SPEEDCLOCK   								100000
#define I2C_DUTYCYCLE    									I2C_DUTYCYCLE_2

#define I2C_TIMEOUT_FLAG          				((uint32_t)35)     /*!< Timeout 35 ms */
#define I2C_TIMEOUT_ADDR_SLAVE    ((uint32_t)10000)  /*!< Timeout 10 s  */
#define I2C_TIMEOUT_BUSY_FLAG     	((uint32_t)10000)  /*!< Timeout 10 s  */

#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32

#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define AFE_CFG 0x5C
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E

#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A

#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F

// Global Constants
#define TOU_THRESH	0x06
#define	REL_THRESH	0x0A

#define TOUCH_VALUE	0x00
#define SENSOR_VALUE 0x04

#define BUFFER_SIZE 22
#define COUNT_KEY 9

#define TOUCH_STATE_MSK0 0x01
#define TOUCH_STATE_MSK1 0x02
#define TOUCH_STATE_MSK2 0x04
#define TOUCH_STATE_MSK3 0x08
#define TOUCH_STATE_MSK4 0x10
#define TOUCH_STATE_MSK5 0x20
#define TOUCH_STATE_MSK6 0x40
#define TOUCH_STATE_MSK7 0x80
#define TOUCH_STATE_MSK8 0x100

typedef enum{
	Value_OK = 0,
	Value_NULL,
	Value_Destination_NULL,
	Value_Size_Non
}SensorValue_Status;

typedef enum{
	TOUCH_0,
	TOUCH_1,
	TOUCH_2,
	TOUCH_3,
	TOUCH_4,
	TOUCH_5,
	TOUCH_6,
	TOUCH_7,
	TOUCH_8
}Touch_Status;


void I2C_MPR121_Init(void);

HAL_StatusTypeDef MPR121_Configuration(void);

HAL_StatusTypeDef MPR121_Send_Polling(uint8_t reg, uint8_t value, uint32_t Timeout);

HAL_StatusTypeDef MPR121_Receive_1byte_Polling(volatile uint16_t *arrSensorValue,uint8_t reg, uint32_t Timeout);
HAL_StatusTypeDef MPR121_Receive_2byte_Polling(volatile uint16_t *arrSensorValue,uint8_t reg, uint32_t Timeout);
HAL_StatusTypeDef MPR121_Receive_All_Key_Polling(volatile uint16_t *arrSensorValue, uint32_t Timeout);
HAL_StatusTypeDef MPR121_Receive(volatile uint16_t* arrSensorValue, uint8_t reg, uint32_t length, uint32_t Timeout);

void extractTouchState(void);
SensorValue_Status extractiSensorValue(volatile uint16_t *destination);
uint16_t whichTouch(void);
#endif
