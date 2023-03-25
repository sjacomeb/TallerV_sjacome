/*
 * project_main.c
 *
 *  Created on: Mar 24, 2023
 *      Author: sjacome
 */


#include <stdint.h>
#include "stm32f4xx.h"

//Definición de elementos del programa

uint32_t counterG2 = 0;
uint32_t auxVariableG2 = 0;
uint32_t *ptr_CounterG2;
uint8_t byteVariableG2;
uint8_t *ptr_ByteVariableG2;

int main(void){

	/*Trabajndo con las variables y punteros*/
	counterG2 = 3456789 ;        //Asignar un valor a la variable
	auxVariableG2 = counterG2;   //C es pasado por valor

	ptr_CounterG2 = &counterG2; //Pasando la posición de memoria del counterG2

	*ptr_CounterG2 = 9876543;   //Escribimos un dato en la casilla de memoria a donde el puntero apunta

	ptr_CounterG2++;            //Movemos 1 posición de memoria el puntero
	*ptr_CounterG2 = 9876543;   //Escribimos de nuevo en la nueva variable

	byteVariableG2 = 234;
	ptr_ByteVariableG2 = &byteVariableG2;
	*ptr_ByteVariableG2 = 87;

	ptr_CounterG2 = &counterG2;
	auxVariableG2 = (uint32_t)ptr_CounterG2;     //Casting

	ptr_ByteVariableG2 = (uint8_t *)auxVariableG2;
	*ptr_ByteVariableG2 = 2;

	while(1){

	}

	return 0;
}
