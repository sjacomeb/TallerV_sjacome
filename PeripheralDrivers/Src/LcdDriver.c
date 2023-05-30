/*
 * LcdDriver.c
 *
 *  Created on: May 28, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "LcdDriver.h"
#include "SysTickDriver.h"

#include <stdint.h>
#include "LcdDriver.h"

void LCD_sendCMD (I2C_Handler_t *ptrHandlerI2C, char cmd){
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(cmd & 0xf0);
	_L=((cmd<<4) & 0xf0);
	_T[0] = _U|0x0C;
	writeDataLCD(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x08;
	writeDataLCD(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0C;
	writeDataLCD(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x08;
	writeDataLCD(ptrHandlerI2C, _T[3]);
}

void writeDataLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){

	/* 1. Generamos la condición de Start*/
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Enviamos la diracción del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Enviamos la dirección de memoria que deseamos escribir*/
	i2c_sendDataByte(ptrHandlerI2C, dataToWrite);

	i2c_stopTransaction(ptrHandlerI2C);
}


void sendataLCD (I2C_Handler_t *ptrHandlerI2C, char data){
	char _U;
	char _L;
	uint8_t _T[4];
	_U=(data & 0xf0);
	_L=((data<<4) & 0xf0);
	_T[0] = _U|0x0D;
	writeDataLCD(ptrHandlerI2C, _T[0]);
	_T[1] = _U|0x09;
	writeDataLCD(ptrHandlerI2C, _T[1]);
	_T[2] = _L|0x0D;
	writeDataLCD(ptrHandlerI2C, _T[2]);
	_T[3] = _L|0x09;
	writeDataLCD(ptrHandlerI2C, _T[3]);
}

void clearLCD (I2C_Handler_t *ptrHandlerI2C) {
//	LCD_sendata (ptrHandlerI2C, 0x00);
	LCD_sendCMD(ptrHandlerI2C, 0x01);
	delay_ms(50);
}

void initLCD (I2C_Handler_t *ptrHandlerI2C) {

	// Delay de inizializacion
	delay_ms(50);
	// Primer 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay_ms(5);
	// Segundo 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);
	delay_ms(1);
	// Tercer 0x30 para BF
	LCD_sendCMD (ptrHandlerI2C, 0x30);

	// Delay después de la secuencia inicial de inicializacion
	delay_ms(50);

	/*
	 * Configuraciones para la escritura
	 */
	// Data lenght 4, lines 2, character font 5X8
	LCD_sendCMD (ptrHandlerI2C, 0x20);
	delay_ms(50);
	LCD_sendCMD (ptrHandlerI2C, 0x28);
	delay_ms(50);
	// Display off
	LCD_sendCMD (ptrHandlerI2C, 0x08);
	delay_ms(50);
	// Display Clear
	LCD_sendCMD (ptrHandlerI2C, 0x01);
	delay_ms(50);
	// Entry mode
	LCD_sendCMD (ptrHandlerI2C, 0x06);

	delay_ms(50);
	// Delay para encendido
	LCD_sendCMD (ptrHandlerI2C, 0x0C);
}

void LCD_sendSTR(I2C_Handler_t *ptrHandlerI2C, char *str) {
	while (*str) sendataLCD (ptrHandlerI2C, *str++);
}
void setCursorLCD(I2C_Handler_t *ptrHandlerI2C, uint8_t x, uint8_t y) {
	uint8_t cursor;
	switch (y) {
	case 0 :
		switch (x) {
		case 0 : cursor = 0x00; break;
		case 1 : cursor = 0x01; break;
		case 2 : cursor = 0x02; break;
		case 3 : cursor = 0x03; break;
		case 4 : cursor = 0x04; break;
		case 5 : cursor = 0x05; break;
		case 6 : cursor = 0x06; break;
		case 7 : cursor = 0x07; break;
		case 8 : cursor = 0x08; break;
		case 9 : cursor = 0x09; break;
		case 10 : cursor = 0x0A; break;
		case 11 : cursor = 0x0B; break;
		case 12 : cursor = 0x0C; break;
		case 13 : cursor = 0x0D; break;
		case 14 : cursor = 0x0E; break;
		case 15 : cursor = 0x0F; break;
		case 16 : cursor = 0x10; break;
		case 17 : cursor = 0x11; break;
		case 18 : cursor = 0x12; break;
		case 19 : cursor = 0x13; break;
		} break;

	case 1 :
		switch (x) {
		case 0 : cursor = 0x40; break;
		case 1 : cursor = 0x41; break;
		case 2 : cursor = 0x42; break;
		case 3 : cursor = 0x43; break;
		case 4 : cursor = 0x44; break;
		case 5 : cursor = 0x45; break;
		case 6 : cursor = 0x46; break;
		case 7 : cursor = 0x47; break;
		case 8 : cursor = 0x48; break;
		case 9 : cursor = 0x49; break;
		case 10 : cursor = 0x4A; break;
		case 11 : cursor = 0x4B; break;
		case 12 : cursor = 0x4C; break;
		case 13 : cursor = 0x4D; break;
		case 14 : cursor = 0x4E; break;
		case 15 : cursor = 0x4F; break;
		case 16 : cursor = 0x50; break;
		case 17 : cursor = 0x51; break;
		case 18 : cursor = 0x52; break;
		case 19 : cursor = 0x53; break;
		} break;

	case 2 :
		switch (x) {
		case 0 : cursor = 0x14; break;
		case 1 : cursor = 0x15; break;
		case 2 : cursor = 0x16; break;
		case 3 : cursor = 0x17; break;
		case 4 : cursor = 0x18; break;
		case 5 : cursor = 0x19; break;
		case 6 : cursor = 0x1A; break;
		case 7 : cursor = 0x1B; break;
		case 8 : cursor = 0x1C; break;
		case 9 : cursor = 0x1D; break;
		case 10 : cursor = 0x1E; break;
		case 11 : cursor = 0x1F; break;
		case 12 : cursor = 0x20; break;
		case 13 : cursor = 0x21; break;
		case 14 : cursor = 0x22; break;
		case 15 : cursor = 0x23; break;
		case 16 : cursor = 0x24; break;
		case 17 : cursor = 0x25; break;
		case 18 : cursor = 0x26; break;
		case 19 : cursor = 0x27; break;
		} break;

	case 3 :
		switch (x) {
		case 0 : cursor = 0x54; break;
		case 1 : cursor = 0x55; break;
		case 2 : cursor = 0x56; break;
		case 3 : cursor = 0x57; break;
		case 4 : cursor = 0x58; break;
		case 5 : cursor = 0x59; break;
		case 6 : cursor = 0x5A; break;
		case 7 : cursor = 0x5B; break;
		case 8 : cursor = 0x5C; break;
		case 9 : cursor = 0x5D; break;
		case 10 : cursor = 0x5E; break;
		case 11 : cursor = 0x5F; break;
		case 12 : cursor = 0x60; break;
		case 13 : cursor = 0x61; break;
		case 14 : cursor = 0x62; break;
		case 15 : cursor = 0x63; break;
		case 16 : cursor = 0x64; break;
		case 17 : cursor = 0x65; break;
		case 18 : cursor = 0x66; break;
		case 19 : cursor = 0x67; break;
		} break;
	}
	LCD_sendCMD(ptrHandlerI2C, 0x80|cursor);
}

void clearScreenLCD(I2C_Handler_t *ptrHandlerI2C){
	for(int i=0 ; i <80 ;i++){
		LCD_sendSTR(ptrHandlerI2C, "  ");
	}
}
