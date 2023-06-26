/*
 * LCD_Driver.h
 *
 *  Created on: Jun 25, 2023
 *      Author: sjacome
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <I2CDriver.h>
#include <SysTickDriver.h>

//Definicion de Macros
#define RS_SELECT_ID         (0b0 << 0)
#define RS_SELECT_DR         (0b1 << 0)
#define RW_SELECT_WRITE      (0b0 << 1)
#define RW_SELECT_READ       (0b1 << 1)
#define EN_DISABLED          (0b0 << 2)
#define EN_ENABLE            (0b1 << 2)
#define LED_DISABLED         (0b0 << 3)
#define LED_ENABLE           (0b1 << 3)

#define	ENTER_MODE_4_BIT     (0b0011 << 4)
#define	RF_MODE_4_BIT        (0b0010 << 4)
#define	RF_LINE_CHARACTER    (0b00101000)
#define	EM_CURSOR_RIGHT      (0b00000110)
#define	ADD                  (0b1000 << 4)

#define BITS_H                (0b1111 << 4)
#define BITS_L                (0b1111)

#define	TURN_ON_DISPLAY      (0b00001000)
#define	DIPLAY_ENABLE        (0b00001100)
#define	CURSOR_BLINKY        (0b00001111)
#define	CLEAR_DISPLAY        (0b00000001)

//Definicion de las cabeceras de las funciones
void lcd_i2c_init(I2C_Handler_t *ptrHandlerLCDI2C);
void lcd_i2c_clear(I2C_Handler_t *ptrHandlerLCDI2C);
void lcd_i2c_cursor_blinky_Enable(I2C_Handler_t *ptrHandlerLCDI2C);
void lcd_i2c_cursor_blinky_Disabled(I2C_Handler_t *ptrHandlerLCDI2C);
void lcd_i2c_gotoxy(I2C_Handler_t *ptrHandlerLCDI2C, uint8_t row, uint8_t column);
void lcd_i2c_data(I2C_Handler_t *ptrHandlerLCDI2C, char data);
void lcd_i2c_putc(I2C_Handler_t *ptrHandlerLCDI2C, char *MsgtoSend);

#endif /* LCD_DRIVER_H_ */
