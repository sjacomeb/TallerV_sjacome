/*
 * PLLDriver.h
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include <stm32f4xx.h>

#define PLLP_2      0
#define PLLP_4      1
#define PLLP_6      2
#define PLLP_8      3

#define MCO1PRE_2   4   //division entre 2
#define MCO1PRE_3   5   //division entre 3
#define MCO1PRE_4   6   //division entre 4
#define MCO1PRE_5   7   //division entre 5

#define HSI_CLOCK       0
#define LSE_OSCILLATOR  1
#define PLL_CLOCK       3

//Estructura del PLL
typedef struct
{
	uint16_t     PLLN;
	uint8_t      PLLM;
	uint8_t      PLLP;
	uint8_t      MC01PRE;
	uint8_t      CLOCK;
}PLL_Config_t;


void configPLL(PLL_Config_t *ptrhandlerPLL);
void updateClock(PLL_Config_t *ptrhandlerPLL, uint8_t clock);
void updatePreescaler(PLL_Config_t *ptrhandlerPLL, uint8_t preescaler);
uint16_t getConfigPLL(void);


#endif /* PLLDRIVER_H_ */
