/*
 * RTCDriver.h
 *
 *  Created on: Jun 7, 2023
 *      Author: sjacome
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include <stm32f4xx.h>

#define RTC_KEY1	0XCA
#define RTC_KEY2	0x53

typedef struct
{
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
	uint16_t year;
	uint8_t month;
	uint8_t day;
} RTC_Config_t;


void RTC_config(RTC_Config_t *ptrhandlerRTC);
void writeRegisterRTC(RTC_Config_t *ptrhandlerRTC);
uint8_t decToBCD(int numberDec);
uint8_t bcdToDec(uint8_t numberBCD);
uint8_t* readTime(void);
uint8_t* readDate(void);
void changeTime(RTC_Config_t *ptrhandlerRTC, int hora, int minutos);
void changeDate(RTC_Config_t *ptrhandlerRTC, int dia, int mes, int año);

#endif /* RTCDRIVER_H_ */
