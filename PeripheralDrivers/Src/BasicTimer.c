/*
 * BasicTimer.c
 *
 *  Created on: Mar 23, 2023
 *      Author: sjacome
 */

#include "BasicTimer.h"

/* Variable que guarda la referencia del periférico que se esta utilizando*/
TIM_TypeDef	*ptrTimer2Used = TIM2;
TIM_TypeDef	*ptrTimer3Used = TIM3;
TIM_TypeDef	*ptrTimer4Used = TIM4;
TIM_TypeDef	*ptrTimer5Used = TIM5;

/* Función en la que cargamos la configuración del Timer
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando.
 * Además, en este caso, debemos ser cuidadosos al momento de utilizar las interrupciones.
 * Los Timer están conectados directamente al elemento NVIC del Cortex-Mx
 * Debemos configurar y/o utilizar:
 *  - TIMx_CR1  (control Register 1)
 *  - TIMx_SMCR ( slave mode control register) -> mantener en 0 para modo Timer Básico
 *  - TIMx_DIER (DMA and Interrupt enable register)
 *  - TIMx_SR (Status register)
 *  - TIMx_CNT (Counter)
 *  - TIMx_PSC (Pre-scaler)
 *  - TIMx_ARR  (Auto-reload register)
 *
 *  Como vamos a trabajar con interrupciones, antes de configurar una nueva, debemos desactivar
 *  el sistema global de interrupciones, activar la IRQ específica y luego volver a encender
 *  el sistema.
 */
void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler){
	// Guardamos una referencia al periferico que estamos utilizando...
	ptrTimer2Used = ptrBTimerHandler->ptrTIMx;
	ptrTimer3Used = ptrBTimerHandler->ptrTIMx;
	ptrTimer4Used = ptrBTimerHandler->ptrTIMx;
	ptrTimer5Used = ptrBTimerHandler->ptrTIMx;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();
	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrBTimerHandler->ptrTIMx == TIM2){
		// Registro del RCC que nos activa la señal de reloj para el TIM2
		RCC ->APB1ENR &=~ RCC_APB1ENR_TIM2EN;
		RCC ->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM3){
		// Registro del RCC que nos activa la señal de reloj para el TIM3
		RCC ->APB1ENR &=~ RCC_APB1ENR_TIM3EN;
		RCC ->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM4){
		// Registro del RCC que nos activa la señal de reloj para el TIM3
		RCC ->APB1ENR &=~ RCC_APB1ENR_TIM4EN;
		RCC ->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM5){
		// Registro del RCC que nos activa la señal de reloj para el TIM3
		RCC ->APB1ENR &=~ RCC_APB1ENR_TIM5EN;
		RCC ->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2. Configuramos el Pre-scaler
	 * Recordar que el prescaler nos indica la velocidad a la que se incrementa el counter, de forma que
	 * periodo_incremento * veces_incremento_counter = periodo_update
	 * Modificar el valor del registro PSC en el TIM utilizado
	 */

	ptrBTimerHandler->ptrTIMx->PSC = ptrBTimerHandler->TIMx_Config.TIMx_speed;
	/* 3. Configuramos la dirección del counter (up/down)*/
	if(ptrBTimerHandler->TIMx_Config.TIMx_mode == BTIMER_MODE_UP){

		/* 3a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
		// Configurar el registro que nos controla el modo up or down
		ptrBTimerHandler->ptrTIMx ->CR1 &= ~(TIM_CR1_DIR);

		/* 3b. Configuramos el Auto-reload. Este es el "limite" hasta donde el CNT va a contar */
		ptrBTimerHandler->ptrTIMx->ARR = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;

		/* 3c. Reiniciamos el registro counter*/
		ptrBTimerHandler ->ptrTIMx ->CNT = 0;

	}else{
		/* 3a. Estamos en DOWN_Mode, el limite se carga en ARR (0) y se comienza en un valor alto
		 * Trabaja contando en direccion descendente*/
		ptrBTimerHandler->ptrTIMx ->CR1 |= TIM_CR1_DIR;


		/* 3b. Configuramos el Auto-reload. Este es el "limite" hasta donde el CNT va a contar
		 * En modo descendente, con numero positivos, cual es el minimi valor al que ARR puede llegar*/
		ptrBTimerHandler->ptrTIMx->ARR = 0;

		/* 3c. Reiniciamos el registro counter
		 * Este es el valor con el que el counter comienza */
		ptrBTimerHandler->ptrTIMx->CNT = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;
	}

	/* 4. Activamos el Timer (el CNT debe comenzar a contar*/
	ptrBTimerHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

	/* 5. Activamos la interrupción debida al Timerx Utilizado
	 * Modificar el registro encargado de activar la interrupcion generada por el TIMx*/

	//Primero habilitar el UIE
	ptrBTimerHandler ->ptrTIMx ->DIER |= SET;

	/* 6. Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrBTimerHandler->ptrTIMx == TIM2){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM3){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if (ptrBTimerHandler->ptrTIMx == TIM4){
		// Activando en NVIC para la interrupción del TIM4
		NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM5){
		// Activando en NVIC para la interrupción del TIM5
		NVIC_EnableIRQ(TIM5_IRQn);
	}
	else{
		__NOP();
	}

	/* 7. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}



__attribute__((weak)) void BasicTimer2_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void BasicTimer3_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void BasicTimer4_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}
__attribute__((weak)) void BasicTimer5_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimerX_Callback could be implemented in the main file
	   */
	__NOP();
}

/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM2_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM2->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer2_Callback();

}
void TIM3_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM3->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer3_Callback();

}
void TIM4_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM4->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer4_Callback();

}
void TIM5_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM5->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer5_Callback();

}
