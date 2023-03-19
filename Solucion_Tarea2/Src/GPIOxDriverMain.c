/*
 * GPIOxDriverMain.c
 *
 *  Created on: Mar 10, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"

/* +++++++++   Primer punto    +++++++++ */

/* a)La función GPIO_ReadPin() lee el valor contenido en el registro IDR del GPIOx y le realiza un shift a la derecha tantas veces
 * como la ubicación del PIN específico, por ejemplo, si se tiene el PIN4 la función le hace al valor del registro un shift
 * a la derecha de 4 posiciones. Es por esto que el resultado que retorna la función no es el valor real del PIN ya que se tienen bits
 * a la izquierda de este valor. */


/* b) Para solucionar este problema hay que aplicarle una máscara donde se conserve el valor real del PIN y se conviertan en "0"
 * los bits a la izquierda de este valor. Para esto se utiliza una operación AND con un uno en la posición del bit menos significativo
 * y ceros en los bits restantes, ya que el uno en esta operación conserva el valor y el cero vuelve el valor cero.*/

/* c) La función fue corregida en el archivo GPIOxDriver.c */

/* +++++++++ Segundo punto +++++++++++ */


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


	/* +++++++++ Tercer punto +++++++++ */


	//Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		GPIO_ReadPin(&handlerUserLedPin);   //Lectura del valor del PIN5
		GPIOxTooglePin(&handlerUserLedPin);  //Cambio de estado del LED2

		__NOP();
	}
}


