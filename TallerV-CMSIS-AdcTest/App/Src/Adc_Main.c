/*
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
#include "AdcDriver.h"

// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
BasicTimer_Handler_t handlerTim5			= {0};
EXTI_Config_t ExtiButton 					= {0};

/*Elemento para hacer la comunicación serial*/

GPIO_Handler_t handlerPinTx					= {0};
GPIO_Handler_t handlerPinRx					= {0};
USART_Handler_t usart2Comm 					= {0};
uint8_t sendMsg = 0;
uint8_t rxData = 0;
char bufferData[64] = "Iniciando " ;


/* Elementos para el Adc */
ADC_Config_t handlerAdc						= {0};
uint16_t dataADC = 0;
uint8_t adcIsComplete = 0;


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

			sprintf(bufferData, "data: %u /n", dataADC);
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

	usart2Comm.ptrUSARTx								= USART2;
	usart2Comm.USART_Config.USART_baudrate				= USART_BAUDRATE_19200;
	usart2Comm.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity				= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_mode					= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_enableIntRX			= USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX			= USART_TX_INTERRUPT_ENABLE;
	USART_Config(&usart2Comm);

	//Configuración ADC
	handlerAdc.channel									= ADC_CHANNEL_0;
	handlerAdc.dataAlignment							= ADC_ALIGNMENT_RIGHT;
	handlerAdc.samplingPeriod							= ADC_SAMPLING_PERIOD_84_CYCLES;
	handlerAdc.resolution								= ADC_RESOLUTION_12_BIT;
	adc_Config(&handlerAdc);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

void BasicTimer5_Callback(void){
	startSingleADC();
}

void callback_extInt13(void){
	__NOP();
}

void usart2Rx_Callback(void){

	rxData = getRxData();
}

void adcComplete_Callback(void){

	dataADC = getADC();

	adcIsComplete = 1;
}
