/*
 * AdcDriver.c
 *
 *  Created on: May 30, 2023
 *      Author: sjacome
 */

#ifndef ADCDRIVER_C_
#define ADCDRIVER_C_
#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t	adcRawData = 0;

void adc_Config(ADC_Config_t *adcConfig){
	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */
	configAnalogPin(adcConfig->channel);

	/* 2. Activamos la señal de reloj para el periférico ADC1 (bus APB2)*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración del ADC1 */
	/* 3. Resolución del ADC */
	switch(adcConfig->resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}

	case ADC_RESOLUTION_10_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		break;
	}

	case ADC_RESOLUTION_8_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_6_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	default:
	{
		break;
	}
	}

	/* 4. Configuramos el modo Scan como desactivado */
	ADC1->CR1 &= ~(ADC_CR1_SCAN);

	/* 5. Configuramos la alineación de los datos (derecha o izquierda) */
	if(adcConfig->dataAlignment == ADC_ALIGNMENT_RIGHT){
		// Alineación a la derecha (esta es la forma "natural")
		ADC1->CR2 &= ~(ADC_CR2_ALIGN);
	}
	else{

		// Alineación a la izquierda (para algunos cálculos matemáticos)
		ADC1->CR2 |= ADC_CR2_ALIGN;
	}

	/* 6. Desactivamos el "continuos mode" */
	ADC1->CR2 &= ~ (ADC_CR2_CONT);

	/* 7. Acá se debería configurar el sampling...*/
	if(adcConfig->channel < ADC_CHANNEL_9){
		ADC1->SMPR2 |= (adcConfig->samplingPeriod << (3* (adcConfig->channel)));
	}
	else{
		ADC1->SMPR1 |= (adcConfig->samplingPeriod << (3* (adcConfig->channel)));
	}

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */
	// Al hacerlo todo 0, estamos seleccionando solo 1 elemento en el conteo de la secuencia
	ADC1->SQR1 = 0;

	// Asignamos el canal de la conversión a la primera posición en la secuencia
	ADC1->SQR3 |= (adcConfig->channel << 0);

	/* 9. Configuramos el preescaler del ADC en 2:1 (el mas rápido que se puede tener */
	ADC->CCR |= ADC_CCR_ADCPRE_0;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. Activamos la interrupción debida a la finalización de una conversión EOC (CR1)*/
	ADC1->CR1 |= ADC_CR1_EOCIE;

	/* 11a. Matriculamos la interrupción en el NVIC*/
	__NVIC_EnableIRQ(ADC_IRQn);

	/* 11b. Configuramos la prioridad para la interrupción ADC */
    __NVIC_SetPriority(ADC_IRQn, 7);
	/* 12. Activamos el modulo ADC */
    ADC1->CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
    __enable_irq();
}
void ADC_ConfigMultichannel (ADC_Config_t *adcConfig){
	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */
	for(int i = 0; i < adcConfig->numeroDeCanales; i++){
		configAnalogPin(adcConfig->channels[i]);
	}


	/* 2. Activamos la señal de reloj para el periférico ADC1 (bus APB2)*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración del ADC1 */
	/* 3. Resolución del ADC */
	switch(adcConfig->resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;
	}

	case ADC_RESOLUTION_10_BIT:
	{

		ADC1->CR1 |= ADC_CR1_RES_0;
		break;
	}

	case ADC_RESOLUTION_8_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_6_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	default:
	{
		break;
	}
	}

	/* 4. Configuramos el modo Scan como desactivado */
	ADC1->CR1 |= ADC_CR1_SCAN;

	/* 5. Configuramos la alineación de los datos (derecha o izquierda) */
	if(adcConfig->dataAlignment == ADC_ALIGNMENT_RIGHT){
		// Alineación a la derecha (esta es la forma "natural")
		ADC1->CR2 &= ~(ADC_CR2_ALIGN);
	}
	else{

		// Alineación a la izquierda (para algunos cálculos matemáticos)
		ADC1->CR2 |= ADC_CR2_ALIGN;
	}

	/* 7. Acá se debería configurar el sampling para multiples canales */
	for(int i = 0; i< adcConfig->numeroDeCanales; i++){
		if((adcConfig->channels)[i] < ADC_CHANNEL_9){
			ADC1->SMPR2 |= (adcConfig->samplingPeriod << (3* (adcConfig->channels)[i]));
		} else {
			ADC1->SMPR1 |= (adcConfig->samplingPeriod << (3 * (((adcConfig->channels)[i]) - 10)));
		}
	}

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */

	// Seleccionamos cuantos canales se requieren para la conversión
	ADC1->SQR1 = ((adcConfig->numeroDeCanales - 1) << ADC_SQR1_L_Pos);

	//Asignar la secuencia de los canales segun en la posición correspondiente a la lista channels
	for(int i = 0; i< adcConfig->numeroDeCanales; i++){
		if(i<6){
			ADC1->SQR3 |= (adcConfig->channels)[i] << (5 * i);
		}
		else if(i>=6 && i<12){
			ADC1->SQR2 |= (adcConfig->channels)[i] << (5 * (i-6));
		}
		else if(i>=12){
			ADC1->SQR1 |= (adcConfig->channels)[i] << (5 * (i-12));
		}
	}

	/* 9. Configuramos el preescaler del ADC en 2:1 (el mas rápido que se puede tener */
	ADC->CCR |= ADC_CCR_ADCPRE_0;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();


	/* 11. Activamos la interrupción debida a la finalización de una secuencia de conversión EOCS (CR2)*/
	ADC1->CR1 |= ADC_CR1_EOCIE;
	ADC1->CR2 |= ADC_CR2_EOCS;

	/* 11a. Matriculamos la interrupción en el NVIC*/
	__NVIC_EnableIRQ(ADC_IRQn);

	/* 11b. Configuramos la prioridad para la interrupción ADC */
    __NVIC_SetPriority(ADC_IRQn, 7);
	/* 12. Activamos el modulo ADC */
    ADC1->CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
    __enable_irq();

}
/*
 * Esta función lanza la conversion ADC y si la configuración es la correcta, solo se hace
 * una conversion ADC.
 * Al terminar la conversion, el sistema lanza una interrupción y el dato es leido en la
 * función callback, utilizando la funciona getADC().
 *
 * */
void startSingleADC(void){
	/* Desactivamos el modo continuo de ADC */
	ADC1->CR2 &= ~ (ADC_CR2_CONT);

	/* Limpiamos el bit del overrun (CR1) */
	ADC1->CR1 &= ~(ADC_CR1_OVRIE);

	/* Iniciamos un ciclo de conversión ADC (CR2)*/
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

/*
 * Esta función habilita la conversion ADC de forma continua.
 * Una vez ejecutada esta función, el sistema lanza una nueva conversion ADC cada vez que
 * termina, sin necesidad de utilizar para cada conversion el bit SWSTART del registro CR2.
 * Solo es necesario activar una sola vez dicho bit y las conversiones posteriores se lanzan
 * automaticamente.
 * */
void startContinousADC(void){

	/* Activamos el modo continuo de ADC */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Iniciamos un ciclo de conversión ADC */
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

/*
 * Función que retorna el ultimo dato adquirido por la ADC
 * La idea es que esta función es llamada desde la función callback, de forma que
 * siempre se obtiene el valor mas actual de la conversión ADC.
 * */
uint16_t getADC(void){
	// Esta variable es actualizada en la ISR de la conversión, cada vez que se obtiene
	// un nuevo valor.
	return adcRawData;
}


/*
 * Esta es la ISR de la interrupción por conversión ADC
 */
void ADC_IRQHandler(void){
	// Evaluamos que se dio la interrupción por conversión ADC
	if(ADC1->SR & ADC_SR_EOC){
		//if( ADC1->CR1 & ADC_CR1_SCAN){
		// Leemos el resultado de la conversión ADC y lo cargamos en una variale auxiliar
		// la cual es utilizada en la función getADC()
		adcRawData = ADC1->DR;

		// Hacemos el llamado a la función que se ejecutará en el main
		adcComplete_Callback();
	}

}

/* Función debil, que debe ser sobreescrita en el main. */
__attribute__ ((weak)) void adcComplete_Callback(void){
	__NOP();
}

/*
 * Con esta función configuramos que pin deseamos que funcione como canal ADC
 * Esta funcion trabaja con el GPIOxDriver, por lo cual requiere que se incluya
 * dicho driver.
 */
void configAnalogPin(uint8_t adcChannel) {

	// Con este switch seleccionamos el canal y lo configuramos como análogo.
	switch (adcChannel) {

	case ADC_CHANNEL_0: {
		// Es el pin PA0
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		// Nota: Para el ejercicio inicial solo se necesita este canal, los demas
		// se necesitan para trabajos posteriores.
		break;
	}
		;

	case ADC_CHANNEL_1: {
		// Buscar y configurar adecuadamente
		// Es el pin PA1
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;

		break;
	}

	case ADC_CHANNEL_2: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;

		break;
	}

	case ADC_CHANNEL_3: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;

		break;
	}

	case ADC_CHANNEL_4: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;

		break;
	}

	case ADC_CHANNEL_5: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;

		break;
	}
	case ADC_CHANNEL_6: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_6;

		break;
	}
	case ADC_CHANNEL_7: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx                        = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_7;

		break;
	}
	case ADC_CHANNEL_8: {
		//Es el pin PB0
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_9: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_10: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_11: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_12: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}
	case ADC_CHANNEL_13: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}
	case ADC_CHANNEL_14: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}
	case ADC_CHANNEL_15: {
		// Buscar y configurar adecuadamente
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	default: {
		break;
	}

	}
	// Despues de configurar el canal adecuadamente, se define este pin como Analogo y se
	// carga la configuración con el driver del GPIOx
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);
}

void muestreoADC(uint8_t numero){

	//Trigger detection on the rising edge
	ADC1->CR2 &= ~(ADC_CR2_EXTEN);
	ADC1->CR2 |= ADC_CR2_EXTEN_0;

	switch(numero){
	case CC1_EVENT_TIM1:{
		ADC1->CR2 &= ~ADC_CR2_EXTSEL;
		break;
	}
	case CC2_EVENT_TIM1:{
		ADC1->CR2 |= ADC_CR2_EXTSEL_0;
		break;
	}
	case CC3_EVENT_TIM1:{
		ADC1->CR2 |= ADC_CR2_EXTSEL_1;
		break;
	}
	case CC2_EVENT_TIM2:{
		ADC1->CR2 |= (3 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case CC3_EVENT_TIM2:{
		ADC1->CR2 |= ADC_CR2_EXTSEL_2;
		break;
	}
	case CC4_EVENT_TIM2:{
		ADC1->CR2 |= (5 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case TRGO_EVENT_TIM2:{
		ADC1->CR2 |= (6 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case CC1_EVENT_TIM3:{
		ADC1->CR2 |= (7 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case TRGO_EVENT_TIM3:{
		ADC1->CR2 |= ADC_CR2_EXTSEL_3;
		break;
	}
	case CC4_EVENT_TIM4:{
		ADC1->CR2 |= (9 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case CC1_EVENT_TIM5:{
		ADC1->CR2 |= (10 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case CC2_EVENT_TIM5:{
		ADC1->CR2 |= (11 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case CC3_EVENT_TIM5:{
		ADC1->CR2 |= (12 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	case EXTI_LINE11:{
		ADC1->CR2 |= (15 << ADC_CR2_EXTSEL_Pos);
		break;
	}
	default: {
			break;
	}
	}
}

#endif /* ADCDRIVER_C_ */
