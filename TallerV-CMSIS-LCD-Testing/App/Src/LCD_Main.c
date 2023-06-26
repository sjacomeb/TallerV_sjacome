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
//#include <LCD_Driver.h>
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

/* Configuracion para el I2C de la LCD*/
GPIO_Handler_t handlerI2cSDA2 = {0};
GPIO_Handler_t handlerI2cSCL2 = {0};
I2C_Handler_t handlerLCD = {0};

char bufferLCDx[64] = {0};
char bufferLCDy[64] = {0};
char bufferLCDz[64] = {0};


#define LCD_ADDRESS		0x26


/* Definicion de prototipos de funciones */
void initSystem(void);
void staticLCD(void);

int main (void){

	initSystem();
	config_SysTick_ms(0);

	staticLCD();

	while(1){

	}
	return(0);

}

void staticLCD(void){

//	lcd_i2c_init(&handlerLCD);
//	lcd_i2c_gotoxy(&handlerLCD, 1, 5);
//	lcd_i2c_putc(&handlerLCD, "Iniciando programa");
//	delay_ms(2000);
//	lcd_i2c_clear(&handlerLCD);


//	clearScreenLCD(&handlerLCD);
//	init_LCD(&handlerLCD);
//	clearLCD(&handlerLCD);
//	delay_ms(10);
//
//	moveCursor_inLCD(&handlerLCD, 0, 1);
//	sendSTR_toLCD(&handlerLCD, "Mensaje prueba");


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

	/* Configurando los pines sobre los que funciona el I2C2*/
	handlerI2cSCL2.pGPIOx                            = GPIOB;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinNumber     = PIN_8;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinMode       = GPIO_MODE_ALTFN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinOType     = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinSpeed      = GPIO_OSPEED_HIGH;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSCL2);

	handlerI2cSDA2.pGPIOx = GPIOB;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinNumber     = PIN_9;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinMode       = GPIO_MODE_ALTFN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinOType     = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinSpeed      = GPIO_OSPEED_HIGH;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSDA2);

	handlerLCD.ptrI2Cx = I2C1;
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



