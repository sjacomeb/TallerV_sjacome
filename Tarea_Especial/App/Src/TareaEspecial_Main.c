/*
 * Solución Tarea Especial
 *  Created on: May 22, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"


// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};

BasicTimer_Handler_t handlerBlinkyTimer 	= {0};

EXTI_Config_t ExtiButton 					= {0};

/*Elemento para hacer la comunicación serial*/

GPIO_Handler_t handlerPinTx					= {0};
GPIO_Handler_t handlerPinRx					= {0};
GPIO_Handler_t handlerPinMCO1				= {0};
USART_Handler_t usartComm 					= {0};

//Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){

	//Inicializamos todos los elementos del sistema
	 initSystem();

	 configPLL();


	while(1){


	}

	return 0;

}

void initSystem(void){


	/* Configuración del pin para el Led_Blinky */
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	/* Configuración del TIM2 para controlar el blinky */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_80MHz_10us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 1250;					//250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//El pin UserButton es una entrada simple que entregará la interrupción EXTI-13
	/* Configuración del UserButton */
	handlerUserButton.pGPIOx 								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	ExtiButton.pGPIOHandler   = &handlerUserButton;    //Puntero a la estructura
	ExtiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&ExtiButton);

	/* Configuracion de la comunicación serial */
	handlerPinTx.pGPIOx 								= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinTx);

	handlerPinRx.pGPIOx 								= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinRx);

	/* Configuración para probar el MCO1 en el analizador de señales */
	handlerPinMCO1.pGPIOx								= GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;
	GPIO_Config(&handlerPinMCO1);

	//USART 1
	usartComm.ptrUSARTx									= USART1;
	usartComm.USART_Config.USART_baudrate				= USART_BAUDRATE_115200;
	usartComm.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	usartComm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usartComm.USART_Config.USART_mode					= USART_MODE_RXTX;
	usartComm.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	usartComm.USART_Config.USART_enableIntRX			= USART_RX_INTERRUPT_DISABLE;
	usartComm.USART_Config.USART_enableIntTX			= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usartComm);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

void callback_extInt13(void){
	__NOP();
}



