/*
 * GPIOxDriver.h
 *
 *  Created on: Mar 10, 2023
 *      Author: sjacome
 *
 * Este programa se encarga de las definiciones relacionadas únicamente con
 * el debido manejo y control del periférico GPIOx (funciones del controlador)
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_
#include "stm32f411xx_hal.h"

typedef struct
{
	uint8_t GPIO_PinNumber;             //PIN con que el que deseamos trabajar
	uint8_t GPIO_PinMode;               //Modo de configuración: input, output, analog, alternative
	uint8_t GPIO_PinSpeed;              //Velocidad de respuesta del PIN (solo digital)
	uint8_t GPIO_PinPuPdControl;        //Seleccionamos si deseamos una salida pull-up, pull-down o "libre"
	uint8_t GPIO_PinOType;              //Selecciona salida PUPD o OpenDrain
	uint8_t GPIO_PinAltFunMode;         //Selecciona cual es la función alternativa que se está configurqando
}GPIO_PinConfig_t;

/* Esta estructura contiene dos elementos:
 * 1) La dirección del puerto (la referencia al puerto)
 * 2) La configuración específica del PIN que se está utilizando
 */

typedef struct
{
	GPIOx_RegDef_t          *pGPIOx;               //Dirección del puerto al que el PIN corresponde
	GPIO_PinConfig_t       GPIO_PinConfig;        //Configuración del PIN

}GPIO_Handler_t;

/*Definición de las cabeceras de las funciones del GPIOxDriver*/
void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin (GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler);

#endif /* GPIOXDRIVER_H_ */
