/*
 * Exti_main.c
 *
 *  Created on: Mar 25, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"


/* Definicion de los elementos */

GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};


uint32_t counterExti13 = 0;
uint8_t j=0;

/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt13(void);


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
//	GPIO_WritePin(&handlerLED2, SET);

	/* Config TIM2 para encender cada 300 ms*/
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 300; // Interrupcion cada 300 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Config EXTI */
	/* Activar user button como entrada digital*/
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);


	/* Activar señal de reloj SYSCFG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* Configuar MUX 13 para usar el pin */
	SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; // Seleccionando el puerto C para el MUX13

	/* Config flanco */
	EXTI->FTSR = 0; // Desactivar flanco bajada
	EXTI->RTSR = 0;	//Valor conocido
	EXTI->RTSR |= EXTI_RTSR_TR13; // Activar flanco subida para pin13

	/* Config mascara EXTI */
	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13;

	/* Desactivar interrupciones */
	__disable_irq();

	/* Matricular interrupcion en NVIC */
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* Crear ISR */
	/* Crear Callback */
	/* Activar interrupciones */
	__enable_irq();


} // Fin init hardware

void callback_extInt13(void){
	j++;
}

//// Definicion ISR para EXTI13
//void EXTI15_10_IRQHandler(void){
//	if(EXTI->PR & EXTI_PR_PR13){
//		EXTI->PR |= EXTI_PR_PR13; // Limpiar bandera de EXTI 13
//		callback_exti13();
//	}
//}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

