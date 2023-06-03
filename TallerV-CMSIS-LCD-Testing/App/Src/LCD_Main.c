/*
 * AccelTest_Main.c
 *
 *  Created on: Apr 8, 2023
 *      Author: sjacome
 */

#include <stm32f4xx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <GPIOxDriver.h>
#include <USARTxDriver.h>
#include <BasicTimer.h>
#include <SysTickDriver.h>
#include <I2CDriver.h>
#include <LcdDriver.h>

/* Definción de Variable */
GPIO_Handler_t BlinkyPin = {0};
GPIO_Handler_t UserButton = {0};
GPIO_Handler_t PinTX = {0};
GPIO_Handler_t PinRX = {0};

BasicTimer_Handler_t BlinkyTimer = {0};
USART_Handler_t CommTerminal = {0};

uint8_t rxData = 0;
char bufferData[64] = "Accel testing...";

/* Configuración para el I2C */
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};

//Configuración LCD
GPIO_Handler_t handlerI2cSDA2 = {0};
GPIO_Handler_t handlerI2cSCL2 = {0};
I2C_Handler_t handlerLCD = {0};
char bufferLCDx[64] = {0};
char bufferLCDy[64] = {0};
char bufferLCDz[64] = {0};

#define ACCEL_ADDRESS          	 0x1D
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55
#define LCD_ADDRESS				 0b0100100

#define POWER_CTL                45
#define WHO_AM_I                 0

/* Definicion de prototipos de funciones */
void initSystem(void);
void staticLCD(void);

int main (void)
{
	initSystem();
//	i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);

	//Imprimir un mensaje de inicio
	writeMsg(&CommTerminal, bufferData);

	while(1){

		staticLCD();
	}
	return(0);

}

void staticLCD(void){
ResetScreenLCD(&handlerLCD);
InitLCD(&handlerLCD);
delay_ms(10);
cleanLCD(&handlerLCD);
delay_ms(10);
moveCursorLCD(&handlerLCD, 0, 1);
sendMsgLCD(&handlerLCD, "Ax = ");
moveCursorLCD(&handlerLCD, 1, 1);
sendMsgLCD(&handlerLCD, "Ay = ");
moveCursorLCD(&handlerLCD, 2, 1);
sendMsgLCD(&handlerLCD, "Az = ");
moveCursorLCD(&handlerLCD, 3, 1);
sendMsgLCD(&handlerLCD, "Sensi = 16834 bit/g");
}

/* Definición de funciones */
void initSystem(void){

	//Configuracion de los handlers
	BlinkyPin.pGPIOx                                 = GPIOA;
	BlinkyPin.GPIO_PinConfig.GPIO_PinNumber          = PIN_5;
	BlinkyPin.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_OUT;
	BlinkyPin.GPIO_PinConfig.GPIO_PinOType           = GPIO_OTYPE_PUSHPULL;
	BlinkyPin.GPIO_PinConfig.GPIO_PinSpeed           = GPIO_OSPEED_FAST;
	BlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	GPIO_Config(&BlinkyPin);

	UserButton.pGPIOx                                 = GPIOC;
	UserButton.GPIO_PinConfig.GPIO_PinNumber          = PIN_13;
	UserButton.GPIO_PinConfig.GPIO_PinMode            = GPIO_MODE_IN;
	UserButton.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	GPIO_Config(&UserButton);

	//Configuración de la comunicación serial

	//Configuracion de transmision
	PinTX.pGPIOx                                     = GPIOA;
	PinTX.GPIO_PinConfig.GPIO_PinNumber              = PIN_2;
	PinTX.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_ALTFN;
	PinTX.GPIO_PinConfig.GPIO_PinOType				 = GPIO_OTYPE_PUSHPULL;
	PinTX.GPIO_PinConfig.GPIO_PinPuPdControl		 = GPIO_PUPDR_NOTHING;
	PinTX.GPIO_PinConfig.GPIO_PinSpeed				 = GPIO_OSPEED_FAST;
	PinTX.GPIO_PinConfig.GPIO_PinAltFunMode          = AF7;
	GPIO_Config(&PinTX);

	//Configuracion recepción
	PinRX.pGPIOx                                     = GPIOA;
	PinRX.GPIO_PinConfig.GPIO_PinNumber              = PIN_3;
	PinRX.GPIO_PinConfig.GPIO_PinMode                = GPIO_MODE_ALTFN;
	PinRX.GPIO_PinConfig.GPIO_PinOType				 = GPIO_OTYPE_PUSHPULL;
	PinRX.GPIO_PinConfig.GPIO_PinPuPdControl		 = GPIO_PUPDR_NOTHING;
	PinRX.GPIO_PinConfig.GPIO_PinSpeed				 = GPIO_OSPEED_FAST;
	PinRX.GPIO_PinConfig.GPIO_PinAltFunMode          = AF7;
	GPIO_Config(&PinRX);

	CommTerminal.ptrUSARTx                             = USART2;
	CommTerminal.USART_Config.USART_baudrate           = USART_BAUDRATE_115200;
	CommTerminal.USART_Config.USART_datasize           = USART_DATASIZE_8BIT;
	CommTerminal.USART_Config.USART_parity             = USART_PARITY_NONE;
	CommTerminal.USART_Config.USART_stopbits           = USART_STOPBIT_1;
	CommTerminal.USART_Config.USART_mode               = USART_MODE_RXTX;
	CommTerminal.USART_Config.USART_enableIntRX        = USART_RX_INTERRUPT_ENABLE;
	CommTerminal.USART_Config.USART_enableIntTX        = USART_TX_INTERRUPT_DISABLE;
	USART_Config(&CommTerminal);

	//Configuracion de los timers
	BlinkyTimer.ptrTIMx                              = TIM2;
	BlinkyTimer.TIMx_Config.TIMx_mode                = BTIMER_MODE_UP;
	BlinkyTimer.TIMx_Config.TIMx_speed               = BTIMER_SPEED_100us;
	BlinkyTimer.TIMx_Config.TIMx_period              = 2500;
	BlinkyTimer.TIMx_Config.TIMx_interruptEnable     = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&BlinkyTimer);

	//Configuración I2C Accel
	I2cSCL.pGPIOx                                    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSCL);

	I2cSDA.pGPIOx                                    = GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSDA);

	Accelerometer.ptrI2Cx                            = I2C1;
	Accelerometer.modeI2C                            = I2C_MODE_FM;
	Accelerometer.slaveAddress                       = ACCEL_ADDRESS;
	i2c_config(&Accelerometer);

	//Configuración I2C LCD
	handlerI2cSCL2.pGPIOx = GPIOB;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSCL2);

	handlerI2cSDA2.pGPIOx = GPIOB;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_HIGH;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinAltFunMode = AF9;
	GPIO_Config(&handlerI2cSDA2);

	handlerLCD.ptrI2Cx = I2C2;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	i2c_config(&handlerLCD);


}

/* Interrupciones */
void usart2Rx_Callback(void){
	//Leemo el valor del registro DR, donde se encuentra almacenado el dato que llega
	// ESto además debe bajar la bandera de la interrupción
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&BlinkyPin);
}



