/*
 * mainTarea3.c
 *
 *  Created on: Apr 2, 2023
 *      Author: sjacome
 */

//Librerías
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


//Definición de los handlers
GPIO_Handler_t 				handlerBlinkyPin 				= {0};
GPIO_Handler_t				handlerPinDisplay_TLeft			= {0};
GPIO_Handler_t				handlerPinDisplay_TRight		= {0};
GPIO_Handler_t				handlerPinDisplay_a				= {0};
GPIO_Handler_t				handlerPinDisplay_b				= {0};
GPIO_Handler_t				handlerPinDisplay_c				= {0};
GPIO_Handler_t				handlerPinDisplay_d				= {0};
GPIO_Handler_t				handlerPinDisplay_e				= {0};
GPIO_Handler_t				handlerPinDisplay_f				= {0};
GPIO_Handler_t				handlerPinDisplay_g				= {0};
GPIO_Handler_t				handlerPinClock					= {0};
GPIO_Handler_t				handlerPinClockOutOfPhase		= {0};
GPIO_Handler_t				handlerPinExternalBotton		= {0};
GPIO_Handler_t				handlerPinTx					= {0};

//Timers encargados del Blinky y de los display (7 segmentos)
BasicTimer_Handler_t 	handlerTimerBlinky 	= {0};
BasicTimer_Handler_t	handlerTimerDisplay	= {0};

//Elementos necesarios para configurar el EXTI
EXTI_Config_t 		extiConfigClock	= {0};
EXTI_Config_t 		extiConfigBotton = {0};

//Definicion de un contador para el encoder
uint8_t counterEncoder = 0;

//Denificion del arreglo para identificar el número a mostrar en el Display      REVISAR
char bufferCounterEncoder[3];

//Definicion de las variables bandera
uint8_t encoderEdge = RESET;
uint8_t externalBottonEdge = RESET;

//Definicion de las cabeceras de las funciones del main
void init_Hardware(void);


int main(void){

	init_Hardware();

		while(1){

		}

		return 0;
}

//Esta funciona carga toda la configuración a nivel de registros de todos los perifericos utilizados
void init_Hardware(void){

	//Configuración del pin correspondiente al led de estado
	handlerBlinkyPin.pGPIOx 										= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber			 		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOType 					= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;

	//Configuración del pin sobre el cual funciona el segmento "a" del display;      LISTO
	handlerPinDisplay_a.pGPIOx 												= GPIOC;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_1;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "b" del display;      LISTO
	handlerPinDisplay_b.pGPIOx 												= GPIOC;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_10;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "c" del display;     LISTO
	handlerPinDisplay_c.pGPIOx 												= GPIOA;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_2;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "d" del display;      LISTO
	handlerPinDisplay_d.pGPIOx 												= GPIOA;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_13;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "e" del display;     LISTO
	handlerPinDisplay_e.pGPIOx 												= GPIOA;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_15;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "f" del display;      LISTO
	handlerPinDisplay_f.pGPIOx 												= GPIOA;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_4;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "g" del display;     LISTO
	handlerPinDisplay_g.pGPIOx 												= GPIOB;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_3;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el transistor left que controla el display izquierdo;   LISTO
	handlerPinDisplay_TLeft.pGPIOx 													= GPIOC;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinNumber			 				= PIN_13;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinOType 							= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinPuPdControl 						= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinSpeed 							= GPIO_OSPEED_FAST;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinAltFunMode						= AF0;

	//Configuración del pin sobre el cual funciona el transistor right que controla el display derecho;   LISTO
	handlerPinDisplay_TRight.pGPIOx 												= GPIOC;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinNumber			 				= PIN_15;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinOType 							= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinSpeed 							= GPIO_OSPEED_FAST;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinAltFunMode						= AF0;

	//Configuración del pin sobre el cual recibirá la señal clock             LISTO
	handlerPinClock.pGPIOx 																= GPIOA;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber										= PIN_8;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinOType										= GPIO_OTYPE_PUSHPULL;
	handlerPinClock.GPIO_PinConfig.GPIO_PinSpeed										= GPIO_OSPEED_MEDIUM;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinClock.GPIO_PinConfig.GPIO_PinAltFunMode									= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal desfasada
	handlerPinClockOutOfPhase.pGPIOx 																= GPIOC;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinNumber											= PIN_9;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinOType											= GPIO_OTYPE_PUSHPULL;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinAltFunMode										= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal del botón (sw)        LISTO
	handlerPinExternalBotton.pGPIOx 																= GPIOA;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinNumber											= PIN_9;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinOType											= GPIO_OTYPE_PUSHPULL;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinPuPdControl										= GPIO_PUPDR_NOTHING;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinAltFunMode										= AF0;

	//Configuración del handlerTimerBlinky correspondiente al Timer2 que controla el Blinky
	handlerTimerBlinky.ptrTIMx 														= TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode 										= BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed 										= BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period 										= 2500; //Periodo de 250ms
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable								= BTIMER_INTERRUP_ENABLE;

	//Configuración del handlerTimerDisplay correspondiente al Timer5 que controla el Display
	handlerTimerDisplay.ptrTIMx 													= TIM5;
	handlerTimerDisplay.TIMx_Config.TIMx_mode 										= BTIMER_MODE_UP;
	handlerTimerDisplay.TIMx_Config.TIMx_speed 										= BTIMER_SPEED_1ms;
	handlerTimerDisplay.TIMx_Config.TIMx_period 									= 10; //Periodo de 10ms
	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable							= BTIMER_INTERRUP_ENABLE;


	//Configuración de los elementos necesarios para el EXTI
	//Exti para la señal del Clock
	extiConfigClock.pGPIOHandler 						= &handlerPinClock;
	extiConfigClock.edgeType							= EXTERNAL_INTERRUPT_RISING_EDGE;
	//EXTI para la señal del Botón
	extiConfigBotton.pGPIOHandler						= &handlerPinExternalBotton;
	extiConfigBotton.edgeType							= EXTERNAL_INTERRUPT_FALLING_EDGE;

	//Se carga la configuración de los handlers correspondientes al GPIO
	GPIO_Config(&handlerBlinkyPin);
	GPIO_Config(&handlerPinDisplay_a);
	GPIO_Config(&handlerPinDisplay_b);
	GPIO_Config(&handlerPinDisplay_c);
	GPIO_Config(&handlerPinDisplay_d);
	GPIO_Config(&handlerPinDisplay_e);
	GPIO_Config(&handlerPinDisplay_f);
	GPIO_Config(&handlerPinDisplay_g);
	GPIO_Config(&handlerPinDisplay_TLeft);
	GPIO_Config(&handlerPinDisplay_TRight);
	GPIO_Config(&handlerPinClock);
	GPIO_Config(&handlerPinClockOutOfPhase);
	GPIO_Config(&handlerPinExternalBotton);
	GPIO_Config(&handlerPinTx);

	//Se carga la configuración de los handlers del timer
	BasicTimer_Config(&handlerTimerBlinky);
	BasicTimer_Config(&handlerTimerDisplay);

	//Se carga la configuración correspondientes al EXTI
	extInt_Config(&extiConfigClock);
	extInt_Config(&extiConfigBotton);





}


