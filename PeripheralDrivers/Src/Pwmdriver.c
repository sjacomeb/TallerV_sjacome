/*
 * Pwmdriver.c
 *
 *  Created on: May 18, 2023
 *      Author: sjacome
 */

#include "PwmDriver.h"
#include "BasicTimer.h"

/**/
void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrPwmHandler->ptrTIMx == TIM2){
		RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	}
	else if(ptrPwmHandler->ptrTIMx == TIM5){
		RCC->APB1ENR &=~ RCC_APB1ENR_PWREN;
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	}
	else{
		__NOP();
	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(ptrPwmHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(ptrPwmHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
//	ptrPwmHandler->ptrTIMx->ARR = 0;
	ptrPwmHandler->ptrTIMx->ARR &= ~(TIM_CR1_DIR); // Cero es el upmode

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);  // Cero es output

		// Configuramos el canal como PWM
		//PMW mode 1 (110)
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_OC1M_0);  //Cero en la posicion cero
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1M;  // Uno en las demas posiciones

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

		break;
	}

	case PWM_CHANNEL_2:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);

		// Configuramos el canal como PWM
		//PMW mode 1 (110)
		ptrPwmHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_OC2M_0);  //Cero en la posicion cero
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2M;  // Uno en las demas posiciones

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR1 |= TIM_CCMR1_OC2PE;

		break;
	}

	case PWM_CHANNEL_3:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);

		// Configuramos el canal como PWM
		//PMW mode 2 (111)
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3M;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC3PE;

			break;
	}
	case PWM_CHANNEL_4:{
		// Seleccionamos como salida el canal
		ptrPwmHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);

		// Configuramos el canal como PWM
		//PMW mode 2 (111)
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4M;

		// Activamos la funcionalidad de pre-load
		ptrPwmHandler->ptrTIMx->CCMR2 |= TIM_CCMR2_OC4PE;

			break;
	}
	default:{
		break;
	}

	/* 6. Activamos la salida seleccionada */
	enableOutput(ptrPwmHandler);

	}// fin del switch-case
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 |= (TIM_CR1_CEN);  //Se activa el counter enable
	ptrPwmHandler->ptrTIMx->CR1 |= (TIM_CR1_ARPE); //Se activa el auto-reload preload enable
}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler) {
	ptrPwmHandler->ptrTIMx->CR1 &= ~ (TIM_CR1_CEN);  //Se desactiva el counter enable
	ptrPwmHandler->ptrTIMx->CR1 &= ~ (TIM_CR1_ARPE); //Se desactiva el auto-reload preload enable
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler) {
	switch (ptrPwmHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}
	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 2
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}
	case PWM_CHANNEL_3: {
		// Activamos la salida del canal 3
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
		break;
	}
	case PWM_CHANNEL_4: {
		// Activamos la salida del canal 4
		ptrPwmHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}
	default: {
		break;
	}
	}
}

/*
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *ptrPwmHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	switch (ptrPwmHandler->config.prescaler){
	case(BTIMER_SPEED_10us):{
		ptrPwmHandler->ptrTIMx->PSC = 160;
		break;
	}
	case(BTIMER_SPEED_100us):{
		ptrPwmHandler->ptrTIMx->PSC = 1600;
		break;
	}
	case(BTIMER_SPEED_1ms):{
		ptrPwmHandler->ptrTIMx->PSC = 16000;
		break;
	}
	default:{
		ptrPwmHandler->ptrTIMx->PSC = ptrPwmHandler->config.prescaler;
		break;
	}
	}

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	ptrPwmHandler->ptrTIMx->ARR = ptrPwmHandler->config.periodo;
}

/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler, uint16_t newFreq){

	// Actualizamos el registro que manipula el periodo
	ptrPwmHandler->ptrTIMx->ARR = newFreq;

	// Llamamos a la función que cambia la frecuencia
	setFrequency(ptrPwmHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *ptrPwmHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;

		break;
	}

	case PWM_CHANNEL_2:{
		ptrPwmHandler->ptrTIMx->CCR2 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_3:{
		ptrPwmHandler->ptrTIMx->CCR3 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	case PWM_CHANNEL_4:{
		ptrPwmHandler->ptrTIMx->CCR4 = ptrPwmHandler->config.duttyCicle;

		break;
	}
	default:{
		break;
	}

	}// fin del switch-case

}

/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
	ptrPwmHandler->config.duttyCicle = newDutty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	setDuttyCycle(ptrPwmHandler);
}


