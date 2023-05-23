/*
 * PLLDriver.h
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include <stm32f4xx.h>

#define HSI_VALUE   10000000	//10MHz
#define HSE_VALUE	80000000    //80 MHz

void configPLL(void);
uint32_t getConfigPLL(uint32_t freqClock);


#endif /* PLLDRIVER_H_ */
