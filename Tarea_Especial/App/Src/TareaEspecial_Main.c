/*
 * Solución Tarea Especial
 *  Created on: May 22, 2023
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
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "PLLDriver.h"
#include "I2CDriver.h"

// Definición de los handlers necesarios
GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
GPIO_Handler_t	handlerMuestreoPin			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
BasicTimer_Handler_t handlerTimer_1KHz		= {0};
EXTI_Config_t ExtiButton 					= {0};

/*Elementos para hacer la comunicación serial*/
GPIO_Handler_t handlerPinTx					= {0};
GPIO_Handler_t handlerPinRx					= {0};
GPIO_Handler_t handlerPinMCO1				= {0};
USART_Handler_t usartComm 					= {0};

/* Elementos para el I2C */
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};

/* Elementos para el PWM*/
GPIO_Handler_t handlerPinPwmChannel1   = {0};
GPIO_Handler_t handlerPinPwmChannel2   = {0};
GPIO_Handler_t handlerPinPwmChannel3   = {0};
PWM_Handler_t handlerSignal1PWM        = {0};
PWM_Handler_t handlerSignal2PWM        = {0};
PWM_Handler_t handlerSignal3PWM        = {0};

uint16_t duttyValue = 1500;

/* Variables y arreglos */
char sendMsg = 0;
char mensaje[] = "Sara";
uint8_t rxData = 0;
uint8_t flag1KHz = 0;
uint8_t flag2seg = 0;
uint8_t contData = 0;
float factorConver = (4 * 9.78)/ (1000 * 256) ;

char bufferData[128] = "Accel testing...";
char bufferMsg[64] = {0};

float datoX = 0;
float datoY = 0;
float datoZ = 0;

uint64_t ejeXAccel[2000];
uint64_t ejeYAccel[2000];
uint64_t ejeZAccel[2000];


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
#define WHO_AM_I                 0


//Definición de las cabeceras de las funciones del main
void initSystem(void);
void dataPrintAccel(void);

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	/* Activamos el coprocesador matematico*/
	SCB->CPACR |= (0xF << 20);

	configPLL();

	writeMsg(&usartComm, bufferData);

	i2c_writeSingleRegister(&Accelerometer, BW_RATE, OUTPUT_RATE);


	while(1){

		//Prueba de Rx y Tx
//		if(sendMsg > 4){
//			writeChar(&usartComm, 'S');
//			writeMsg(&usartComm, mensaje);
//			writeCharTX(&usartComm, 'a');
//			writeMsgTX(&usartComm, mensaje);
//
//			sendMsg = 0;
//		}

		//Imprime información del acelerometro con las letras w,s,r,x,y,z,a.
		dataPrintAccel();

	}
	return 0;
}


//Función que imprime informacion del acelerometro dependiendo de algunas letras,
//muestreando a 1KHz
void dataPrintAccel(void){

	if(flag1KHz == 1){
		if(rxData != '\0'){
			writeChar(&usartComm, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usartComm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if (rxData == 's'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usartComm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&Accelerometer, POWER_CTL);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			if (rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&usartComm, bufferData);

				i2c_writeSingleRegister(&Accelerometer, POWER_CTL , 0x2D);
				rxData = '\0';
			}
			else if (rxData == 'x'){
				sprintf(bufferData, "Axis X data (r) \n");
				writeMsg(&usartComm, bufferData);

				uint8_t AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;
				float datoX = (float)AccelX * factorConver;

				sprintf(bufferData, "AccelX = %.2f m/s² \n", datoX);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usartComm, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);
				uint16_t AccelY = AccelY_high << 8 | AccelY_low;
				float datoY = ((float)AccelY - 57800)* factorConver ;

				sprintf(bufferData, "AccelY = %.2f m/s² \n", datoY);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&usartComm, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);
				uint16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				float datoZ = (float)AccelZ * factorConver;

				sprintf(bufferData, "AccelZ = %.2f m/s² \n", datoZ);
				writeMsg(&usartComm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'a'){
				writeMsg(&usartComm, "Datos 3 ejes \n" );
				for(int i = 0; i<2000; i++){
					 sprintf(bufferData, " dato %d; AccelX = %.3f ;   AccelY = %.3f;  AccelZ = %.3f  m/s² \n",i,((float)ejeXAccel[i]),((float)ejeYAccel[i]),((float)ejeZAccel[i]));
					 writeMsg(&usartComm, bufferData);
					 rxData = '\0';
				}
				flag2seg = 0;
			}
			else{
				rxData = '\0';
			}
		}
		flag1KHz = 0;

		if(contData < 2000){
			ejeXAccel[contData] = datoX;
			ejeYAccel[contData] = datoY;
			ejeZAccel[contData] = datoZ;
		}

	}
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
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_80MHz_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;	  //250 ms (Tiempo de subida)
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable 	= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración del TIM5 para el muestreo a 1KHz*/
	handlerTimer_1KHz.ptrTIMx 								= TIM5;
	handlerTimer_1KHz.TIMx_Config.TIMx_mode					= BTIMER_MODE_UP;
	handlerTimer_1KHz.TIMx_Config.TIMx_speed				= BTIMER_SPEED_80MHz_10us;
	handlerTimer_1KHz.TIMx_Config.TIMx_period				= 10;	  //1 ms (Tiempo de subida)
	handlerTimer_1KHz.TIMx_Config.TIMx_interruptEnable 		= BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerTimer_1KHz);

	/* Configuración del pin para el TIM5 */
	handlerMuestreoPin.pGPIOx 								= GPIOC;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_10;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerMuestreoPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerMuestreoPin);

	//El pin UserButton es una entrada simple que entregará la interrupción EXTI-13
	/* Configuración del UserButton */
	handlerUserButton.pGPIOx 								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber 		= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	ExtiButton.pGPIOHandler   = &handlerUserButton;
	ExtiButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&ExtiButton);

	/* Configuracion de la comunicación serial para el Usart */
	handlerPinTx.pGPIOx 								= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinTx);

	handlerPinRx.pGPIOx 								= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber 			= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
	GPIO_Config(&handlerPinRx);

	usartComm.ptrUSARTx									= USART1;
	usartComm.USART_Config.USART_baudrate				= USART_BAUDRATE_115200;
	usartComm.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	usartComm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usartComm.USART_Config.USART_mode					= USART_MODE_RXTX;
	usartComm.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	usartComm.USART_Config.USART_enableIntRX			= USART_RX_INTERRUPT_ENABLE;
	usartComm.USART_Config.USART_enableIntTX			= USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usartComm);

	/* Configuración MCO1 (probar señal) */
	handlerPinMCO1.pGPIOx								= GPIOA;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinOType			= GPIO_OTYPE_PUSHPULL;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPinMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;
	GPIO_Config(&handlerPinMCO1);

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
	Accelerometer.mainClock							 = MAIN_CLOCK_80_MHz_FOR_I2C;
	i2c_config(&Accelerometer);

	/* Configuramos el PWM para la señal 1*/
	handlerPinPwmChannel1.pGPIOx                             = GPIOC;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinNumber		 = PIN_6;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinMode		 = GPIO_MODE_ALTFN;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinOType		 = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinSpeed		 = GPIO_OSPEED_FAST;
	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinAltFunMode	 = AF2;
	GPIO_Config(&handlerPinPwmChannel1);

	handlerSignal1PWM.ptrTIMx								= TIM3;
	handlerSignal1PWM.config.channel						= PWM_CHANNEL_1;
	handlerSignal1PWM.config.duttyCicle						= duttyValue;
	handlerSignal1PWM.config.periodo						= 20000;
	handlerSignal1PWM.config.prescaler						= 16;
	pwm_Config(&handlerSignal1PWM);

	enableOutput(&handlerSignal1PWM);
	startPwmSignal(&handlerSignal1PWM);

	/* Configuramos el PWM para la señal 2*/
	handlerPinPwmChannel2.pGPIOx                             = GPIOC;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinNumber		 = PIN_7;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinMode		 = GPIO_MODE_ALTFN;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinOType		 = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinSpeed		 = GPIO_OSPEED_FAST;
	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinAltFunMode	 = AF2;
	GPIO_Config(&handlerPinPwmChannel2);

	handlerSignal2PWM.ptrTIMx								= TIM3;
	handlerSignal2PWM.config.channel						= PWM_CHANNEL_2;
	handlerSignal2PWM.config.duttyCicle						= duttyValue;
	handlerSignal2PWM.config.periodo						= 20000;
	handlerSignal2PWM.config.prescaler						= 16;
	pwm_Config(&handlerSignal2PWM);


	enableOutput(&handlerSignal2PWM);
	startPwmSignal(&handlerSignal2PWM);

	/* Configuramos el PWM para la señal 3*/
	handlerPinPwmChannel3.pGPIOx                             = GPIOC;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinNumber		 = PIN_8;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinMode		 = GPIO_MODE_ALTFN;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinOType		 = GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinSpeed		 = GPIO_OSPEED_FAST;
	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinAltFunMode	 = AF2;
	GPIO_Config(&handlerPinPwmChannel3);

	handlerSignal3PWM.ptrTIMx								= TIM3;
	handlerSignal3PWM.config.channel						= PWM_CHANNEL_3;
	handlerSignal3PWM.config.duttyCicle						= duttyValue;
	handlerSignal3PWM.config.periodo						= 20000;
	handlerSignal3PWM.config.prescaler						= 16;
	pwm_Config(&handlerSignal3PWM);


	enableOutput(&handlerSignal3PWM);
	startPwmSignal(&handlerSignal3PWM);

}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

void BasicTimer5_Callback(void){
	GPIOxTooglePin(&handlerMuestreoPin);

	if(contData < 2000){
		contData++;

		flag2seg = 1;
	}
	else{
		contData = 0;
	}

	flag1KHz = 1;
}

void callback_extInt13(void){
	__NOP();
}

void usart1Rx_Callback(void){

	rxData = getRxData();

//	//Prueba UsartRx por interrupción
//	sprintf(bufferMsg, "Recibido Char = %c \n" , rxData);
//	writeMsg(&usartComm, bufferMsg);
}

