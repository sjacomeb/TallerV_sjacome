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
#include "PwmDriver.h"
#include "SysTickDriver.h"
#include "LCD_Driver.h"

#include "arm_math.h"

#define PASOS_360_MOTOR		200
#define PASOS_270_MOTOR		150
#define PASOS_180_MOTOR		100
#define PASOS_90_MOTOR		50

#define LCD_ADDRESS			0x26

// Definición de los handlers necesarios
GPIO_Handler_t handlerDireccion = {0};
GPIO_Handler_t handlerBlinkyPin = {0};
GPIO_Handler_t pinInductivo = {0};
GPIO_Handler_t pinCapacitivo = {0};
GPIO_Handler_t pinRain = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t motorTimer = {0};
EXTI_Config_t extiCapacitivo = {0};

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm = {0};
uint8_t rxData = 0;
char bufferReception[64] = {0};

/* Elementos para el PWM*/
GPIO_Handler_t handlerPwmMotor = {0};
PWM_Handler_t signalPwmMotor = {0};
GPIO_Handler_t handlerPwmServo = {0};
PWM_Handler_t signalPwmServo = {0};

/* Configuracion para el I2C de la LCD*/
GPIO_Handler_t handlerI2cSDA2 = {0};
GPIO_Handler_t handlerI2cSCL2 = {0};
I2C_Handler_t handlerLCD = {0};
char bufferLCDx[64] = {0};
char bufferLCDy[64] = {0};
char bufferLCDz[64] = {0};

/* Variables y arreglos*/
char cmd[64];
char userMsg[64];
uint8_t contadorMotor = 0;
uint8_t flagServo = 0;
uint8_t flagPosicionInicial = 0;
uint8_t flagLCD_Metal = 0;
uint8_t flagLCD_NoMetal = 0;
uint8_t flagLCD_Humedo = 0;
uint8_t flagLCD_MetalHumedo = 0;
uint8_t flag3Sensores = 0;
uint8_t flagTerminado = 0;
uint8_t pasos = 0;
uint8_t sensorInductivo = 0;
uint8_t sensorHumedad = 0;


//Definición de las cabeceras de las funciones del main
void initSystem(void);
void mensajeInicio_LCD(void);

int main(void) {

	//Inicializamos todos los elementos del sistema
	initSystem();

	mensajeInicio_LCD();


	while (1) {


		if(flagServo == 1){

			delay_ms(1000);
			enableOutput(&signalPwmServo);
			startPwmSignal(&signalPwmServo);
			updateDuttyCycle(&signalPwmServo, 2400);
			delay_ms(2000);
			updateDuttyCycle(&signalPwmServo, 600);
			delay_ms(1000);

			flagServo= 0;
			startPwmSignal(&signalPwmMotor);
			flagPosicionInicial = 1;

		}

		if(flag3Sensores == 1){

			delay_ms(1000);
			enableOutput(&signalPwmServo);
			startPwmSignal(&signalPwmServo);
			updateDuttyCycle(&signalPwmServo, 2400);
			delay_ms(2000);
			updateDuttyCycle(&signalPwmServo, 600);
			delay_ms(1000);

			flag3Sensores= 0;
			flagTerminado = 1;

		}

		if(flagLCD_Metal == 1){

			delay_ms(10);
			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 2);
			lcd_i2c_putc(&handlerLCD, "Metal detectado");

			flagLCD_Metal = 0;
		}

		if(flagLCD_NoMetal == 1){

			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 1);
			lcd_i2c_putc(&handlerLCD, "No Metal detectado");

			flagLCD_NoMetal = 0;
		}

		if(flagLCD_MetalHumedo == 1){

			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 4);
			lcd_i2c_putc(&handlerLCD, "Metal Humedo");
			lcd_i2c_gotoxy(&handlerLCD, 2, 5);
			lcd_i2c_putc(&handlerLCD, "detectado");

			flagLCD_MetalHumedo = 0;
		}

		if(flagLCD_Humedo == 1){

			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 3);
			lcd_i2c_putc(&handlerLCD, "Residuo Humedo");
			lcd_i2c_gotoxy(&handlerLCD, 2, 5);
			lcd_i2c_putc(&handlerLCD, "detectado");

			flagLCD_Humedo = 0;
		}

		if(flagTerminado == 1){

			delay_ms(1000);
			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 0);
			lcd_i2c_putc(&handlerLCD, "Esperando residuo...");

			flagTerminado = 0;
		}

	}

	return 0;

}

void mensajeInicio_LCD(void){

	lcd_i2c_init(&handlerLCD);
	delay_ms(10);
	lcd_i2c_gotoxy(&handlerLCD, 1, 1);
	lcd_i2c_putc(&handlerLCD, "Iniciando programa");
	lcd_i2c_gotoxy(&handlerLCD, 2, 2);
	lcd_i2c_putc(&handlerLCD, "de clasificacion");

}

void initSystem(void) {

	config_SysTick_ms(0);

	/* Configuración del pin para el Led_Blinky */
	handlerBlinkyPin.pGPIOx = GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	/* Configuración del TIM2 para controlar el blinky */
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 250;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable =BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/*Configuración de la comunicación serial */
	handlerPinTX.pGPIOx = GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx = GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx = USART2;
	usart2Comm.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity = USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits = USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode = USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	usart2Comm.USART_Config.USART_enableIntTX = USART_TX_INTERRUPT_DISABLE;
	USART_Config(&usart2Comm);

	/* Configuración del sensor inductivo */
	pinInductivo.pGPIOx = GPIOC;
	pinInductivo.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	pinInductivo.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	pinInductivo.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinInductivo);

	/* Configuración del sensor capacitivo */
	pinCapacitivo.pGPIOx = GPIOC;
	pinCapacitivo.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	pinCapacitivo.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	pinCapacitivo.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinCapacitivo);

	extiCapacitivo.pGPIOHandler = &pinCapacitivo;
	extiCapacitivo.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiCapacitivo);

	/* Configuración del rain sensor */
	pinRain.pGPIOx = GPIOA;
	pinRain.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	pinRain.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	pinRain.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinRain);

	/* Configuramos el PWM Motor*/
	handlerPwmMotor.pGPIOx = GPIOB;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerPwmMotor);

	/* Configurando el Timer para que genere la señal PWM Motor */
	signalPwmMotor.ptrTIMx = TIM3;
	signalPwmMotor.config.channel = PWM_CHANNEL_2;
	signalPwmMotor.config.duttyCicle = 5000;
	signalPwmMotor.config.periodo = 10000;
	signalPwmMotor.config.prescaler = 16;
	pwm_Config(&signalPwmMotor);

	/* Configuración del TIM3 para controlar PWM motor */
	motorTimer.ptrTIMx = TIM3;
	motorTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	motorTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&motorTimer);

	/* Configuramos el PWM Servo */
	handlerPwmServo.pGPIOx = GPIOB;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPwmServo.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;
	GPIO_Config(&handlerPwmServo);

	/* Configurando el TIM4 para que genere la señal PWM Servo */
	signalPwmServo.ptrTIMx = TIM4;
	signalPwmServo.config.channel = PWM_CHANNEL_2;
	signalPwmServo.config.duttyCicle = 600;
	signalPwmServo.config.periodo = 20000;
	signalPwmServo.config.prescaler = 16;
	pwm_Config(&signalPwmServo);

	/* Configuración del pin dirección del motor */
	handlerDireccion.pGPIOx = GPIOC;
	handlerDireccion.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerDireccion.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerDireccion.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handlerDireccion.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerDireccion.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDireccion);

	/* Configurando los pines sobre los que funciona el I2C2*/
	handlerI2cSCL2.pGPIOx                            = GPIOB;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinNumber     = PIN_8;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinMode       = GPIO_MODE_ALTFN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinOType     = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_NOTHING;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinSpeed      = GPIO_OSPEED_HIGH;
	handlerI2cSCL2.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSCL2);

	handlerI2cSDA2.pGPIOx = GPIOB;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinNumber     = PIN_9;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinMode       = GPIO_MODE_ALTFN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinOType     = GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinSpeed      = GPIO_OSPEED_HIGH;
	handlerI2cSDA2.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSDA2);

	handlerLCD.ptrI2Cx = I2C1;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	i2c_config(&handlerLCD);
}

//Timer blinky led de estado
void BasicTimer2_Callback(void) {
	GPIOxTooglePin(&handlerBlinkyPin);
	sensorInductivo = GPIO_ReadPin(&pinInductivo);
	sensorHumedad = GPIO_ReadPin(&pinRain);

}

//Timer para el motor
void BasicTimer3_Callback(void) {


	if(flagPosicionInicial == 1){
		GPIO_WritePin(&handlerDireccion, 0);
		startPwmSignal(&signalPwmMotor);
		if (contadorMotor <= pasos) {
			contadorMotor++;
		}else {
			stopPwmSignal(&signalPwmMotor);
			contadorMotor = 0;
			flagPosicionInicial = 0;
			GPIO_WritePin(&handlerDireccion, 1);
			flagTerminado = 1;
		}
	}
	else{
		if (contadorMotor <= pasos) {
			contadorMotor++;
		}else{
			stopPwmSignal(&signalPwmMotor);
			contadorMotor = 0;
			flagServo = 1;
		}
	}

}

void BasicTimer4_Callback(void) {

}

//Interrupción USART
void usart2Rx_Callback(void) {

	rxData = getRxData();
}


//Interrupción sensor capacitivo
void callback_extInt11(void) {

	if(GPIO_ReadPin(&pinInductivo) == 0 && GPIO_ReadPin(&pinRain) == 0){

		flag3Sensores = 1;
		flagLCD_MetalHumedo = 1;

	}
	else if(GPIO_ReadPin(&pinInductivo) == 0){

		enableOutput(&signalPwmMotor);
		pasos = PASOS_180_MOTOR;
		startPwmSignal(&signalPwmMotor);
		flagLCD_Metal = 1;

	}
	else if(GPIO_ReadPin(&pinRain) == 0){

		enableOutput(&signalPwmMotor);
		pasos = PASOS_90_MOTOR;
		startPwmSignal(&signalPwmMotor);
		flagLCD_Humedo = 1;

	}
	else{

		enableOutput(&signalPwmMotor);
		pasos = PASOS_270_MOTOR;
		startPwmSignal(&signalPwmMotor);
		flagLCD_NoMetal = 1;
	}

}

