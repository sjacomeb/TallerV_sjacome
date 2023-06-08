/*
 * PLLDriver.c
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#include <stm32f4xx.h>
#include "PLLDriver.h"

uint16_t valuePlln = 0;
uint16_t valuePllm = 0;
uint16_t valuePllp = 2;
uint16_t frecuencia = 0;

uint8_t clock = 0;
uint8_t preescaler = 0;

/* Esta configuración es para 80 MHz */
//Función para cargar la configuración de los registros
void configPLL(PLL_Config_t *ptrhandlerPLL){

	/* Configuración HSI*/
	//Desactivo el reloj
	RCC->CR &= ~(RCC_CR_HSION);

	//Limpio el registro del ajuste
	RCC->CR &= ~(RCC_CR_HSITRIM);

	//Ajuste para obtener exactamente 16 MHz
	RCC->CR |= (12 << RCC_CR_HSITRIM_Pos);

	//Activo el reloj
	RCC->CR |= RCC_CR_HSION;

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC); // HSI clock selected as PLL clock entry

	/* Configuración LSE */
	//Se habilita la señal del bus
	RCC->APB1ENR &= ~(RCC_APB1ENR_PWREN);
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	//Se quita la protección de los bits para modificar el LSE
	PWR->CR &= ~(PWR_CR_DBP);
	PWR->CR |= PWR_CR_DBP;

	//Encendemos el LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON);
	RCC->BDCR |= RCC_BDCR_LSEON;

	/* Configuración PLL (100 MHz)*/
	//f(VCO clock) = 16 MHz * (PLLN/PLLM)

	//Wait states to the Latency
	FLASH->ACR  &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= (FLASH_ACR_LATENCY_3WS); //90 < HCLK ≤ 100

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
	RCC->PLLCFGR |= (ptrhandlerPLL->PLLN << RCC_PLLCFGR_PLLN_Pos);

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (ptrhandlerPLL->PLLM << RCC_PLLCFGR_PLLM_Pos);

	//f(Clock output) = f(VCO clock) / PLLP
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
	RCC->PLLCFGR |= (ptrhandlerPLL->PLLP << RCC_PLLCFGR_PLLP_Pos);

	/* División de los buses */
	//AHB prescaler, no se divide (trabaja a 100MHz > 80 MHz)
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
	RCC->CFGR |= (RCC_CFGR_HPRE_DIV1);

	//APB1 prescaler, se divide por 2 (40 MHz), ya que trabaja a 50 MHz
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

	//APB2 prescaler, no se divide (trabaja a 100MHz > 80 MHz)
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

	// Seleccionamos la señal PLL
	RCC -> CFGR |= RCC_CFGR_MCO1;

	//Preescaler
	RCC ->CFGR &= ~(RCC_CFGR_MCO1PRE_0);
	RCC -> CFGR |= (ptrhandlerPLL->MC01PRE << RCC_CFGR_MCO1PRE_Pos);

	/*Se enciende el PLL*/
	RCC->CR |= RCC_CR_PLLON;

	// Esperamos que el PLL se cierre (estabilizacion)
	while (!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	//SW : System clock switch
	RCC->CFGR |= RCC_CFGR_SW_1;  // PLL selected as system clock

}

/* Función que selecciona la señal de reloj */
void updateClock(PLL_Config_t *ptrhandlerPLL, uint8_t clock){

	RCC->CFGR &= ~(RCC_CFGR_MCO1);

	RCC->CFGR |= (clock << RCC_CFGR_MCO1_Pos);
}

/* Función que configura el preescaler de la señal elegida */
void updatePreescaler(PLL_Config_t *ptrhandlerPLL, uint8_t preescaler){


	RCC->CFGR &= ~(RCC_CFGR_MCO1PRE);

	RCC->CFGR |= (preescaler << RCC_CFGR_MCO1PRE_Pos);

}

/* Función que entrega la frecuencia en MHz */
uint16_t getConfigPLL(void){

	//Guarda el valor de los registros en las variables

	valuePlln = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos);
	valuePllm = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos);
	valuePllm &= (RCC_PLLCFGR_PLLM);

	uint16_t frecuencia = ((16 * valuePlln) / (valuePllm * valuePllp)) ;

	return frecuencia;
}

