/*
 * main.c
 *
 *  Created on: Apr 4, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

/* Definicion de los elementos */
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButtonSW = {0};
GPIO_Handler_t handlerCLK = {0};
GPIO_Handler_t handlerDT = {0};

BasicTimer_Handler_t handlerBlinkyTimer = {0}; //TIM2

//Definición del exti
EXTI_Config_t ExtiClock = {0};

uint8_t encoderEdge = 0;   //Bandera
uint8_t counterEncoder =0;

uint8_t s = 0;
uint8_t estado = 0;

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt7(void);


int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();

	while(1){


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

	/* Configuración del Pin PC7 (CLK) */
	handlerCLK.pGPIOx = GPIOB;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber 	= PIN_8;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerCLK);

	/* Configuración del Pin PB8 (DT) */
	handlerDT.pGPIOx = GPIOC;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	= PIN_13;
	handlerDT.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_IN;
	handlerDT.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerDT.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerDT.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDT);


	/* Config TIM2 para encender cada 300 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 300; // Interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Config botón SW*/

//	handlerUserButtonSW.pGPIOx = GPIOC;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
//	handlerUserButtonSW.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerUserButtonSW);

	//Cargamos la configuración del EXTI
	ExtiClock.pGPIOHandler = &handlerDT;
	ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&ExtiClock);


}
void callback_extInt7(void){ //cLOCK
	encoderEdge = 1; //Flanco de subida
	s++;
}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

