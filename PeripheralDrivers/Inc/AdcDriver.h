/*
 * AdcDriver.h
 *
 *  Created on: May 30, 2023
 *      Author: sjacome
 */

#ifndef ADCDRIVER_H_
#define ADCDRIVER_H_

#include "stm32f4xx.h"

#define ADC_CHANNEL_0		0
#define ADC_CHANNEL_1		1
#define ADC_CHANNEL_2		2
#define ADC_CHANNEL_3		3
#define ADC_CHANNEL_4		4
#define ADC_CHANNEL_5		5
#define ADC_CHANNEL_6		6
#define ADC_CHANNEL_7		7
#define ADC_CHANNEL_8		8
#define ADC_CHANNEL_9		9
#define ADC_CHANNEL_10		10
#define ADC_CHANNEL_11		11
#define ADC_CHANNEL_12		12
#define ADC_CHANNEL_13		13
#define ADC_CHANNEL_14		14
#define ADC_CHANNEL_15		15
#define ADC_CHANNEL_16		16

#define CC1_EVENT_TIM1		0
#define CC2_EVENT_TIM1		1
#define CC3_EVENT_TIM1		2
#define CC2_EVENT_TIM2		3
#define CC3_EVENT_TIM2		4
#define CC4_EVENT_TIM2		5
#define TRGO_EVENT_TIM2		6
#define CC1_EVENT_TIM3		7
#define TRGO_EVENT_TIM3		8
#define CC4_EVENT_TIM4		9
#define CC1_EVENT_TIM5		10
#define CC2_EVENT_TIM5		11
#define CC3_EVENT_TIM5		12
#define EXTI_LINE11			15


#define ADC_RESOLUTION_12_BIT	0
#define ADC_RESOLUTION_10_BIT	1
#define ADC_RESOLUTION_8_BIT	2
#define ADC_RESOLUTION_6_BIT	3

#define ADC_ALIGNMENT_RIGHT		0
#define ADC_ALIGNMENT_LEFT		1

#define ADC_SAMPLING_PERIOD_3_CYCLES	0b000;
#define ADC_SAMPLING_PERIOD_15_CYCLES	0b001;
#define ADC_SAMPLING_PERIOD_28_CYCLES	0b010;
#define ADC_SAMPLING_PERIOD_56_CYCLES	0b011;
#define ADC_SAMPLING_PERIOD_84_CYCLES	0b100;
#define ADC_SAMPLING_PERIOD_112_CYCLES	0b101;
#define ADC_SAMPLING_PERIOD_144_CYCLES	0b110;
#define ADC_SAMPLING_PERIOD_480_CYCLES	0b111;

typedef struct
{
	uint8_t		channel;		// Canal ADC que será utilizado para la conversión ADC
	uint8_t		resolution;		// Precisión con la que el ADC hace la adquisición del dato
	uint16_t	samplingPeriod;	// Tiempo deseado para hacer la adquisición del dato
	uint8_t		dataAlignment;	// Alineación a la izquierda o a la derecha
	uint16_t	adcData;		//Dato de la conversión
	uint8_t     numeroDeCanales; //El numero de canales
	uint8_t 	channels[16]; //Arreglo que guarda los canales del ADC

}ADC_Config_t;

void adc_Config(ADC_Config_t *adcConfig);
void configAnalogPin(uint8_t adcChannel);
void adcComplete_Callback(void);
void startSingleADC(void);
void startContinousADC(void);
void ADC_ConfigMultichannel (ADC_Config_t *adcConfig);
uint16_t getADC(void);
void muestreoADC(uint8_t numero);

#endif /* ADCDRIVER_H_ */
