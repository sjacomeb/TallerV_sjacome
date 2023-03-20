/*
 * GPIOxDriverMain.c
 *
 *  Created on: Mar 10, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include "GPIOxDriver.h"

/* +++++++++   Primer punto    +++++++++ */

/* a)La función GPIO_ReadPin() lee el valor contenido en el registro IDR del GPIOx y le realiza un shift a la derecha tantas veces
 * como la ubicación del PIN específico, por ejemplo, si se tiene el PIN4 la función le hace al valor del registro un shift
 * a la derecha de 4 posiciones. Es por esto que el resultado que retorna la función no es el valor real del PIN ya que se tienen bits
 * a la izquierda de este valor. */


/* b) Para solucionar este problema hay que aplicarle una máscara donde se conserve el valor real del PIN y se conviertan en "0"
 * los bits a la izquierda de este valor. Para esto se utiliza una operación AND con un uno en la posición del bit menos significativo
 * y ceros en los bits restantes, ya que el uno en esta operación conserva el valor y el cero vuelve el valor cero.*/

/* c) La función fue corregida en el archivo GPIOxDriver.c */

/* +++++++++ Segundo punto +++++++++++ */

/* La función GPIOxTooglePin va a cambiar el estado de un PIN específico, para esto seleccionamos un pin en el handler, el cual debe estar configurado
 * como una salida es por esto que lo hacemos sobre el registro ODR el cual nos permite configurar el pin como una salida. Luego se crea una máscara con
 * un 0b1 (SET) desplazado a la izquierda tantas veces el númeto del PIN específico, ubicandonos así en la posición del PIN que queremos cambiar. Finalmente
 * se aplica la operación XOR al valor que cargamos del registro ODR con la máscara desplazada, logrando cambiar únicamente el estado del PIN específico.
 * Utilizamos la operación XOR porque este conserva el valor cuando se aplica con cero y cambia el valor cuando se aplica un uno.
 */

/* Función principal del programa */
int main (void){

//Definimos el handler para el PIN que deseamos configurar
	GPIO_Handler_t handlerUserLedPin = {0};

	//Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber       = PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

	//Cargamos a la configuración del PIN específico
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);

	/* ++++++++++++++++ Tercer punto +++++++++++++++++++ */
	/* Hay que hacer la configuración de cada pin */

	//PC9
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPC9 = {0};

			//Deseamos trabajar con el puerto GPIOC
			handlerLedPC9.pGPIOx = GPIOC;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinNumber       = PIN_9;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPC9.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_C9
				GPIO_Config(&handlerLedPC9);

	//PC6
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPC6 = {0};

			//Deseamos trabajar con el puerto GPIOC
			handlerLedPC6.pGPIOx = GPIOC;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPC6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_C6
				GPIO_Config(&handlerLedPC6);

	//PB8
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPB8 = {0};

			//Deseamos trabajar con el puerto GPIOB
			handlerLedPB8.pGPIOx = GPIOB;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPB8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_B8
				GPIO_Config(&handlerLedPB8);

	//PA6
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPA6 = {0};

			//Deseamos trabajar con el puerto GPIOA
			handlerLedPA6.pGPIOx = GPIOA;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinNumber       = PIN_6;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPA6.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_A6
				GPIO_Config(&handlerLedPA6);

	//PC7
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPC7 = {0};

			//Deseamos trabajar con el puerto GPIOC
			handlerLedPC7.pGPIOx = GPIOC;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPC7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_C7
				GPIO_Config(&handlerLedPC7);

	//PC8
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPC8 = {0};

			//Deseamos trabajar con el puerto GPIOC
			handlerLedPC8.pGPIOx = GPIOC;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinNumber       = PIN_8;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPC8.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_C8
				GPIO_Config(&handlerLedPC8);

	//PA7
	//Definimos el handler para el PIN que deseamos configurar
		GPIO_Handler_t handlerLedPA7 = {0};

			//Deseamos trabajar con el puerto GPIOA
			handlerLedPA7.pGPIOx = GPIOA;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinNumber       = PIN_7;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_OUT;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerLedPA7.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_A7
				GPIO_Config(&handlerLedPA7);

	//Configuración del User_Button
		GPIO_Handler_t handlerUserButton = {0};

			handlerUserButton.pGPIOx = GPIOC;
			handlerUserButton.GPIO_PinConfig.GPIO_PinNumber       = PIN_13;
			handlerUserButton.GPIO_PinConfig.GPIO_PinMode         = GPIO_MODE_IN;
			handlerUserButton.GPIO_PinConfig.GPIO_PinOType        = GPIO_OTYPE_PUSHPULL;
			handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl  = GPIO_PUPDR_NOTHING;
			handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed        = GPIO_OSPEED_MEDIUM;
			handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode   = AF0;        //Ninguna Función

			//Cargamos a la configuración del PIN_A6
				GPIO_Config(&handlerUserButton);

	//Creamos un contador
	uint8_t cont = 1;

	//Inicializamos las variables que almacenan el estado del pinX

	uint8_t valueP0 = 0;
	uint8_t valueP1  = 0;
	uint8_t valueP2 = 0;
	uint8_t valueP3 = 0;
	uint8_t valueP4 = 0;
	uint8_t valueP5 = 0;
	uint8_t valueP6  = 0;


//Este es el ciclo principal, donde se ejecuta todo el programa

	while(1){

		//Prueba del primer y segundo punto
		//		GPIO_ReadPin(&handlerUserLedPin);       //Leemos el valor del PIN_A5
		//		GPIOxTooglePin(&handlerUserLedPin);     //Cambiamos el estado del LED2

		//Probando el segundo punto con un delay
		/* Delay para probar el estado del PIN_A5 cada cierto tiempo
		* for(int j=0; j<1200000; j++){
		*  } */


		//Punto tres

		if(GPIO_ReadPin(&handlerUserButton) == SET){
			if(cont>60){
				cont=1;
			}

			valueP0 = ((cont>>0) & 0b1);
			valueP1 = ((cont>>1) & 0b1);
			valueP2 = ((cont>>2) & 0b1);
			valueP3 = ((cont>>3) & 0b1);
			valueP4 = ((cont>>4) & 0b1);
			valueP5 = ((cont>>5) & 0b1);
			valueP6 = ((cont>>6) & 0b1);

			GPIO_WritePin(&handlerLedPA7, valueP0);
			GPIO_WritePin(&handlerLedPC8,valueP1);
			GPIO_WritePin(&handlerLedPC7,valueP2);
			GPIO_WritePin(&handlerLedPA6, valueP3);
			GPIO_WritePin(&handlerLedPB8, valueP4);
			GPIO_WritePin(&handlerLedPC6,valueP5);
			GPIO_WritePin(&handlerLedPC9,valueP6);

			cont++;

			for(uint32_t i=0; i<2; i++){
						__NOP();
					}

		}else{
			if(GPIO_ReadPin(&handlerUserButton) == RESET){
				if(cont<1){

					cont = 60;
				}
				valueP0 = ((cont>>0) & 0b1);
				valueP1 = ((cont>>1) & 0b1);
				valueP2 = ((cont>>2) & 0b1);
				valueP3 = ((cont>>3) & 0b1);
				valueP4 = ((cont>>4) & 0b1);
				valueP5 = ((cont>>5) & 0b1);
				valueP6 = ((cont>>6) & 0b1);

				GPIO_WritePin(&handlerLedPA7, valueP0);
				GPIO_WritePin(&handlerLedPC8,valueP1);
				GPIO_WritePin(&handlerLedPC7,valueP2);
				GPIO_WritePin(&handlerLedPA6, valueP3);
				GPIO_WritePin(&handlerLedPB8, valueP4);
				GPIO_WritePin(&handlerLedPC6,valueP5);
				GPIO_WritePin(&handlerLedPC9,valueP6);


				for(uint32_t i=0; i<=2; i++){
							__NOP();
						}

			}
		}
	}
}














