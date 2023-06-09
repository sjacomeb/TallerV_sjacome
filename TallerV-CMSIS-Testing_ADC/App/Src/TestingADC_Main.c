/*
 * mainTarea_3.c
 *
 *  Created on: Apr 8, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"

// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
BasicTimer_Handler_t handlerTim5			= {0};
EXTI_Config_t ExtiButton 					= {0};

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX     = {0};
GPIO_Handler_t handlerPinRX     = {0};
USART_Handler_t usart2Comm      = {0};
uint8_t sendMsg = 0;
uint8_t rxData = 0;

char bufferData[64] = "Iniciando \n";

/* Elementos para el Adc */
ADC_Config_t handlerAdc	= {0};
uint8_t adcIsComplete = 0;
uint8_t adcCounter= 0;
uint16_t dataADC[] = {0};


#define numberOfChannels	2

//Definición de las cabeceras de las funciones del main
void initSystem(void);

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	writeMsg(&usart2Comm, bufferData);

	while(1){

		if(rxData != '\0'){

			if(rxData == 's'){

				startSingleADC();
			}

		}

		if(adcIsComplete == 1){

			sprintf(bufferData,"%u\t%u\n",dataADC[0],dataADC[1]);
			writeMsg(&usart2Comm, bufferData);

			adcIsComplete = 0;
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

	/* Configuración TIM5 */
	handlerTim5.ptrTIMx 							= TIM5;
	handlerTim5.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTim5.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerTim5.TIMx_Config.TIMx_period				= 2500;
	handlerTim5.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerTim5);

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
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX       		= USART_RX_INTERRUPT_ENABLE; //Deshabilitados los dos veo los
	usart2Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUPT_DISABLE; //los datos de las funciones sin interrupciones

	USART_Config(&usart2Comm);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

void BasicTimer5_Callback(void){
	startSingleADC();
}

void adcComplete_Callback(void){

	dataADC[adcCounter] = getADC();
	if(adcCounter < (numberOfChannels-1)){
		adcCounter++;
	}
	else{
		adcIsComplete = 1;
		adcCounter = 0;
	}
}

/* Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2 */
void usart2Rx_Callback(void){

	rxData = getRxData();

}
