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


//Definición de los handlers necesarios para el código
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

//Definicion de la variable contadora que almacenará el valor del encoder.
uint8_t counterEncoder = 0;

//Denificion del arreglo para identificar el número a mostrar en el Display
char bufferCounterEncoder[3];

//Definicion de las variables bandera
uint8_t encoderEdge = RESET;
uint8_t externalBottonEdge = RESET;

//Definicion de las cabeceras de las funciones del main
void initSystem(void);
void DisplayNumber (uint8_t number);
void ShowNumber(void);


int main(void)
{
	//Inicializamos todas los elementos del sistema
	initSystem();

	while(1){
		/*Si la variable bandera está en SET, es porque hubo un cambio de flanco en la señal
		entrando al bloque if*/
		if(encoderEdge == SET){

			//Identificamos según el flanco detectado si se ha hecho un giro en el sentido CW
			if((GPIO_ReadPin(&handlerPinClock) & (GPIO_ReadPin(&handlerPinClockOutOfPhase))) == RESET){
				/*Verificamos si la variable contador no ha pasado de 50
				Mientras esté en dichos valores, la variable seguirá aumentando*/
				if(counterEncoder < 50){
					counterEncoder++;
				}
				/*Convertimos la variable counterEncoder que alamacena el valor del paso en un arreglo
				 * de Char para posteriormente ser enviadas por medio del protocolo USART*/
				sprintf(bufferCounterEncoder,"%u",counterEncoder);
				writeMsg(&handlerUsart2, messageEncoderCW);
				writeMsg(&handlerUsart2, bufferCounterEncoder);
			}

			//Identificamos según el flanco detectado si se ha hecho un giro en el sentido CCW
			else if((GPIO_ReadPin(&handlerPinClock) & (GPIO_ReadPin(&handlerPinClockOutOfPhase))) == SET){

				/*Verificamos si la variable contador no es menor que 0
				 * mientras esté en dichos valores seguirá disminuyendo*/
				if(counterEncoder > 0){
					counterEncoder--;
				}
				sprintf(bufferCounterEncoder,"%u",counterEncoder);
				writeMsg(&handlerUsart2, messageEncoderCCW);
				writeMsg(&handlerUsart2, bufferCounterEncoder);
			}
			//Bajamos la variable bandera para reiniciar el proceso;
			encoderEdge = RESET;
		}

		/*Si la variable bandera (externalBottonEdge) es igual a SET es porque se ha detectado
		 un cambio de flanco en el boton externo (se ha activado)*/

		if(externalBottonEdge == SET){
			//Enviamos el mensaje cada vez que se presione el botón
			writeMsg(&handlerUsart2, "\nMetallica: Enter Sandman. Reproduzcalo mientras"
					" califica esta tarea :D");
			//Bajamos la variable bandera para reiniciar el proceso;
			externalBottonEdge = RESET;
		}

		}

	return 0;
}

/*Esta función se encarga de mostrar las unidades y decenas en los display 7 segmentos respectivamente
 * según el valor de la variable counterEncoder*/
void ShowNumber(void){
	if(GPIO_ReadPin(&handlerPinDisplay_TLeft) == RESET){
		DisplayNumber(counterEncoder/10);
	}
	else {
		DisplayNumber(counterEncoder%10);
	}
}

/*Esta funcion es encargada de configurar los pines GPIO correspondiente a cada segmento en el display
 * para mostrar el número solicitado*/
void DisplayNumber (uint8_t number){
	switch(number){
	case 0:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 1:
		GPIO_WritePin(&handlerPinDisplay_a, SET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 2:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, SET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 3:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 4:
		GPIO_WritePin(&handlerPinDisplay_a, SET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 5:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, SET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 6:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, SET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 7:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 8:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 9:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
		// Falta el acso default...
	}
}

//Esta funciona carga toda la configuración a nivel de registros de todos los perifericos utilizados
void initSystem(void){

	//Configuración del pin correspondiente al led de estado
	handlerBlinkyPin.pGPIOx 										= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber			 		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 					= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOType 					= GPIO_0TYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 			= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed 					= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;

	//Configuración del pin sobre el cual funciona el segmento "a" del display;      LISTO
	handlerPinDisplay_a.pGPIOx 												= GPIOC;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_1;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "b" del display;      LISTO
	handlerPinDisplay_b.pGPIOx 												= GPIOC;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_10;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "c" del display;     LISTO
	handlerPinDisplay_c.pGPIOx 												= GPIOA;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_2;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "d" del display;      LISTO
	handlerPinDisplay_d.pGPIOx 												= GPIOA;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_13;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "e" del display;     LISTO
	handlerPinDisplay_e.pGPIOx 												= GPIOA;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_15;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "f" del display;      LISTO
	handlerPinDisplay_f.pGPIOx 												= GPIOA;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_4;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el segmento "g" del display;     LISTO
	handlerPinDisplay_g.pGPIOx 												= GPIOB;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_3;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinOType 						= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;

	//Configuración del pin sobre el cual funciona el transistor left que controla el display izquierdo;   LISTO
	handlerPinDisplay_TLeft.pGPIOx 													= GPIOC;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinNumber			 				= PIN_13;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinOType 							= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinPuPdControl 						= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinSpeed 							= GPIO_OSPEED_FAST;
	handlerPinDisplay_TLeft.GPIO_PinConfig.GPIO_PinAltFunMode						= AF0;

	//Configuración del pin sobre el cual funciona el transistor right que controla el display derecho;   LISTO
	handlerPinDisplay_TRight.pGPIOx 												= GPIOC;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinNumber			 				= PIN_15;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinOType 							= GPIO_0TYPE_PUSHPULL;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinSpeed 							= GPIO_OSPEED_FAST;
	handlerPinDisplay_TRight.GPIO_PinConfig.GPIO_PinAltFunMode						= AF0;

	//Configuración del pin sobre el cual recibirá la señal clock
	handlerPinClock.pGPIOx 																= GPIOA;
	handlerPinClock.GPIO_PinConfig.GPIO_PinNumber										= PIN_7;
	handlerPinClock.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinClock.GPIO_PinConfig.GPIO_PinOPType										= GPIO_0TYPE_PUSHPULL;
	handlerPinClock.GPIO_PinConfig.GPIO_PinSpeed										= GPIO_OSPEED_MEDIUM;
	handlerPinClock.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinClock.GPIO_PinConfig.GPIO_PinAltFunMode									= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal desfasada
	handlerPinClockOutOfPhase.pGPIOx 																= GPIOA;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinNumber											= PIN_6;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinOPType											= GPIO_0TYPE_PUSHPULL;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinPuPdControl									= GPIO_PUPDR_NOTHING;
	handlerPinClockOutOfPhase.GPIO_PinConfig.GPIO_PinAltFunMode										= AF0;

	//Configuracion del pin sobre el cual se recibirá la señal del botón
	handlerPinExternalBotton.pGPIOx 																= GPIOB;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinNumber											= PIN_9;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_IN;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinOPType											= GPIO_0TYPE_PUSHPULL;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_MEDIUM;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinPuPdControl										= GPIO_PUPDR_NOTHING;
	handlerPinExternalBotton.GPIO_PinConfig.GPIO_PinAltFunMode										= AF0;

	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	//Configuración del pin TX por el cual se transmitirá la información en pantalla
	handlerPinTx.pGPIOx 																= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber											= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode											= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType											= GPIO_0TYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed											= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl										= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode										= AF7;


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


	//Configuracion del modulo serial USART para la comunicación

	handlerUsart2.ptrUSARTx															= USART2;
	handlerUsart2.USART_Config.USART_mode											= USART_MODE_TX;
	handlerUsart2.USART_Config.USART_baudrate 										= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize 										= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity 										= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits										= USART_STOPBIT_1;


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

		//Se carga la configuración del modulo de comunciación USART
		USART_Config(&handlerUsart2);

		//Se carga la configuración correspondientes al EXTI
		extInt_Config(&extiConfigClock);
		extInt_Config(&extiConfigBotton);

		//Se establecen los estados iniciales de los perifericos que sean necesarios.
		GPIO_WritePin(&handlerPinDisplay_TRight, SET);
		GPIO_WritePin(&handlerPinDisplay_TLeft, RESET);
		ShowNumber();

		//Se habilitan los timers para empezar el conteo
		startTimer(&handlerTimerBlinky);
		startTimer(&handlerTimerDisplay);

	}

//Respectivos callbacks para las interrupciones deseadas
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

void BasicTimer5_Callback(void){
	GPIOxTooglePin(&handlerPinDisplay_TRight);
	GPIOxTooglePin(&handlerPinDisplay_TLeft);
	ShowNumber();
	// Esta forma de actualizar los displays hace que se genere una sombra.
}

void callback_extInt7(void){
	encoderEdge = SET;
}

void callback_extInt9(void){
	externalBottonEdge = SET;
}





