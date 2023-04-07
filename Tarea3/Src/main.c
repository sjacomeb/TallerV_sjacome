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
uint8_t t=0;

/* Punteros*/
uint8_t *pCounter = &counter;
uint8_t *pEncoderEdge = &encoderEdge;

/* Prototipos de funciones del main */
void init_Hardware(void);
void displayNumber(uint8_t number);
void contadorEncoder(uint8_t *pCounter, uint8_t *pEncoderEdge);
void callback_extInt13(void);
void BasicTimer2_Callback(void);

int main(void){

	/* Inicializacion de elementos*/
	init_Hardware();


	while(1){
		GPIO_WritePin(&handlerTransistor1, RESET);
		GPIO_WritePin(&handlerTransistor2, SET);

//		contadorEncoder(counter, encoderEdge);
//		displayNumber(counter);


	    if((GPIO_ReadPin(&handlerDT) == 0) && (encoderEdge == 1)){
	        if(counter <= 0){
	        	counter = 0;
	        } else {
	            (counter)--;
	        }
	        encoderEdge = 0;
	        displayNumber(counter);

	    } else if((GPIO_ReadPin(&handlerDT) == 1) && (encoderEdge == 1)){
	        if(counter > 9){
	        	counter = 9;
	        } else {
	            (counter)++;
	        }
	        encoderEdge = 0;
	        displayNumber(counter);
	    }



//		if(counter>9){
//			displayNumber(counter%10);
//			GPIO_WritePin(&handlerTransistor1, RESET); // Transistor encendido
//			GPIO_WritePin(&handlerTransistor2, SET);   //Transistor apagado
//		}else{
//			displayNumber((counter - (counter%10))/10);
//			GPIO_WritePin(&handlerTransistor2, RESET);
//			GPIO_WritePin(&handlerTransistor1, SET);
//		}
//		t = GPIO_ReadPin(&handlerTransistor1);
//		GPIO_WritePin(&handlerTransistor2, !t);
	}

	return 0;
}

/*Esta función que recibe la información del encoder y hace el conteo de 0 a 99 */

void contadorEncoder(uint8_t *pCounter, uint8_t *pEncoderEdge){

    if((GPIO_ReadPin(&handlerDT) == 0) && (*pEncoderEdge == 1)){
        if(*pCounter <= 0){
            *pCounter = 0;
        } else {
            (*pCounter)--;
        }
        *pEncoderEdge = 0;

    } else if((GPIO_ReadPin(&handlerDT) == 1) && (*pEncoderEdge == 1)){
        if(*pCounter > 9){
            *pCounter = 9;
        } else {
            (*pCounter)++;
        }
        *pEncoderEdge = 0;
    }
}

/*Esta funcion es encargada de configurar los pines GPIO correspondiente a cada segmento en el display
 * para mostrar el número solicitado*/
void displayNumber (uint8_t number){
	switch(number){
	case 0:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 1:
		GPIO_WritePin(&handlerPinDisplay_a, SET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 2:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, SET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 3:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 4:
		GPIO_WritePin(&handlerPinDisplay_a, SET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 5:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, SET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 6:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, SET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 7:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, SET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, SET);
		GPIO_WritePin(&handlerPinDisplay_g, SET);
		break;
	case 8:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, RESET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	case 9:
		GPIO_WritePin(&handlerPinDisplay_a, RESET);
		GPIO_WritePin(&handlerPinDisplay_b, RESET);
		GPIO_WritePin(&handlerPinDisplay_c, RESET);
		GPIO_WritePin(&handlerPinDisplay_d, RESET);
		GPIO_WritePin(&handlerPinDisplay_e, SET);
		GPIO_WritePin(&handlerPinDisplay_f, RESET);
		GPIO_WritePin(&handlerPinDisplay_g, RESET);
		break;
	default:{
		break;
	}
	}
}

/*Esta función se encarga de mostrar las unidades y decenas en los display 7 segmentos respectivamente
 * según el valor de la variable counter*/
//void ShowNumber(void){
//	if(GPIO_ReadPin(&handlerTransistor2) == 0){
//		displayNumber(counter/10);
//	}
//	else {
//		displayNumber(counter%10);
//	}
//}

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

	/* Configuración del TIM5 que controla el Display */
	handlerTimerDisplay.ptrTIMx 							= TIM5;
	handlerTimerDisplay.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerTimerDisplay.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_1ms;
	handlerTimerDisplay.TIMx_Config.TIMx_period 			= 250;                   //Periodo de 10ms
	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerTimerDisplay);

	/* Configuración del CLK LISTO */
	handlerCLK.pGPIOx = GPIOC;
	handlerCLK.GPIO_PinConfig.GPIO_PinNumber 	      = PIN_13;
	handlerCLK.GPIO_PinConfig.GPIO_PinMode 	          = GPIO_MODE_IN;
	handlerCLK.GPIO_PinConfig.GPIO_PinOType	          = GPIO_OTYPE_PUSHPULL;
	handlerCLK.GPIO_PinConfig.GPIO_PinSpeed	          = GPIO_OSPEED_FAST;
	handlerCLK.GPIO_PinConfig.GPIO_PinPuPdControl     = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerCLK);

	/* Configuración del DT LISTO */
	handlerDT.pGPIOx = GPIOB;
	handlerDT.GPIO_PinConfig.GPIO_PinNumber 	      = PIN_8;
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
	/* Configuración del segmento "a" del display LISTO */
	handlerPinDisplay_a.pGPIOx 												= GPIOA;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_0;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_a.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_a);

	/* Configuración del  segmento "b" del display LISTO */
	handlerPinDisplay_b.pGPIOx 												= GPIOB;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_7;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_b.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_b);

	/* Configuración del segmento "c" del display LISTO */
	handlerPinDisplay_c.pGPIOx 												= GPIOB;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_6;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_c.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_c);

	/* Configuración del segmento "d" del display LISTO */
	handlerPinDisplay_d.pGPIOx 												= GPIOB;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_0;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_FAST;
	handlerPinDisplay_d.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_d);

	/* Configuración del segmento "e" del display LISTO */
	handlerPinDisplay_e.pGPIOx 												= GPIOC;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_9;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_e.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_e);

	/* Configuración del segmento "f" del display LISTO */
	handlerPinDisplay_f.pGPIOx 												= GPIOA;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_1;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_f.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_f);

	/* Configuración del segmento "g" del display LISTO */
	handlerPinDisplay_g.pGPIOx 												= GPIOC;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_8;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerPinDisplay_g.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerPinDisplay_g);

	/* Configuración del transistor 1 */
	handlerTransistor1.pGPIOx 												= GPIOC;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_10;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerTransistor1.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerTransistor1);

	/* Configuración del transistor 2 */
	handlerTransistor2.pGPIOx 												= GPIOC;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinNumber			 			= PIN_12;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinMode 							= GPIO_MODE_OUT;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinOType 						= GPIO_OTYPE_PUSHPULL;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinPuPdControl 					= GPIO_PUPDR_NOTHING;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEED_MEDIUM;
	handlerTransistor2.GPIO_PinConfig.GPIO_PinAltFunMode					= AF0;
	GPIO_Config(&handlerTransistor2);

	//Cargamos la configuración del EXTI

	/* Interrupción del encoder */
	ExtiClock.pGPIOHandler = &handlerCLK;
	ExtiClock.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&ExtiClock);
//
//	/* Interrupción del botón */
//	ExtiButton.pGPIOHandler = &handlerButton;
//	ExtiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
//	extInt_Config(&ExtiButton);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLED2);
}

//void BasicTimer5_Callback(void){
//	GPIOxTooglePin(&handlerTransistor1);
//}

void callback_extInt13(void){
	encoderEdge = 1;        //Subiendo la bandera
	s++;
}

//void callback_extInt7(void){
//	buttonEdge = 1;
//	s++;
//}
