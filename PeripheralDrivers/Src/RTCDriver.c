/*
 * RTCDriver.c
 *
 *  Created on: Jun 7, 2023
 *      Author: sjacome
 */

#include "RTCDriver.h"
#include <string.h>
#include <stdio.h>

//Arreglos para guardar fecha y hora
uint8_t tiempo[3];
uint8_t fecha[3];

void RTC_config(RTC_Config_t *ptrhandlerRTC){

	/* Encender interfaz de potencia */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// Modificación de bit que protege contra escritura
	//para acceder a los registros del RTC
	PWR->CR |= PWR_CR_DBP;

	/* Activar LSE, señal de reloj para RTC */
	RCC->BDCR |= RCC_BDCR_LSEON;

	/* Esperar estabilidad LSE */
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}

	/* Elegir LSE como señal de reloj para RTC */
	RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	/* Encender RTC clock */
	RCC->BDCR |= RCC_BDCR_RTCEN;

	/* Escritura de llaves para desbloquear RTC */
	RTC->WPR = RTC_KEY1;
	RTC->WPR = RTC_KEY2;

	writeRegisterRTC(ptrhandlerRTC);

}

//Función que contiene los accesos al RTC y los registros
void writeRegisterRTC(RTC_Config_t *ptrhandlerRTC){

	/* Inicializamos el RCT */
	RTC->ISR |= RTC_ISR_INIT;

	/* Esperar a estado de modificación */
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}

	/* Desactivar los registros oscuros */
	RTC->CR |= RTC_CR_BYPSHAD;

	/* Inicializamos los registros */
	RTC->TR = 0;
	RTC->DR = 0;

	/* Configurar como default formato 24 horas*/
	RTC->CR &= ~RTC_CR_FMT;

	/* Configuración del tiempo */
	RTC->TR |= decToBCD(ptrhandlerRTC->seconds) << RTC_TR_SU_Pos;
	RTC->TR |= decToBCD(ptrhandlerRTC->minutes) << RTC_TR_MNU_Pos;
	RTC->TR |= decToBCD(ptrhandlerRTC->hour) << RTC_TR_HU_Pos;

	/* Configuración de la fecha */
	RTC->DR |= decToBCD(ptrhandlerRTC->day) << RTC_DR_DU_Pos;
	RTC->DR |= decToBCD(ptrhandlerRTC->month)<< RTC_DR_MU_Pos;
	RTC->DR |= decToBCD(ptrhandlerRTC->year) << RTC_DR_YU_Pos;

	/* Encendido de registros oscuros */
	RTC->CR &= ~RTC_CR_BYPSHAD;

	/* Desactivar estado de inicialización */
	RTC->ISR &= ~RTC_ISR_INIT;

	/* Esperar desactivación de estado de inicialización */
	while(RTC->ISR & RTC_ISR_INIT){
		__NOP();
	}

}


//Función que convierte decimales a formato BCD
uint8_t decToBCD(int numberDec){
	uint8_t numberBCD = (uint8_t)((numberDec/10*16) + (numberDec%10));
	return numberBCD;
}

uint8_t bcdToDec(uint8_t numberBCD){
	uint8_t numberDec = ((numberBCD/16*10) + (numberBCD%16));
	return numberDec;
}

//Función que lee la hora
uint8_t *readTime(void){

	tiempo[0] = bcdToDec((RTC->TR >> RTC_TR_HU_Pos) & 0b111111);
	tiempo[1] = bcdToDec((RTC->TR >> RTC_TR_MNU_Pos)& 0b1111111);
	tiempo[2] = bcdToDec((RTC->TR>> RTC_TR_SU_Pos) & 0b1111111);

	return tiempo;
}

//Función que lee la fecha
uint8_t *readDate(void){

	fecha[0]= bcdToDec((RTC->DR >> RTC_DR_DU_Pos) & 0b111111);
	fecha[1]= bcdToDec((RTC->DR >> RTC_DR_MU_Pos) & 0b11111);
	fecha[2]= bcdToDec((RTC->DR >> RTC_DR_YU_Pos) & 0b11111111);

	return fecha;
}

//Función que cambia la hora
void changeTime(RTC_Config_t *ptrhandlerRTC, int hora, int minutos){

	ptrhandlerRTC->hour = hora;
	ptrhandlerRTC->minutes = minutos;

	writeRegisterRTC(ptrhandlerRTC);

}

//Función que cambia la fecha
void changeDate(RTC_Config_t *ptrhandlerRTC, int dia, int mes, int año){

	ptrhandlerRTC->day = dia;
	ptrhandlerRTC->month = mes;
	ptrhandlerRTC->year = año;

	writeRegisterRTC(ptrhandlerRTC);

}




