/*
 * GPIOxDriver.c
 *
 *  Created on: Mar 10, 2023
 *      Author: sjacome
 *
 * Este archivo es la parte del programa donde se escribe el control para que sea lo más genérico
 * posible, es decir, el programa se ejecute y configure correctamente independiente del puerto
 * GPIO y el PIN seleccionados.
 */

#include "GPIOxDriver.h"

/*Se está creando una función que permita ir paso a paso sobre las configuraciones de los perifericos
en el orden necesario para culminar en un cambio de comportamiento en el periférico.*/

void GPIO_Config(GPIO_Handler_t *pGPIOHandler){
    //Variable para paso a paso
    uint32_t auxConfig = 0;
    uint32_t auxPosition = 0;

    //1. Activar el periferico
    //Verificamos para cada uno de los GPIOx
    if(pGPIOHandler-> pGPIOx == GPIOA){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOB){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOC){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOD){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOE){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }
    else if(pGPIOHandler->pGPIOx == GPIOH){
        //Escribe 1 (SET) en la posición para GPIOA
        RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
    }

    /*INICIO DE CONFIGURACION
    2. CONFIGURAR GPIOx_MODER*/

    //Se crea variable leyendo y moviendo el valor pin number veces
    auxConfig = (pGPIOHandler ->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Limpiar bits especificos del registro (0b00) usando una mascara
    pGPIOHandler-> pGPIOx-> MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Cargar la config al MODER
    pGPIOHandler-> pGPIOx->MODER |= auxConfig;

    //3. CONFIGURAR GPIOx_OTYPER
    //Leer y mover
    auxConfig = (pGPIOHandler-> GPIO_PinConfig.GPIO_PinOType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Limpiar
    pGPIOHandler-> pGPIOx-> OTYPER &= ~(/*SET*/ 0b1 << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

    //Cargar la config al OTYPER
    pGPIOHandler-> pGPIOx-> OTYPER |= auxConfig;

    //4. CONFIGURAR OSPEED
    auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinSpeed << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->OSPEEDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->OSPEEDR |= auxConfig;

    //5. CONFIGURAR PUPD
    auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinPuPdControl << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->PUPDR &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);
    pGPIOHandler->pGPIOx->PUPDR |= auxConfig;

    //6. CONFIGURAR FUNCIONES ALTERNATIVAS
    if(pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == 2 /*GPIO_MODE_ALTFN*/){

        if(pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){
            //Registro AFRL que controla PIN_0 a PIN_7
            auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;
            pGPIOHandler->pGPIOx->AFR[0] &= ~(0b111 << auxPosition); //Limpiar
            pGPIOHandler->pGPIOx->AFR[0] |=  (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition); //Escribir
        } else {
        //Registro AFRH, para PIN_8 a PIN_15
        auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);
        pGPIOHandler->pGPIOx->AFR[1] &= ~(0b111 << auxPosition); //Limpiar
        pGPIOHandler->pGPIOx->AFR[1]|=  (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition); //Escribir
        }
    }
}

void GPIO_WritePin(GPIO_Handler_t * pPinHandler, uint8_t newState){
    if(newState == /*SET*/ 0b1){
        pPinHandler->pGPIOx->BSRR |= (/*SET*/ 0b1 << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
    } else {
        pPinHandler->pGPIOx->BSRR |= (/*SET*/ 0b1 << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
    }

}

uint32_t GPIO_ReadPin(GPIO_Handler_t * pPinHandler){
    uint32_t pinValue = 0; //Variable auxiliar
    pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber); //Lectura en posicion requerida
    return pinValue;
}

void GPIOxTooglePin(GPIO_Handler_t *pPinHandler){
	// Utilizacion de XOR
		pPinHandler->pGPIOx->ODR ^= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
}
