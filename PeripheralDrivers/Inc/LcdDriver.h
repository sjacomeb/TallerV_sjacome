/*
 * LcdDriver.h
 *
 *  Created on: May 28, 2023
 *      Author: sjacome
 */

#ifndef LCDDRIVER_H_
#define LCDDRIVER_H_

#include <stm32f4xx.h>
#include "I2CDriver.h"
#include "SysTickDriver.h"

#define I2C_WRITE_DATA						0
#define I2C_READ_DATA						1

#define MAIN_CLOCK_4_MHz_FOR_I2C			4
#define MAIN_CLOCK_16_MHz_FOR_I2C			16
#define MAIN_CLOCK_20_MHz_FOR_I2C			20

#define I2C_MODE_SM			0
#define I2C_MODE_FM			1

//#define I2C_MODE_SM_SPEED_100KHz			80
//#define I2C_MODE_FM_SPEED_400KHz			14

#define I2C_MAX_RISE_TIME_SM				17
#define I2C_MAX_RISE_TIME_FM				6


void initLCD(I2C_Handler_t *ptrHandlerI2C);
void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd);
void sendataLCD (I2C_Handler_t *ptrHandlerI2C, char data);
void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str);
void clearLCD (I2C_Handler_t *ptrHandlerI2C);
void setCursorLCD (I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y);
void clearScreenLCD(I2C_Handler_t *ptrHandlerI2C);
void writeDataLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);

#define LCD_ADDRESS		0b0100110



#endif /* LCDDRIVER_H_ */
