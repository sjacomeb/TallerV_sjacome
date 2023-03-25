/*
 * ExitConfig_main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"

/*Definición de los elementos del sistema*/
GPIO_Handler_t handlerLED2 = {0};
GPIO_Handler_t handlerUserButton = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
uint32_t counterExti13 = 0;


/*Prototipos de las funciones */

void init_Hardware(void);
void callback_exti13(void);

/**/
int main (void){

	/*Inicialización de todos los elementos del sistema*/
	init_Hardware();

	while(1){

	}
	return 0;
}

/**/
void init_Hardware(void){

	/*Configuración del LED2 - PA5*/
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_FAST;

	GPIO_Config(&handlerLED2);
//	GPIO_WritePin(&handlerLED2, SET);

	/* Configuración del TIM2 para que haga un blinky cada 300ms */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 300;    //Lanza una interrupción cada 300ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;

	/* Cargamos la configuración del TIM2 en los registros */
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración del exti- user button -PC13 */

	/* 1.Configurar el pin como una entrada digital */
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode  = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	/* Cargando la configuración en los registros*/
	GPIO_Config(&handlerUserButton);

	/* 2. Activando la señal de reloj del SYSCFG */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 3. Configurando el Mux 13 para que utilice el puerto C */
	SYSCFG->EXTICR[3] &= ~(0XF << SYSCFG_EXTICR4_EXTI13_Pos);   //0xF representa 0b1111
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;        //Seleccionando el puerto C para el Mux 13

	/* 4.a Configurar el tipo de flanco - Configuramos el rising por elección */
	EXTI->FTSR = 0;  //Desactivamos todos los posibles flancos de bajada;
	EXTI->RTSR = 0;   //Llevando el registro a un valor conocido
	EXTI->RTSR |= EXTI_RTSR_TR13;

	/* 4.b Activar interrupcion */
	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13;  //Activamos la interrupcion 13

	/* 5.a Desactivar todas las interrupciones */
	__disable_irq();

	/* 5.b Matricular la interrupcion */
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* 5.c Crear ISR */
	/* 5.d Crear el callback*/
	/* 5.e  Activar las interrupciones */
	__enable_irq();

}//Termina el init_Hardware

void callback_exti13(void){
	counterExti13++;
}

/**/
void EXTI15_10_IRQHandler(void){

	if((EXTI->PR & EXTI_PR_PR13)!=0){
		EXTI->PR |= EXTI_PR_PR13;     //Limpiar la bandera
		callback_exti13();
	}
}

/**/
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);

}



