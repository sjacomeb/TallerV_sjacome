/*
 * main.c
 *
 *  Created on: Apr 3, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* Definicion de los elementos */

GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};
GPIO_Handler_t handlerPinExternalBotton ={0};
GPIO_Handler_t handlerPinData ={0};
GPIO_Handler_t handlerPinClock ={0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};
EXTI_Config_t UserExti ={0};
EXTI_Config_t 		extiConfigClock	= {0};
EXTI_Config_t 		extiConfigBotton = {0};

uint32_t counterEncoder = 0;

//Definicion de las variables bandera
uint8_t encoderEdge = RESET;
uint8_t externalBottonEdge = RESET;

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_exti7(void);
void callback_exti8(void);
void callback_exti9(void);


int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();

	while(1){
		if(encoderEdge == SET){

			//Identificamos según el flanco detectado si se ha hecho un giro en el sentido CW
			if((GPIO_ReadPin(&handlerPinClock) & (GPIO_ReadPin(&handlerPinData))) == RESET){
				/*Verificamos si la variable contador no ha pasado de 50
				Mientras esté en dichos valores, la variable seguirá aumentando*/
				if(counterEncoder < 50){
					counterEncoder++;
				}
			}

			//Identificamos según el flanco detectado si se ha hecho un giro en el sentido CCW
			else if((GPIO_ReadPin(&handlerPinClock) & (GPIO_ReadPin(&handlerPinData))) == SET){

				/*Verificamos si la variable contador no es menor que 0
				 * mientras esté en dichos valores seguirá disminuyendo*/
				if(counterEncoder > 0){
					counterEncoder--;
				}

			}
			//Bajamos la variable bandera para reiniciar el proceso;
			encoderEdge = RESET;
		}

		/*Si la variable bandera (externalBottonEdge) es igual a SET es porque se ha detectado
		 un cambio de flanco en el boton externo (se ha activado)*/

		if(externalBottonEdge == SET){
			//Bajamos la variable bandera para reiniciar el proceso;
			externalBottonEdge = RESET;
		}

		}

	return 0;
}

void init_Hardware(void){

	/* Config Led2 - PA5 */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 	= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);
//	GPIO_WritePin(&handlerLED2, SET);

	/* Config TIM2 para encender cada 250 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250; // Interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	//Configuración del pin sobre el cual recibirá la señal clock
	handlerPinClock.pGPIOx 																= GPIOA;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber										= PIN_9;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinOType										= GPIO_OTYPE_PUSHPULL;
	handlerPinClock.GPIO_PinConfig.GPIO_PinSpeed										= GPIO_OSPEED_MEDIUM;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinClock.GPIO_PinConfig.GPIO_PinAltFunMode									= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal desfasada
	handlerPinData.pGPIOx 																= GPIOA;
	handlerPinData.GPIO_PinConfig.GPIO_PinNumber										= PIN_7;
	handlerPinData.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinData.GPIO_PinConfig.GPIO_PinOType											= GPIO_OTYPE_PUSHPULL;
	handlerPinData.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinData.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinData.GPIO_PinConfig.GPIO_PinAltFunMode									= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal del botón (sw)
	handlerPinExternalBotton.pGPIOx 																= GPIOB;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinNumber											= PIN_8;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinOType											= GPIO_OTYPE_PUSHPULL;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinPuPdControl										= GPIO_PUPDR_NOTHING;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinAltFunMode										= AF0;


	//Exti para la señal del Clock
	extiConfigClock.pGPIOHandler 						= &handlerPinClock;
	extiConfigClock.edgeType							= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigBotton);

	//EXTI para la señal del Botón
	extiConfigBotton.pGPIOHandler						= &handlerPinExternalBotton;
	extiConfigBotton.edgeType							= EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&extiConfigBotton);

} // Fin init hardware

void callback_exti7(void){

}

void callback_exti8(void){

}

void callback_exti9(void){

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

