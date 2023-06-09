/*
 * examen_Main.c
 *
 *  Created on: Apr 8, 2023
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
#include "RTCDriver.h"
#include "PwmDriver.h"

#include "arm_math.h"

//Acelerometro
#define ACCEL_ADDRESS          	 0b1010011
#define ACCEL_XOUT_H             50
#define ACCEL_XOUT_L             51
#define ACCEL_YOUT_H             52
#define ACCEL_YOUT_L             53
#define ACCEL_ZOUT_H             54
#define ACCEL_ZOUT_L             55

#define WHO_AM_I                 0
#define POWER_CTL                45
#define BW_RATE                  44
#define OUTPUT_RATE              14

#define ACCEL_DATA_SIZE         4096

// Definición de los handlers necesarios
GPIO_Handler_t 	handlerUserButton 			= {0};
GPIO_Handler_t 	handlerBlinkyPin 			= {0};
GPIO_Handler_t handlerPinMCO1				= {0};
GPIO_Handler_t	handlerMuestreoPin			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
BasicTimer_Handler_t handlerTimer_200Hz		= {0};
PLL_Config_t pll							= {0};
RTC_Config_t rtc							= {0};

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX     = {0};
GPIO_Handler_t handlerPinRX     = {0};
USART_Handler_t usartComm      = {0};
uint8_t rxData = 0;
char bufferReception[64] = {0};

/* Elementos para el ADC*/
ADC_Config_t AdcChannel = {0};
uint8_t AdcIsComplete = 0;
uint16_t adcCounter = 0;
uint8_t val = 1;
uint16_t dataAdcCH1[256] = {0};
uint16_t dataAdcCH2[256] = {0};
#define numberOfChannels	2

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

int32_t  accelXData[ACCEL_DATA_SIZE];
int32_t  accelYData[ACCEL_DATA_SIZE];
float32_t  accelZData[ACCEL_DATA_SIZE];
uint8_t  accelData[6];
float32_t transformedAccelZ[ACCEL_DATA_SIZE];
uint8_t  flagData = 0;
uint16_t i = 0;
uint8_t flagFFT = 0;

/* FFT */
float32_t fs = 8000.0;     //frecuencia de muestreo
float32_t f0 = 250.0;      //Frecuencia fundamental de la señal
float32_t dt = 0.0;        // Periodo de muestreo (1/fs)
float32_t stopTime = 1.0;
float32_t amplitud = 5;    //Amplitud de la señal generada
//float32_t sineSignal[SINE_DATA_SIZE];
//float32_t transformedSignal[SINE_DATA_SIZE];
float32_t *ptrSineSignal;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status = ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize = 1024;

/* Elementos para el I2C */
GPIO_Handler_t I2cSDA = {0};
GPIO_Handler_t I2cSCL = {0};
GPIO_Handler_t lcdSDA = {0};
GPIO_Handler_t lcdSCL = {0};
I2C_Handler_t Accelerometer = {0};
uint8_t i2cBuffer = {0};
uint8_t accelData[] = {0};

/* Elementos RTC*/
uint8_t* valTime;
uint8_t* valDate;

/* Elementos para el PWM*/
GPIO_Handler_t handlerPinPwmChannel   = {0};
PWM_Handler_t handlerSignalPWM        = {0};

/* Variables y arreglos*/
char cmd[64];
char userMsg[64];
unsigned int firstParameter;
unsigned int secondParameter;
unsigned int thirdParameter;
uint8_t counterReception = 0;
bool stringComplete = false;

//Definición de las cabeceras de las funciones del main
void initSystem(void);
void comandos(char *ptrBufferReception);
void dataADC(void);
//void dataAccel(void);

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	configPLL(&pll);

	/* Activamos el coprocesador matematico*/
	SCB->CPACR |= (0xF << 20);

	muestreoADC(9);

//	i2cBuffer = i2c_readSingleRegister(&Accelerometer, WHO_AM_I);
//	sprintf(userMsg, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//	writeMsg(&usartComm, userMsg);
//
//	statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, fftSize);
//	sprintf(userMsg, "Initialization... SUCCESS! \n");
//	writeMsg(&usartComm,userMsg);

	while(1){

		if(AdcIsComplete == 1){
			stopPwmSignal(&handlerSignalPWM);
			// Enviamos los datos por consola
			for(uint8_t i = 0; i<256 ; i++){
				sprintf(userMsg,"dato %u: %u , %u \n",i,dataAdcCH1[i],dataAdcCH2[i]);
				writeMsg(&usartComm, userMsg);
			}
			// Bajamos la bandera del ADC
			AdcIsComplete = 0;
		}

//		//Prueba transformada
//		if(flagFFT == 1 ){
//
//			stopTime = 0.0;
//			int i = 0;
//			int j = 0;
//
//			sprintf(userMsg, "FFT \n");
//			writeMsg(&usartComm, userMsg);
//
//			if(statusInitFFT == ARM_MATH_SUCCESS){
//				arm_rfft_fast_f32(&config_Rfft_fast_f32, accelZData, transformedAccelZ, ifftFlag);
//
//				for(i=1; i< fftSize; i++){
//					if(i % 2){
//						sprintf(userMsg, "%u ; %#.6f \n", j, transformedAccelZ[j]);
//						writeMsg(&usartComm, userMsg);
//						j++;
//					}
//				}
//
//			}
//			else {
//				writeMsg(&usartComm, "FFT not Initialized..,");
//			}
//			//Bajar bandera FFT
//			flagFFT = 0;
//		}


		//Creamos una cadena de caracteres con los datos que llegan
		//El caracter '*' indica el final de la cadena
		if(rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			if(rxData == '*'){
				stringComplete = true;

				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			rxData = '\0';
		}
		//Análisis de la cadena de datos obtenida
		if(stringComplete){
			comandos(bufferReception);
			stringComplete = false;
		}

	}

	return 0;

}

void comandos(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %u", cmd, &firstParameter, &secondParameter,&thirdParameter);

	//Este comando imprime una lista con los otros comandos que tiene el equipo
	if(strcmp(cmd,"help") == 0){

		writeMsg(&usartComm, "Help Menu CMDs: \n");
		writeMsg(&usartComm, "1) help           -- Imprime este menu \n");
		writeMsg(&usartComm, "2) selectClock    -- Selecciona la señal: HSI=0, LSE=1, PLL=3 \n");
		writeMsg(&usartComm, "3) selectPreescaler  --Selecciona preescaler: 2,3,4,5 \n");
		writeMsg(&usartComm, "4) readTime --Muestra la hora del sistema en formato 24hr (hora:min:seg)\n");
		writeMsg(&usartComm, "5) readDate -- Muestra la fecha del sistema (dia/mes/año) \n");
		writeMsg(&usartComm, "6) changeTime -- Cambia la hora del sistema (hora min) \n");
		writeMsg(&usartComm, "7) changeDate -- Cambia la fecha del sistema (dia mes año) \n");
		writeMsg(&usartComm, "8) changePeriod -- Cambia el periodo de la señal PWM. Rango: 8000-20000 Hz \n");
		writeMsg(&usartComm, "9) dataADC -- Muestra los datos de la conversión ADC de los dos canales \n");
	}
	//Permite elegir la señal en el MCO1
	else if(strcmp(cmd, "selectClock") == 0){
		if(firstParameter == 0){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected HSI \n");
		}else if(firstParameter == 1){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected LSE \n");
		}else if(firstParameter == 3){
			updateClock(&pll, firstParameter);
			writeMsg(&usartComm, "Selected PLL \n");
		}else{
			writeMsg(&usartComm, "Wrong selection \n");
		}
	}
	//Permite seleccionar el preescaler de la señal en el MCO1
	else if(strcmp(cmd, "selectPreescaler") == 0){
		if(firstParameter == 2){
			updatePreescaler(&pll, MCO1PRE_2);
			writeMsg(&usartComm, "División de 2 \n");
		}else if(firstParameter == 3){
			updatePreescaler(&pll, MCO1PRE_3);
			writeMsg(&usartComm, "División de 3 \n");
		}else if(firstParameter == 4){
			updatePreescaler(&pll, MCO1PRE_4 );
			writeMsg(&usartComm, "División de 4 \n");
		}else if(firstParameter == 5){
			updatePreescaler(&pll, MCO1PRE_5);
			writeMsg(&usartComm, "División de 5 \n");
		}else{
			writeMsg(&usartComm, "Parámetro erróneo \n");
		}
	}
	//Muestra la hora
	else if(strcmp(cmd, "readTime") == 0){
		valTime = readTime();
		writeMsg(&usartComm, "Hora actual ");
		sprintf(userMsg, "%d:%d:%d \n",valTime[0],valTime[1],valTime[2]);
		writeMsg(&usartComm,userMsg);
	}
	//Muestra la fecha
	else if(strcmp(cmd, "readDate") == 0){
		valDate = readDate();
		writeMsg(&usartComm, "Fecha actual ");
		sprintf(userMsg, "%d/%d/%d \n",valDate[0],valDate[1],valDate[2]);
		writeMsg(&usartComm,userMsg);
	}
	//Permite cambiar la hora
	else if(strcmp(cmd, "changeTime") == 0){
		if (firstParameter <=24 && secondParameter <60){
			changeTime(&rtc, firstParameter, secondParameter);
			writeMsg(&usartComm, "Hora cambiada \n");
		}else{
			writeMsg(&usartComm, "Parámetros erróneos \n");
		}
	}
	//Permite cambiar la hora
	else if(strcmp(cmd, "changeDate") == 0){
		if (firstParameter <=31 && secondParameter <=12 ){
			changeDate(&rtc, firstParameter, secondParameter, thirdParameter);
			writeMsg(&usartComm, "Fecha cambiada \n");
		}else{
			writeMsg(&usartComm, "Parámetros erróneos \n");
		}
	}
	//Recibe el periodo para cambiar la velocidad de muestreo de la conversión
	else if(strcmp(cmd, "changePeriod") == 0){
		if(firstParameter >= 50 && firstParameter <= 125){
			updatePeriod(&handlerSignalPWM, firstParameter);
			writeMsg(&usartComm, "Velocidad de muestreo de la conversión ADC cambiada");
		}
		else{
			writeMsg(&usartComm, "Parámetro erróneo");
		}
	}
	//Imprime los datos de los dos canales de la conversión ADC
	else if(strcmp(cmd, "dataADC") == 0){
		writeMsg(&usartComm, "Datos de la conversión ADC : dato : canal 1, canal 2 ");
		dataADC();
	}
	else{
		writeMsg(&usartComm, "Wrong CMD \n");
	}
}

void dataADC(void){


}

//Muestrea los datos a un 200Hz y los guarda en arreglos
//void dataAccel(void){
//
//	if(flag200Hz == 1){
//
//		uint8_t AccelX_low =  i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_L);
//		uint8_t AccelX_high = i2c_readSingleRegister(&Accelerometer, ACCEL_XOUT_H);
//		AccelX = AccelX_high << 8 | AccelX_low;
//
//		uint8_t AccelY_low = i2c_readSingleRegister(&Accelerometer, ACCEL_YOUT_L);
//		uint8_t AccelY_high = i2c_readSingleRegister(&Accelerometer,ACCEL_YOUT_H);
//		AccelY = AccelY_high << 8 | AccelY_low;
//
//		uint8_t AccelZ_low = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_L);
//		uint8_t AccelZ_high = i2c_readSingleRegister(&Accelerometer, ACCEL_ZOUT_H);
//		AccelZ = AccelZ_high << 8 | AccelZ_low;
//
//		flag200Hz = 0;
//	}
//
//	if(contData < 256){
//		ejeXAccel[contData] = AccelX;
//		ejeYAccel[contData] = AccelY;
//		ejeZAccel[contData] = AccelZ;
//	}
//}


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

	/* Configuración del TIM5 para el muestreo a 200Hz*/
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
	I2cSDA.GPIO_PinConfig.GPIO_PinNumber             = PIN_7;
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

	/* Configuración RTC */
	rtc.seconds			= 0;
	rtc.minutes			= 0;
	rtc.hour			= 0;
	rtc.day				= 5;
	rtc.month			= 1;
	rtc.year			= 23;
	RTC_config(&rtc);

	/* Configuración del ADC */
	AdcChannel.channels[0]                           = ADC_CHANNEL_0;
	AdcChannel.channels[1]                           = ADC_CHANNEL_1;
	AdcChannel.dataAlignment                         = ADC_ALIGNMENT_RIGHT;
	AdcChannel.resolution                            = ADC_RESOLUTION_12_BIT;
	AdcChannel.samplingPeriod                        = ADC_SAMPLING_PERIOD_84_CYCLES;
	AdcChannel.numeroDeCanales                       = 2;
	ADC_ConfigMultichannel(&AdcChannel);

	/* Configuramos el PWM*/
	handlerPinPwmChannel.pGPIOx                             = GPIOB;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode	= AF1;
	GPIO_Config(&handlerPinPwmChannel);

	/* Configurando el Timer para que genere la señal PWM*/
	handlerSignalPWM.ptrTIMx								= TIM4;
	handlerSignalPWM.config.channel							= PWM_CHANNEL_4;
	handlerSignalPWM.config.duttyCicle						= 20;
	handlerSignalPWM.config.periodo							= 50;
	handlerSignalPWM.config.prescaler						= 100;
	pwm_Config(&handlerSignalPWM);

	/* Activamos la señal */
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);


}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
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

void adcComplete_Callback(void){

	if(val == 1){

		dataAdcCH1[adcCounter] = getADC();
	}
	else{
		dataAdcCH2[adcCounter] = getADC();
		adcCounter++;

	}
	if(adcCounter == 256){
		AdcIsComplete = 1;
		adcCounter = 0;
	}
	val^=1;


}
