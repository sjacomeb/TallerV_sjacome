/*
 * PLLDriver.h
 *
 *  Created on: May 20, 2023
 *      Author: sjacome
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include <stm32f4xx.h>

#define HSI_VALUE   16	//16 MHz
#define HSE_VALUE	80	//80 MHz

void configPLL(void);
uint8_t getConfigPLL(void);


#endif /* PLLDRIVER_H_ */
