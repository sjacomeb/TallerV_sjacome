/*
 * I2CDriver.c
 *
 *  Created on: May 19, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include "I2CDriver.h"

/* Recordar que se debe configurar los pines para el I2C (SDA y SCL)
 * para lo cual se necesita el modulo GPIO y los pines configurados
 * en el modo Alternate Function.
 * Además, estos pines deben ser configurados como salidas open-drain
 * y con las resitencias en modo pull-up
 */

void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	/* 1. Activamos la señal de reloj para el modulo I2C seleccionado */
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Reiniciamos el periférico, de foma que inicia en un estado conocido */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	/* 3. Indicamos cual es la velocidad del reloj principal, que es la señal utilizada
	 * por el periférico para generar la señal de reloj para el bus I2C*/
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); //Borramos la configuracion previa
	ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);

	/* 4. Configuramos el modo I2C en el que el sistema funciona.
	 * En esta configuracion se incluye tambien la velocidad del reloj
	 * y el tiempo máximo para el cambio de la señal (T-Rise).
	 * Todo comienza con los dos registros en 0 */
	ptrHandlerI2C->ptrI2Cx->CCR = 0;
	ptrHandlerI2C->ptrI2Cx->TRISE = 0;

	if(ptrHandlerI2C->modeI2C == I2C_MODE_SM){

		//Estamos en modo "standar" (SM Mode)
		//Seleccionamos el modo estandar
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		//VOLVER A CAMBIAR LUEGO A 16 MHZ
		//Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz_80MHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise máximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM_80MHz;
	}
	else{
		//Estamos en modo "Fast" (FM Mode)
		//Seleccionamos el modo Fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		//VOLVER A CAMBIAR LUEGO A 16 MHZ
		//Configuramos el registro que se encarga de generar la señal de reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_FM_SPEED_400KHz_80MHz << I2C_CCR_CCR_Pos);

		//Configuramos el registro que controla el tiempo T-Rise máximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM_80MHz;

	}

	/* 5. Activamos el modulo I2C */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

}

/* Generamos la condición de stop */
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 7. Generamos la condición de stop */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}


/* Condición de start*/
// 1. Verificamos que la línea no esta ocupada - bit "busy" en I2C_CR2
// 2. Generamos la señal "start"
/* 2.a Esperamos a que la bandera del evento "start" se levante.
 * Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 1. Verificamos que la linea no esta ocupada - bit "busy" en I2C_CR2 */
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}
	/* 2. Generamos la señal "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* 2.a Esperamos a que la bandera del evento "start" se levante.
	* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

/* Condición de restart */
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	/* 2. Generamos la señal "start" */
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* 2.a Esperamos a que la bandera del evento "start" se levante.
	* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

/* Activamos la indicación para no-ACK (indicación para el Slave de terminar)*/
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	/* Debemos escribir cero en la posición ACK del registro de control 1*/
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/* Activamos la indicación para ACK */
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	/* Debemos escribir uno en la posición ACK del registro de control 1*/
	ptrHandlerI2C->ptrI2Cx->CR1 |= ~I2C_CR1_ACK;
}

/* Enviar la dirección del esclavo  */
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){
	/* Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

	/* Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 * (en el siguiente paso se envia la dirección de memoria que se desea escribir */
	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	/* Esperamos hasta que la bandera del evento "addr" se levante
	 * (esto nos indica que la dirección fue enviada satisfactoriamente */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}
	/* Debemos limpiar la bandera de la recpeción de ACK de la "addr" para lo cual
	 * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */
	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

/* Envio de la dirección de memoria */
void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	/* Enviamos la direccion de memoria que deseamos leer */
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	/* Esperamos hasta que el byte sea transmitido */
	while (!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

/* Envio de un dato */
void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/* 5. Cargamos el valor que deseamos escribir */
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	/* 6. Esperamos hasta que el byte sea transmitido */
	while (!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

/* Lectura de un dato */
uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	/* Esperamos hasta que el byte entrante sea recibido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C->dataI2C;
}

/* Leer varios registros*/
void i2c_readMultiRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t numberRegister, uint8_t *auxReadString){

	/* 1. Gereamos la condición Start */
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Enviamos la dirección de memoria que deseamos leer */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* 4. Creamos una condición de reStart */
	i2c_reStartTransaction(ptrHandlerI2C);

	/* 5. Enviamos la dirección del esclavo y la indicación de LEER */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	/* 6. Enviamos el ack para que se envie después de cada lectura*/
	i2c_sendAck(ptrHandlerI2C);

	/* 7. Lectura de los datos que envia el esclavo hasta el penúltimo dato*/
	for(uint8_t i=0 ; i< numberRegister; i++){

		if(i == numberRegister-1){

			/* 8. Generamos la condición de NoAck, para que el Master no repsonda y el slave solo envie 1 byte */
			i2c_sendNoAck(ptrHandlerI2C);

			/* 9.Generamos la condición Stop, para que el slave se detenga después de 1 byte */
			i2c_stopTransaction(ptrHandlerI2C);

			//Lectura de los datos hasta el penúltimo
			auxReadString[i] = i2c_readDataByte(ptrHandlerI2C);
		}else{
			/* 10. Lectura de último dato */
			auxReadString[i] = i2c_readDataByte(ptrHandlerI2C);
		}
	}
}

/* Leer un registro */
uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){

	/* 0. Creamos una varibles auxiliar para recibir el dato que leemos */
	uint8_t auxRead = 0;

	/* 1. Gereamos la condición Start */
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Enviamos la dirección de memoria que deseamos leer */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* 4. Creamos una condición de reStart */
	i2c_reStartTransaction(ptrHandlerI2C);

	/* 5. Enviamos la dirección del esclavo y la indicación de LEER */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	/* 6. Leemos el dato que envia el esclavo */
	auxRead = i2c_readDataByte(ptrHandlerI2C);

	/* 7. Generamos la condición de NoAck, para que el Master no repsonda y el slave solo envie 1 byte */
	i2c_sendNoAck(ptrHandlerI2C);

	/* 8.Generamos la condición Stop, para que el slave se detenga después de 1 byte */
	i2c_stopTransaction(ptrHandlerI2C);

	return auxRead;

}

/* Escribir un registro */
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){

	/* 1. Generamos la condición Start */
	i2c_startTransaction(ptrHandlerI2C);

	/* 2. Enviamos la dirección del esclavo y la indicación de ESCRIBIR */
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	/* 3. Enviamos la dirección de memoria que deseamos escribir */
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	/* 4. Enviamos el valor que desemaos escribir en el registro seleccionado */
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	/* 5. Generamos la condición Stop, para que el slave se detenga despues de 1 byte */
	i2c_stopTransaction(ptrHandlerI2C);
}

void i2c_WriteSingle(I2C_Handler_t *ptrHandlerI2C, uint8_t newValue){
	//1. Generacion de Start bit
	i2c_startTransaction(ptrHandlerI2C);
	//2. Enviamos la direccion del Sclavo e indicamos que se desea escribir el Sclave
	i2c_sendSlaveAddressRW(ptrHandlerI2C,ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);
	//3. Enviamos la direccion de memoria que deseamos escribir NO SE ESPECIFICA
	//4. Enviamos el dato que se desea escribir en el Sclave
	i2c_sendDataByte(ptrHandlerI2C, newValue);
	//5. Generamos la condicion de Stop
	i2c_stopTransaction(ptrHandlerI2C);
}

