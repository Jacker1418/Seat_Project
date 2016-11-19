#ifndef __UART_H__
#define __UART_H__

#include "stm32f1xx_hal_def.h"

//#define USE_CRC

#define START 0xFF
#define END 0xFE

#define QUEUE_SIZE 64
#define UART_BUFFER_SIZE 64
typedef enum{
	Mode = 0,
	Year = 1,
	Month = 2,
	Day = 3,
	Hour = 4,
	Minute = 5,
#ifdef USE_CRC
	CRC_16,
#endif
	Length
}Packet_Index;

typedef enum{
	Normal = 0,
	Real_Time = 1,
	Response = 2,
	None = 3
}UART_Modes;

typedef enum{
	Packet_Ok = 0,
	Packert_CRC_Error,
	Packet_NULL,
	Packet_Length_NULL,
	Packet_Date_NULL,
	Packet_Value_NULL,
	Packet_Mode_NULL,
	Packet_Dropout,
#ifdef USE_CRC
	Packet_CRC_NULL,
#endif
	RTC_NULL,
	Message_NULL
}Packet_Status;

typedef enum{
	Queue_OK,
	Queue_Full,
	Queue_Empty
}Queue_Status;

struct Date{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
};

struct Packet{
	uint8_t mode;
	struct Date date;
	uint16_t sensorValue[9];
	float position[2];
#ifdef USE_CRC
	uint16_t crc;
#endif
	uint8_t length;
};

static volatile struct Packet queuePacket[QUEUE_SIZE];
static volatile uint8_t head;
static volatile uint8_t tail;
static volatile uint16_t countQueue;

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


void UART_Console_Init(void);
void UART_Bluetooth_Init(void);

void Queue_Init(void);

Queue_Status Pop(struct Packet* packet);
Queue_Status Push(UART_Modes mode, RTC_HandleTypeDef* rtc, uint16_t* sensorValues, float* position);
uint8_t howManyItem(void);
Queue_Status checkTime_Items(RTC_HandleTypeDef* myRTC, struct Date* androidRTC);

HAL_StatusTypeDef Bluetooth_Send_Packet_Polling( struct Packet* packet ,uint32_t Timeout);
HAL_StatusTypeDef Bluetooth_Send_Polling( uint8_t* buffer, uint32_t Timeout);
HAL_StatusTypeDef Bluetooth_Receive_IT(volatile uint8_t* buffer, uint32_t length);

Packet_Status makePacket(volatile struct Packet* packet, UART_Modes mode, RTC_HandleTypeDef* rtc, uint16_t* sensorValues, float* position);
Packet_Status releasePacket(volatile struct Packet* packet, uint8_t* message, uint8_t length);

Packet_Status packetToArray(uint8_t* destination, struct Packet* packet);
Packet_Status messageToLength(uint8_t* message, struct Packet* packet, uint16_t* index);
Packet_Status messageToDate(uint8_t* message, struct Packet* packet, uint16_t* index);
Packet_Status messageToSensorValue(uint8_t* message, struct Packet* packet, uint16_t* index);
Packet_Status messageToPosition(uint8_t* message, struct Packet* packet, uint16_t* index);
Packet_Status messageToCRC(uint8_t* message, struct Packet* packet, uint16_t* index);

void printPacket(volatile struct Packet *packet);
#endif
