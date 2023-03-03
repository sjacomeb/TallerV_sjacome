

#include <stdint.h>

int main(void)
{
    //Operaciones  básicas

	uint16_t varOperadores = 12;

	//Operaciones
	varOperadores = varOperadores +1;
	varOperadores ++;
	varOperadores += 1;

	//Modulo
	varOperadores = varOperadores % 2;
	varOperadores %= 2;

	//SHIFT

	uint8_t testVar = 0b00000101;

	testVar = testVar << 1;
	testVar >>=1;
	testVar = testVar <<4;

	// Operadores Bitwise

	uint8_t variableA = 0b00111101;
	uint8_t variableB = 0b11111111;

	//OR
	uint8_t resultado = variableA | variableB;

	//AND
	resultado = variableA & variableB;

	//NOT
	resultado = -resultado;

	//XOR
	resultado = variableA ^ variableB;

	//Varias operaciones
	resultado = -(0b111 << 2 );

	variableA &= resultado;
	variableA |= (0b11 << 6 );




}
