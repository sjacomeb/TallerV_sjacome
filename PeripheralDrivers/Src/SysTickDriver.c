/*
 * SysTickDriver.c
 *
 *  Created on: May 18, 2023
 *      Author: sjacome
 *
 * Este driver controla el Timer que trae por defecto todo procesador ARM Cortex Mx,
 * el cual hace parte del sistema independiente de la empresa fabricante del MCU.
 *
 * Para encontrar cual es su registro de configuración, debemos utilizar el manual
 * genérico del procesador Cortex-M4, ya que es allí donde se encuentra la documentación
 * para este periférico.
 *
 * En el archivo core_cm4.h, la estructura que define el periférico se llama SysTick_Type.
 *
 */

#include <stm32f4xx.h>
#include <SysTickDriver.h>

uint64_t ticks = 0;
uint64_t ticks_start = 0;
uint64_t ticks_counting = 0;


void config_SysTick_ms(uint8_t systemClock){
	//Reiniciamos el valor de la variable que cuenta tiempo
	ticks = 0;

	//Cargando el valor del límite de incrementos que representan 1ms
	switch(systemClock){

	//Caso para el reloj HSI -> 16MHz
	case 0:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;

	//Caso para el reloj HSE
	case 1:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;

	//Caso para el reloj PLL a 100MHz
	case 2:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_100MHz_1ms;
		break;

	default:
		SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;
		break;
	}

	//Limpiamos el valor actual del SysTick
	SysTick->VAL = 0;

	//Configuramos el reloj interno como el reloj par el Timer
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	//Desactivamos las interrupciones globales
	__disable_irq();

	//Matriculamos la interrupción en el NVIC
	NVIC_EnableIRQ(SysTick_IRQn);

	//Activamos la interrupción debida al conteo a cero del SysTicks
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	//Activamos el Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	//Activamos de nuevo las interrupciones globales
	__enable_irq();

}

uint64_t getTicks_ms(void){
	return ticks;
}

void delay_ms(uint32_t wait_time_ms){
	//Captura el primer valor de tiempo para comparar
	ticks_start = getTicks_ms();

	//Captura el segundo valor de tiempo para comparar
	ticks_counting = getTicks_ms();

	/* Compara: Si el valor "counting" es menor que el "start + wait"
	 * actualiza el valor "counting"
	 * Repite esta operación hasta que el counting sea mayor*/
	while(ticks_counting < (ticks_start+ (uint64_t)wait_time_ms)){

		//Actualizar el valor
		ticks_counting = getTicks_ms();
	}
}


void SysTick_Handler(void){
	//Verificamos que la interrupción se lanzó
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		//Limpiamos la bandera
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		//Incrementamos en 1 el contador
		ticks++;
	}
}



