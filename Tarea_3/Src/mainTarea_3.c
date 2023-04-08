/*
 * mainTarea_3.c
 *
 *  Created on: Apr 8, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/* Definición de los elementos */
GPIO_Handler_t 	handlerLED2 				= {0};
GPIO_Handler_t 	handlerButton 				= {0};
GPIO_Handler_t 	handlerCLK 					= {0};
GPIO_Handler_t 	handlerDT 					= {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0};

EXTI_Config_t ExtiClock = {0};
uint8_t encoderEdge = 0;
uint8_t l=0;

void init_Hardware(void);
void callback_extInt13(void);
void BasicTimer2_Callback(void);

int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();


	while(1){


	}

	return 0;
}

void init_Hardware(void){

	/* Configuración del led de estado */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 	= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);

	/* Configuración del TIM2 para controlar el blinky */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250;       // Interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración del DT LISTO */
	handlerDT.pGPIOx = GPIOC;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	      = PIN_13;
	handlerDT.GPIO_PinConfig.GPIO_PinMode 	          = GPIO_MODE_IN;
	handlerDT.GPIO_PinConfig.GPIO_PinOType	          = GPIO_OTYPE_PUSHPULL;
	handlerDT.GPIO_PinConfig.GPIO_PinSpeed	          = GPIO_OSPEED_FAST;
	handlerDT.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDT);

	ExtiClock.pGPIOHandler = &handlerDT;
	ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&ExtiClock);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

void callback_extInt13(void){
	encoderEdge = 1;  //Subiendo la bandera
	l++;
}
