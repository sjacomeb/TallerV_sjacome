/*
 * mainTalleres.c
 *
 *  Created on: Mar 30, 2023
 *      Author: sjacome
 */

#include <stdint.h>

// Creacion de una funcion que duplique el valor de un numero

// 1.0 Paso por valor Básico

void duplicarNumero(uint8_t numero);

// 1.1 Paso por referencia

void duplicarNumeroRef(uint8_t *numero);

// 1.2 Paso por valor reasignando variables.

uint8_t duplicarNumeroReturn(uint8_t numero);

// 1.3 Arreglos por Referencia

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas);

// ***** // SOLUCION EJERCICIO // ***** //

void stringCaseConverter(uint8_t *string);

int main(void){

	uint8_t variable = 10;

	//duplicarNumero(variable);

	//variable = duplicarNumeroReturn(variable);

	duplicarNumeroRef(&variable);

	uint16_t deudas[] = {10000, 50000, 2500, 4000};

	abonarDeudas (deudas,4);     //No se coloca & porque un arreglo ya es un puntero

	// 1.5 EJERCICIO:

		// Crear una función llamada stringCaseConverter que no retorne ningún
		// valor, y reciba una string.

		// Esta función deberá cambiar las minúsculas por mayúsculas y viceversa
		// del string original.


}

void duplicarNumero(uint8_t numero){        //Paso por valor

	numero = numero*2;
}

uint8_t duplicarNumeroReturn(uint8_t numero){

	return numero*2;
}

void duplicarNumeroRef(uint8_t *numero){       //Paso por referencia

	*numero = (*numero)*2;
}

void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas){         //misDeudas[]   =     *misDeudas

	for(uint8_t i=0; i<cantidadDeudas; i++){

		//misDeudas[i] = misDeudas[i]/2;         //Es lo mismo que la cuarta linea
		//*misDeudas = *misDeudas/2;            //Accedemos al primer elemento del arreglo con puntero
		//*(misDeudas+1) = *(misDeudas+1)/2;   //Accedemos al segundo elemento del arreglo
		*(misDeudas+i) = *(misDeudas+i)/2;      //Dividimos a la mitad el  valor de cada posición del arreglo
	}
}

void stringCaseConverter(uint8_t *string){      //El ultimo valor de un string (arreglo) es cero

	uint8_t i = 0;      //Contador
	while(*(string+i)!= 0){

		//Para mayúsculas
		if((*(string+i)>64) && (*(string+i)<91)){
			*(string+i) = *(string+i)+32;
		}
		//Para minúsculas
		else if((*(string+i)>64) && (*(string+i)<91)){
			*(string+i) = *(string+i)-32;
		}
		//Dejar igual cualquier otro caracter
		else{
			*(string+i) = *(string+i);
		}
		i++;   //Aumentamos el contador
	}
}

