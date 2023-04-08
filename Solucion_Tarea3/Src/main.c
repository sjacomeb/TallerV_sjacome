/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"


/* Definición de los elementos */
GPIO_Handler_t 	handlerLED2 				= {0};
GPIO_Handler_t 	handlerButton 				= {0};
GPIO_Handler_t 	handlerCLK 					= {0};
GPIO_Handler_t 	handlerDT 					= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
GPIO_Handler_t	handlerTransistor1			= {0};
GPIO_Handler_t	handlerTransistor2			= {0};
GPIO_Handler_t	handlerPinDisplay_a			= {0};
GPIO_Handler_t	handlerPinDisplay_b			= {0};
GPIO_Handler_t	handlerPinDisplay_c			= {0};
GPIO_Handler_t	handlerPinDisplay_d			= {0};
GPIO_Handler_t	handlerPinDisplay_e			= {0};
GPIO_Handler_t	handlerPinDisplay_f			= {0};
GPIO_Handler_t	handlerPinDisplay_g			= {0};


/*Definición de los timers*/
BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t handlerTimerDisplay = {0};

/* Definición del exti */
EXTI_Config_t ExtiClock = {0};
EXTI_Config_t ExtiButton = {0};

/* Definición de las banderas */
uint8_t encoderEdge = 0;   //Bandera del reloj
uint8_t buttonEdge = 0;    //Bandera del botón

/* Variables */
uint8_t counter = 0;             //Contador del encoder
uint8_t s = 0;                   //Probar el funcionamiento del encoder
uint8_t t=0;                     //Estado del Data


/* Prototipos de funciones del main */
void init_Hardware(void);
void callback_extInt13(void);
void BasicTimer2_Callback(void);

int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();


	while(1){


	}

	return 0;
}


void init_Hardware(void){

	/* Configuración del led de estado */
	handlerLED2.pGPIOx = GPIOA;
	handlerLED2.GPIO_PinConfig.GPIO_PinNumber 	= PIN_5;
	handlerLED2.GPIO_PinConfig.GPIO_PinMode 	= GPIO_MODE_OUT;
	handlerLED2.GPIO_PinConfig.GPIO_PinOType	= GPIO_OTYPE_PUSHPULL;
	handlerLED2.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerLED2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerLED2);

	/* Configuración del TIM2 para controlar el blinky */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode		= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed		= BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period		= 250;       // Interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

//	/* Configuración del TIM5 que controla el Display */
//	handlerTimerDisplay.ptrTIMx 							= TIM5;
//	handlerTimerDisplay.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
//	handlerTimerDisplay.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
//	handlerTimerDisplay.TIMx_Config.TIMx_period 			= 250;                   //Periodo de 10ms
//	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUP_ENABLE;
//	BasicTimer_Config(&handlerTimerDisplay);

//	/* Configuración del CLK LISTO */
//	handlerCLK.pGPIOx = GPIOB;
//	handlerCLK.GPIO_PinConfig.GPIO_PinNumber 	      = PIN_8;
//	handlerCLK.GPIO_PinConfig.GPIO_PinMode 	          = GPIO_MODE_IN;
//	handlerCLK.GPIO_PinConfig.GPIO_PinOType	          = GPIO_OTYPE_PUSHPULL;
//	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed	          = GPIO_OSPEED_FAST;
//	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
//	GPIO_Config(&handlerCLK);

	/* Configuración del DT LISTO */
	handlerDT.pGPIOx = GPIOC;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	      = PIN_13;
	handlerDT.GPIO_PinConfig.GPIO_PinMode 	          = GPIO_MODE_IN;
	handlerDT.GPIO_PinConfig.GPIO_PinOType	          = GPIO_OTYPE_PUSHPULL;
	handlerDT.GPIO_PinConfig.GPIO_PinSpeed	          = GPIO_OSPEED_FAST;
	handlerDT.GPIO_PinConfig.GPIO_PinPuPdControl      = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDT);

//	/* Configuración del SW */
//	handlerButton.pGPIOx 										= GPIOA;
//	handlerButton.GPIO_PinConfig.GPIO_PinNumber					= PIN_9;
//	handlerButton.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_IN;
//	handlerButton.GPIO_PinConfig.GPIO_PinOType					= GPIO_OTYPE_PUSHPULL;
//	handlerButton.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_MEDIUM;
//	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
//	handlerButton.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;
//	GPIO_Config(&handlerButton);
//
//	/* Configuración del segmento "a" del display LISTO */
//	handlerPinDisplay_a.pGPIOx 												= GPIOB;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_8;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_a);
//
//	/* Configuración del  segmento "b" del display LISTO */
//	handlerPinDisplay_b.pGPIOx 												= GPIOC;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_9;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_b);
//
//	/* Configuración del segmento "c" del display LISTO */
//	handlerPinDisplay_c.pGPIOx 												= GPIOC;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_4;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_c);
//
//	/* Configuración del segmento "d" del display LISTO */
//	handlerPinDisplay_d.pGPIOx 												= GPIOA;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_11;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
//	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_d);
//
//	/* Configuración del segmento "e" del display LISTO */
//	handlerPinDisplay_e.pGPIOx 												= GPIOC;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_6;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_e);
//
//	/* Configuración del segmento "f" del display LISTO */
//	handlerPinDisplay_f.pGPIOx 												= GPIOC;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_8;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_f);
//
//	/* Configuración del segmento "g" del display LISTO */
//	handlerPinDisplay_g.pGPIOx 												= GPIOB;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_7;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerPinDisplay_g);
//
//	/* Configuración del transistor 1 */
//	handlerTransistor1.pGPIOx 												= GPIOC;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_10;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerTransistor1.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerTransistor1);
//
//	/* Configuración del transistor 2 */
//	handlerTransistor2.pGPIOx 												= GPIOC;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_12;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
//	handlerTransistor2.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
//	GPIO_Config(&handlerTransistor2);

	//Cargamos la configuración del EXTI

//	/* Interrupción del encoder */
//	ExtiClock.pGPIOHandler = &handlerCLK;
//	ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
//	extInt_Config(&ExtiClock);
//
//	/* Interrupción del botón */
//	ExtiButton.pGPIOHandler = &handlerButton;
//	ExtiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
//	extInt_Config(&ExtiButton);

	ExtiClock.pGPIOHandler = &handlerCLK;
	ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&ExtiClock);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

//void BasicTimer5_Callback(void){
//	GPIOxTooglePin(&handlerTransistor1);
//}

void callback_extInt13(void){
	encoderEdge = 1;  //Subiendo la bandera
	s++;
}

//void callback_extInt7(void){
//	buttonEdge = 1;
//	s++;
//}
