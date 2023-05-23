/*
 * PLLDriver.c
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#include <stm32f4xx.h>
#include "PLLDriver.h"

//Función para cargar la configuración de los registros
void configPLL(void){

	//Activamos la señal de reloj del Power Control
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

//	//Para trabajar con velocidades >= 84 MHz y <= 100MHz
//	PWR->CR &= ~(PWR_CR_VOS);
//	PWR->CR |= (PWR_CR_VOS);

	//Wait states to the Latency
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN ;
	FLASH->ACR  &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= (FLASH_ACR_LATENCY_2WS); //64 < HCLK ≤ 90

	/* Modify the CPU clock */
	//SW : System clock switch
	RCC -> CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_1;   // PLL selected as system clock

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC); // HSI clock selected as PLL clock entry

	//Se configura el valor que queremos obtener
	//f(VCO clock) = 16 MHz * (PLLN/PLLM)

	// PLLN = 60 MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
	RCC->PLLCFGR |= (0x3C << RCC_PLLCFGR_PLLN_Pos);

	// PLLM = 2 MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLM_1);

	//f(Clock output) = f(VCO clock) / PLLP
	//La frecuencia deseada es 80 MHz, entonces PLLP = 6 MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLP_0);

	/* División de los buses */
	//AHB prescaler, no se divide (trabaja a 100MHz > 80 MHz)
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
	RCC->CFGR |= (RCC_CFGR_HPRE_DIV1);

	//APB1 prescaler, se divide por 2, ya que trabaja a 50 MHz
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

	//APB2 prescaler, no se divide (trabaja a 100MHz > 80 MHz)
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

	// Seleccionamos la señal PLL
	RCC -> CFGR |= RCC_CFGR_MCO1;

	//Division de 5 (preescaler)
	RCC ->CFGR &= RCC_CFGR_MCO1PRE;
	RCC -> CFGR |= RCC_CFGR_MCO1_1;

	/*Se enciende el PLL*/
	RCC->CR |= RCC_CR_PLLON;

	// Esperamos que el PLL se cierre (estabilizacion)
	while (!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

}

//Función que entrega la frecuencia
uint32_t getConfigPLL(uint32_t freqClock){

	freqClock = 0;

	uint32_t valuePlln = ((RCC_PLLCFGR_PLLN & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos);
	uint32_t valuePllm = ((RCC_PLLCFGR_PLLM & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos);

	if( freqClock == 16 ){

		freqClock = HSI_VALUE ;

	}
	else{

		freqClock = ((HSE_VALUE * valuePlln) / valuePllm) / 2 ;		//valuePLLP = 2
	}

	return freqClock ;

}
