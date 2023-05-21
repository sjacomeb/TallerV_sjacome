/*
 * PLLDriver.c
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#include <stm32f4xx.h>
#include "PLLDriver.h"

void Pll_Config(void){
	/***********************Increasing the CPU frequency******************/
	/* 1) Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
	//LATENCY: These bits represent the ratio of the CPU clock period to the Flash memory access time.

	//3WS (4 CPU cycles) -> 90 < HCLK ≤ 100 -> Voltage range 2.7 V - 3.6 V

	FLASH->ACR &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= (FLASH_ACR_LATENCY_3WS);

	/* 2) Check that the new number of wait states is taken into account to access the Flash memory
          by reading the FLASH_ACR register*/

	/* 3) Modify the CPU clock source by writing the SW bits in the RCC_CFGR register*/

	RCC->CFGR |= RCC_CFGR_SWS_1;

	//Se configura el valor que queremos obtener, es decir 100MHz
	//f(VCO clock) = 16 MHz * (PLLN/PLLM)
	//Empezamos con el factor de multiplicación PLLN=100MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
	RCC->PLLCFGR |= (0x64 << RCC_PLLCFGR_PLLN_Pos);

	//Luego se configura el factor de  división PLLM = 2MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLM_2);

	//f(Clock output) = f(VCO clock) / PLLP
	//Como la frecuencia deseada es 100MHz, el PLLP = 8 MHz
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
	RCC->PLLCFGR |= (0x3 << RCC_PLLCFGR_PLLP_Pos);

	/* 4) If needed, modify the CPU clock prescaler by writing the HPRE bits in RCC_CFGR*/
	//Se pone en 0 ya que no se desea hacer ninguna división
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	/* 5) Check that the new CPU clock source or/and the new CPU clock prescaler value is/are
       taken into account by reading the clock source status (SWS bits) or/and the AHB
       prescaler value (HPRE bits), respectively, in the RCC_CFGR register. */
	// El APB1 no puede superar los 50 MHz por lo tanto se debe dividir en 2
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2);

	// El APB2 no puede superar los 100 MHz por lo tanto no se divide por ningun factor
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV1);

	/*Finalmente se activa el PLL*/
	RCC->CR |= RCC_CR_PLLON;
}

