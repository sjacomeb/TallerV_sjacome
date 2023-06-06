/*
 * mainTarea_3.c
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
#include "PLLDriver.h"

#include "arm_math.h"

// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
GPIO_Handler_t handlerPinMCO1				= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
PLL_Config_t pll							= {0};


//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX     = {0};
GPIO_Handler_t handlerPinRX     = {0};
USART_Handler_t usartComm      = {0};
uint8_t sendMsg = 0;
uint8_t rxData = 0;
char bufferReception[64] = {0};

/* Variables y arreglos*/
char cmd[64];
unsigned int firstParameter;
unsigned int secondParameter;
char userMsg[64];
uint8_t counterReception = 0;
bool stringComplete = false;

//Definición de las cabeceras de las funciones del main
void initSystem(void);
void comandos(char *ptrBufferReception);

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

//	configPLL(&pll);

	while(1){

		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			if(rxData == '*'){
				stringComplete = true;

				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			rxData = '\0';
		}

		if(stringComplete){
			comandos(bufferReception);
			stringComplete = false;
		}
	}

	return 0;

}

void comandos(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	//Este comando imprime una lista con los otros comandos que tiene el equipo
	if(strcmp(cmd,"help") == 0){

		writeMsg(&usartComm, "Help Menu CMDs: \n");
		writeMsg(&usartComm, "1) help           -- Imprime este menu \n");
		writeMsg(&usartComm, "2) selectClock    -- Selecciona la señal: HSI=0, LSE=1, PLL=3 \n");
		writeMsg(&usartComm, "3) selectPreescaler   --Selecciona preescaler: 2,3,4,5 \n");

	}
	//Permite elegir la señal en el MCO1
	else if(strcmp(cmd, "selectClock") == 0){
		if(firstParameter == 0){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected HSI \n");
		}else if(firstParameter == 1){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected LSE \n");
		}else if(firstParameter == 3){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected PLL \n");
		}else{
			writeMsg(&usartComm, "Wrong selection \n");
		}
	}
	//Permite seleccionar el preescaler de la señal en el MCO1
	else if(strcmp(cmd, "selectPreescaler") == 0){
		if(firstParameter == 2){
			updatePreescaler(&pll, firstParameter);
			writeMsg(&usartComm, "Division by 2 \n");
		}else if(firstParameter == 3){
			updatePreescaler(&pll, firstParameter);
			writeMsg(&usartComm, "Division by 3 \n");
		}else if(firstParameter == 4){
			updatePreescaler(&pll, firstParameter);
			writeMsg(&usartComm, "Division by 4 \n");
		}else if(firstParameter == 5){
			updatePreescaler(&pll, firstParameter);
			writeMsg(&usartComm, "Division by 5 \n");
		}else{
			writeMsg(&usartComm, "Wrong parameter \n");
		}


	}
	else{
		writeMsg(&usartComm, "Wrong CMD \n");
	}
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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100MHz_10us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración MCO1 (probar señal) */
	handlerPinMCO1.pGPIOx								= GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;
	GPIO_Config(&handlerPinMCO1);

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

	usartComm.ptrUSARTx										= USART2;
	usartComm.USART_Config.USART_baudrate					= USART_BAUDRATE_19200;
	usartComm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usartComm.USART_Config.USART_parity						= USART_PARITY_NONE;
	usartComm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usartComm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usartComm.USART_Config.USART_enableIntRX       			= USART_RX_INTERRUPT_ENABLE;
	usartComm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usartComm);

	/* Configuración PLL */
	pll.PLLN 			= 100;
	pll.PLLM			= 8;
	pll.PLLP			= PLLP_2;
	pll.MC01PRE			= MCO1PRE_4;

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

	rxData = getRxData();
}
