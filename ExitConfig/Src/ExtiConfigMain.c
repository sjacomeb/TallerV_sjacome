/*
 * ExtiConfigMain.c
 *
 *  Created on: Mar 30, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <GPIOxDriver.h>
#include <BasicTimer.h>
#include <ExtiDriver.h>

GPIO_Handler_t handlerLed2 = {0};
GPIO_Handler_t UserButton = {0};
GPIO_Handler_t handlerPIN10 = {0};
GPIO_Handler_t handlerPIN12 = {0};
BasicTimer_Handler_t timer = {0};
EXTI_Config_t UserExti = {0};

//Definimos una funcion para la configuracion de GPIO
void init_Hardware(void);


//definimos una variable para contar
uint32_t counterExti13 = 0;

int main(void)
{
	//Realizamos la configuracion inicial
	init_Hardware();

	GPIO_WritePin(&handlerLed2, SET);
	GPIO_WritePin(&handlerPIN10, RESET);

    /* Loop forever */
	while(1){


		__NOP();

	}
	return(0);
}

//Definimos la funcion que se desea ejecutar cuando se genera la interrupcion por el TIM2

void BasicTimer_Callback(void)
{
	GPIOxTooglePin(&handlerPIN12);
}

void init_Hardware(void)
{
	//Configuracion el LED 2 -> PA5
	handlerLed2.pGPIOx                                       = GPIOA;
	handlerLed2.GPIO_PinConfig.GPIO_PinNumber                = PIN_5;
	handlerLed2.GPIO_PinConfig.GPIO_PinMode                  = GPIO_MODE_OUT;
	handlerLed2.GPIO_PinConfig.GPIO_PinOType                = GPIO_OTYPE_PUSHPULL;
	handlerLed2.GPIO_PinConfig.GPIO_PinSpeed                 = GPIO_OSPEED_MEDIUM;
	handlerLed2.GPIO_PinConfig.GPIO_PinPuPdControl           = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLed2);

	//Configuracion PIN 13 -> PC13
	UserButton.pGPIOx                                       = GPIOC;
	UserButton.GPIO_PinConfig.GPIO_PinNumber                = PIN_13;
	UserButton.GPIO_PinConfig.GPIO_PinMode                  = GPIO_MODE_IN;
	UserButton.GPIO_PinConfig.GPIO_PinOType                = GPIO_OTYPE_PUSHPULL;
	UserButton.GPIO_PinConfig.GPIO_PinSpeed                 = GPIO_OSPEED_MEDIUM;
	UserButton.GPIO_PinConfig.GPIO_PinPuPdControl           = GPIO_PUPDR_NOTHING;
	GPIO_Config(&UserButton);


	//Configuracion el LED 2 -> PC10
	handlerPIN10.pGPIOx                                       = GPIOC;
	handlerPIN10.GPIO_PinConfig.GPIO_PinNumber                = PIN_10;
	handlerPIN10.GPIO_PinConfig.GPIO_PinMode                  = GPIO_MODE_OUT;
	handlerPIN10.GPIO_PinConfig.GPIO_PinOType                = GPIO_OTYPE_PUSHPULL;
	handlerPIN10.GPIO_PinConfig.GPIO_PinSpeed                 = GPIO_OSPEED_MEDIUM;
	handlerPIN10.GPIO_PinConfig.GPIO_PinPuPdControl           = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerPIN10);

	//Configuracion el LED 2 -> PC12
	handlerPIN12.pGPIOx                                       = GPIOC;
	handlerPIN12.GPIO_PinConfig.GPIO_PinNumber                = PIN_12;
	handlerPIN12.GPIO_PinConfig.GPIO_PinMode                  = GPIO_MODE_OUT;
	handlerPIN12.GPIO_PinConfig.GPIO_PinOType                = GPIO_OTYPE_PUSHPULL;
	handlerPIN12.GPIO_PinConfig.GPIO_PinSpeed                 = GPIO_OSPEED_MEDIUM;
	handlerPIN12.GPIO_PinConfig.GPIO_PinPuPdControl           = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerPIN12);

	//Configuracion TIMx
	timer.ptrTIMx                                           = TIM2;
	timer.TIMx_Config.TIMx_speed                            = BTIMER_SPEED_1ms;
	timer.TIMx_Config.TIMx_mode                             = BTIMER_MODE_UP;
	timer.TIMx_Config.TIMx_period                           = 250;
	BasicTimer_Config(&timer);


	//Configuracion Exti
	UserExti.pGPIOHandler =                         &UserButton;
	UserExti.edgeType =                             EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&UserExti);

	//Fin de la configuracion necesaria para el Blinky
}

//Funcion para ISR
void callback_extInt13(void){
	GPIOxTooglePin(&handlerLed2);
	GPIOxTooglePin(&handlerPIN10);
}


	//Configuracion del EXTI

	//2 a. Activamos la señal de reloj para el SYSCFG
/*	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	//2 b. Configurar el multiplexor 13
	SYSCFG->EXTICR[3] &= ~(0xF<<SYSCFG_EXTICR4_EXTI13_Pos); // 0xF = 1111, limpiamos
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; //Asignamos el puerto C

	//Configuracion del EXTI
	EXTI->FTSR = 0;  //Desactivamos TODOS los posibles detecciones de flancos de bajada "Falling"
	EXTI->RTSR = 0;  //Desactivar TODOS los posibles detecciones de flancos de subida "Rising"
	EXTI->RTSR |= EXTI_RTSR_TR13; //Activamos el flanco de subida
	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13; //Activamos la interrupcion EXTI 13

	//4 a. Deshabilitamos las interrupciones globales
	__disable_irq();
	//4 b. Activamos el NVIC para la interrupción del EXTI15-10
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	//4 c. Crear ISR
	//4 d. Activamos las interrupciones globales
	__enable_irq();  */


/*void callback_exti13(void)
{
	counterExti13++;
}
void EXTI15_10_IRQHandler(void)
{
	//verificamos la interrupcion
	if((EXTI->PR & EXTI_PR_PR13)!=0)
	{
		EXTI->PR |= EXTI_PR_PR13; //Bajamos la bandera
		callback_exti13();
	}
} */


