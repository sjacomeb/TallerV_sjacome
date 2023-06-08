/*
 * examenMain.c
 *
 *  Created on: Jun 7, 2023
 *      Author: sjacome
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "stm32f4xx.h"

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "PLLDriver.h"
#include "AdcDriver.h"
#include "I2CDriver.h"

#include "arm_math.h"

// Definición de los handlers necesarios

GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
GPIO_Handler_t handlerPinMCO1				= {0};
GPIO_Handler_t	handlerMuestreoPin			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
BasicTimer_Handler_t handlerTimer_200Hz		= {0};
PLL_Config_t pll							= {0};

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX     = {0};
GPIO_Handler_t handlerPinRX     = {0};
USART_Handler_t usartComm      = {0};
uint8_t sendMsg = 0;
uint8_t rxData = 0;
char bufferReception[64] = {0};

/* Elementos para el ADC*/
ADC_Config_t channel_1				= {0};
ADC_Config_t channel_2				= {0};

/* Elementos Acelerometro */
uint8_t flag200Hz = 0;
uint8_t flag2seg = 0;
uint16_t contData = 0;
int16_t AccelX = 0 ;
int16_t AccelY = 0 ;
int16_t AccelZ = 0;
uint64_t ejeXAccel[256];
uint64_t ejeYAccel[256];
uint64_t ejeZAccel[256];

/* Elementos para el I2C */
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
GPIO_Handler_t lcdSDA = {0};
GPIO_Handler_t lcdSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};
uint8_t accelData[] = {0};

/* Variables y arreglos*/
char cmd[64];
char userMsg[64];
char bufferData[64] = {0};
unsigned int firstParameter;
unsigned int secondParameter;
uint8_t counterReception = 0;
bool stringComplete = false;

//Acelerometro
#define ACCEL_ADDRESS          	 0b1010011
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55
#define POWER_CTL                45
#define BW_RATE                  44
#define OUTPUT_RATE              14

//Definición de las cabeceras de las funciones del main
void initSystem(void);
void dataAccel(void);

int main(void){

	//Inicializamos todos los elementos del sistema

	initSystem();

	/* Activamos el coprocesador matematico*/
	SCB->CPACR |= (0xF << 20);

	configPLL(&pll);

	writeMsg(&usartComm, "Funciona");

	i2cBuffer = i2c_readSingleRegister(&Accelerometer,ACCEL_ADDRESS );
	sprintf(bufferData, "data = 0x%x \n", (unsigned int)i2cBuffer);
	writeMsg(&usartComm, bufferData);

	while(1){


		if(flag200Hz == 1){

			i2c_readMultiRegister(&Accelerometer, ACCEL_XOUT_H, 6, accelData);
				AccelX = accelData[0] << 8 | accelData[1];
				AccelY = accelData[2] << 8 | accelData[3];
				AccelZ = accelData[4] << 8 | accelData[5];

				flag200Hz = 0;
		}

		if(contData < 256){
			ejeXAccel[contData] = AccelX;
			ejeYAccel[contData] = AccelY;
			ejeZAccel[contData] = AccelZ;
		}

//		if(flag200Hz == 1){
//
//			uint8_t AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
//			uint8_t AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);
//			AccelX = AccelX_high << 8 | AccelX_low;
//
//			uint8_t AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
//			uint8_t AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);
//			AccelY = AccelY_high << 8 | AccelY_low;
//
//
//			uint8_t AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
//			uint8_t AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);
//			AccelZ = AccelZ_high << 8 | AccelZ_low;
//
//			flag200Hz = 0;
//		}
//
//		if(contData < 256){
//			ejeXAccel[contData] = AccelX;
//			ejeYAccel[contData] = AccelY;
//			ejeZAccel[contData] = AccelZ;
//		}


		if(rxData != '\0'){
			writeChar(&usartComm, rxData);

			if (rxData == 'x'){
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&usartComm, bufferData);

				sprintf(bufferData, "AccelX = %d \n", AccelX);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usartComm, bufferData);

				sprintf(bufferData, "AccelY = %d \n", AccelY);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&usartComm, bufferData);

				sprintf(bufferData, "AccelZ = %d \n", AccelZ);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'a'){
				writeMsg(&usartComm, "Datos ejes Acelerometro \n" );
				for(uint8_t j = 0; j<256; j++){
					 sprintf(bufferData, " dato %d ; AccelX = %.2f ;   AccelY = %.2f;  AccelZ = %.2f \n",j,((float)ejeXAccel[j]),((float)ejeYAccel[j]),((float)ejeZAccel[j]));
					 writeMsg(&usartComm, bufferData);
					 rxData = '\0';
				}
				flag2seg = 0;
			}
			else{
				rxData = '\0';
			}
		}

	}

	return 0;

}

void initSystem(void){

	/* Configuración del pin para el Led_Blinky */
	handlerBlinkyPin.pGPIOx 								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	/* Configuración del TIM2 para controlar el blinky */
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100MHz_10us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración del TIM5 para el muestreo a 1KHz*/
	handlerTimer_200Hz.ptrTIMx 								= TIM5;
	handlerTimer_200Hz.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimer_200Hz.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100MHz_10us;
	handlerTimer_200Hz.TIMx_Config.TIMx_period				= 50;	  //5 ms (Tiempo de subida)
	handlerTimer_200Hz.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerTimer_200Hz);

	/* Configuración del pin para el TIM5 */
	handlerMuestreoPin.pGPIOx 								= GPIOC;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_10;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerMuestreoPin);

	/* Configuración MCO1 (probar señal) */
	handlerPinMCO1.pGPIOx								= GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;
	GPIO_Config(&handlerPinMCO1);

	/*Configuración de la comunicación serial usart1 */
	handlerPinTX.pGPIOx 								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx 								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinRX);

	usartComm.ptrUSARTx										= USART1;
	usartComm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usartComm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usartComm.USART_Config.USART_parity						= USART_PARITY_NONE;
	usartComm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usartComm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usartComm.USART_Config.USART_enableIntRX       			= USART_RX_INTERRUPT_ENABLE;
	usartComm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usartComm);

	//Configuración I2C
	I2cSCL.pGPIOx                                    = GPIOB;
	I2cSCL.GPIO_PinConfig.GPIO_PinNumber             = PIN_8;
	I2cSCL.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSCL.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSCL.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSCL);

	I2cSDA.pGPIOx                                    = GPIOB;
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;
	I2cSDA.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;
	I2cSDA.GPIO_PinConfig.GPIO_PinOType              = GPIO_OTYPE_OPENDRAIN;
	I2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	I2cSDA.GPIO_PinConfig.GPIO_PinSpeed              = GPIO_OSPEED_FAST;
	I2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode         = AF4;
	GPIO_Config(&I2cSDA);

	Accelerometer.ptrI2Cx                            = I2C1;
	Accelerometer.modeI2C                            = I2C_MODE_FM;
	Accelerometer.slaveAddress                       = ACCEL_ADDRESS;
	Accelerometer.mainClock							 = MAIN_CLOCK_100_MHz_FOR_I2C;
	i2c_config(&Accelerometer);

	/* Configuración PLL */
	pll.PLLN 			= 100;
	pll.PLLM			= 8;
	pll.PLLP			= PLLP_2;
	pll.MC01PRE			= MCO1PRE_2;

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

//Timer para el muestreo Acelerometro
void BasicTimer5_Callback(void){
	GPIOxTooglePin(&handlerMuestreoPin);

	if(contData < 256){
		contData++;

		flag2seg = 1;
	}
	else{
		contData = 0;
	}

	flag200Hz = 1;
}

void usart1Rx_Callback(void){

	rxData = getRxData();
}


