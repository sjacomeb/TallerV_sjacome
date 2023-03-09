/*
 * tallerSemana5.c
 *
 *  Created on: Mar 9, 2023
 *      Author: sjacome
 */

//Librerías para las variables
#include <stdint.h>

//librerías para variables booleanas
#include <stdbool.h>

//Librerías para usar funciones matemáticas
#include <math.h>

#define CONSTANTE 100
#define UNSIGNED 0
#define SIGNED 1

bool variableBooleana = true;
uint8_t parametro1 = 100;
int16_t parametro2 = -173;

// Header funciones
void clearGlobal (void);
uint8_t getMaxChar(void);
uint16_t getMaxValue (uint16_t x,uint16_t y,uint16_t z);
uint64_t getMaxVariable (uint8_t bit, uint8_t signo);

int main(void){
	clearGlobal(); //función vacía
	parametro1 = getMaxChar(); //hay que ponerla en una variable porque la función no es vacía
	uint16_t parametro3 = getMaxValue(200,100,50);
	(void) parametro3; //para evitar warning de unused variable
	uint64_t parametro4 = getMaxVariable(16,SIGNED);
	(void) parametro4;
	uint8_t bin = 0b10010110;
	uint8_t dec = 0;
	uint8_t aux = 0;

	//Convertidor de binario a decimal
	for(uint8_t i = 0; i < 8; i++){
		aux = bin & (1<<i);
		aux = aux >> i;
		aux = aux * pow (2, i);
		dec = dec + aux;
	}
}

//Función que vuelve las variables a cero
void clearGlobal (void){
	variableBooleana = false;
	parametro1 = 0;
	parametro2 = 0;
}

//Función del valor máximo que almacena un char (0-255)
uint8_t getMaxChar(void){
	uint8_t maxChar = pow(2,8)-1;
	return maxChar;
}

//Función que entrega el número mayor de 3 entradas
uint16_t getMaxValue (uint16_t x,uint16_t y,uint16_t z){
	if ((x>=y) && (x>=z)){
		return x;
	}else if((y>=x) && (y>=z)){
		return y;
	}else{
		return z;
	}
}

// Funcion que indica el valor máximo que almacena una variable
uint64_t getMaxVariable (uint8_t bit, uint8_t signo){
	uint64_t maxVar;

	if (bit == 8 || bit == 16 || bit ==32 || bit == 64){
		if (signo == SIGNED){
			maxVar = pow(2, bit)/2 -1;
		}else if (signo == UNSIGNED){
			maxVar = pow(2, bit) -1;
		}else{
			maxVar = 0;
		}
	}else{
		maxVar = 0;
	}
	return maxVar;
}
