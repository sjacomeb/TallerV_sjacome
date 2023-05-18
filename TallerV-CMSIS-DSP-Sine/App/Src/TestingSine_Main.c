/*
 *
 *  Created on: Apr 8, 2023
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

#include "arm_math.h"

// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
EXTI_Config_t ExtiButton 					= {0};

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX     = {0};
GPIO_Handler_t handlerPinRX     = {0};
USART_Handler_t usart2Comm      = {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;
char bufferMsg[64] = {0};

/* Arreglos para pruebas con librerias de CMSIS*/
float32_t srcNumber[4] = {-0.987, 32.26, -45.21, -987.321};
float32_t destNumber[4] = {0};
uint32_t dataSize = 0;

/* Para utilizar la funcion seno */
float32_t sineValue = 0.0;
float32_t sineArgValue = 0.0;

//Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){

	/* Activamos el coprocesador matematico*/
	SCB->CPACR |= (0xF << 20);


	//Inicializamos todos los elementos del sistema
	initSystem();

	while(1){

		/* Realiza operacion de punto flotante cuando se escribe
		 * algun caracter por el puerto serial
		 * */
		if(usart2DataReceived == 'A'){

			dataSize = 4;

			/* Ejecutando la funcion para obtener el valor absoluto */
			arm_abs_f32(srcNumber, destNumber, dataSize);

			for(int j = 0; j<4; j++){

				sprintf(bufferMsg, "Valor absoluto de %#.2f = %#.2f \n ", srcNumber[j], destNumber[j]);
				writeMsg(&usart2Comm, bufferMsg);

			}

			usart2DataReceived = '\0';
		}

		/* Para probar el seno */
		if(usart2DataReceived == 'B'){

			sineArgValue = M_PI/4;

			//La funcion recibe un valor en radianes

			sineValue = arm_sin_f32(sineArgValue);
			sprintf(bufferMsg, "Seno(%#.2f) = %#.6f \n ", sineArgValue, sineValue);
			writeMsg(&usart2Comm, bufferMsg);

			usart2DataReceived = '\0';

		}

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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
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

	/*Configuración de la comunicación serial */
	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber  			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx									= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_19200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX       		= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUPT_DISABLE;

	USART_Config(&usart2Comm);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

void callback_extInt13(void){
	__NOP();
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2 */
void usart2Rx_Callback(void){

	usart2DataReceived = getRxData();
}
