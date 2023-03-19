/*
 * GPIOxDriverMain.c
 *
 *  Created on: Mar 10, 2023
 *      Author: sjacome
 *
 * Este programa introduce el periférico más simple que tiene el micro, que es
 * el encargado de manejar los pines de cada puerto del micro.
 *
 * Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo
 * entrada y salida de datos digitales (o sea 1's y 0's), además se les puede
 * asignar funciones especiales que generalmente están ligadas a otro periférico
 * adicional.
 *
 * De igual forma, varias características pueden ser configuradas para cada el PINx
 * específico como son:
 * -La velocidad a la que puede responder
 * -Tipo de entrada (pull-up,pull-down, open drain)
 * -Tipo de salida (push-pull, open drain...)
 * -Entrada análoga
 *
 * Debemos definir entonces todos los registros que manejan el periféricos GPIOx y
 * luego crear algunas funciones para utilizar adecuadamente el equipo.
 */

#include <stdint.h>
#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

/* Función principal del programa */
int main (void){

//Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};

	//Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

	//Cargamos a la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);


	//Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		//Leemos el pin
			GPIO_ReadPin(&handlerUserLedPin);

		NOP();
	}
}
