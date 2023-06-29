/*
 * mainProyecto.c
 *
 *  Created on: Jun 28, 2023
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
GPIO_Handler_t handlerDireccion = { 0 };
GPIO_Handler_t handlerBlinkyPin = { 0 };
GPIO_Handler_t pinInductivo = { 0 };
GPIO_Handler_t pinCapacitivo = { 0 };
GPIO_Handler_t pinRain = { 0 };
GPIO_Handler_t pinTrigger = { 0 };
GPIO_Handler_t pinEcho = { 0 };
BasicTimer_Handler_t handlerBlinkyTimer = { 0 };
BasicTimer_Handler_t motorTimer = { 0 };
BasicTimer_Handler_t UltrasonidoTimer = { 0 };
EXTI_Config_t extiCapacitivo = { 0 };
EXTI_Config_t extiUltrasonido = { 0 };

/* Elementos para el PWM*/
GPIO_Handler_t handlerPwmMotor = { 0 };
PWM_Handler_t signalPwmMotor = { 0 };
GPIO_Handler_t handlerPwmServo = { 0 };
PWM_Handler_t signalPwmServo = { 0 };

/* Configuracion para el I2C de la LCD*/
GPIO_Handler_t handlerI2cSDA2 = { 0 };
GPIO_Handler_t handlerI2cSCL2 = { 0 };
I2C_Handler_t handlerLCD = { 0 };

//Elementos para hacer la comunicación serial
GPIO_Handler_t handlerPinTX = { 0 };
GPIO_Handler_t handlerPinRX = { 0 };
USART_Handler_t usart2Comm = { 0 };
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;
char bufferMsg[64] = { 0 };

/* Variables y arreglos*/
char cmd[64];
char userMsg[64];
uint8_t contadorMotor = 0;
uint8_t flagServo = 0;
uint8_t flagPosicionInicial = 2;
uint8_t flagLCD_Metal = 0;
uint8_t flagLCD_NoMetal = 0;
uint8_t flagLCD_Humedo = 0;
uint8_t flagLCD_MetalHumedo = 0;
uint8_t flag3Sensores = 0;
uint8_t flagDisparo = 0;
uint8_t flagInterrupcion = 0;
int distancia = 0;
uint8_t sensorInductivo = 0;
uint8_t sensorHumedad = 0;
uint8_t direccion = 0;

uint16_t recibiendo = 0;
uint8_t tiempoP = 0;
uint8_t trigger = 0;

//Definición de las cabeceras de las funciones del main
void initSystem(void);
void tiempoUltrasonido(void);

int main(void) {

	//Inicializamos todos los elementos del sistema
	initSystem();

	while (1) {
		if (flagDisparo == 1) {
			tiempoUltrasonido();
			flagDisparo = 0;
		}
	}
	return 0;
}

void tiempoUltrasonido(void) {

	//Genera el disparo
	GPIO_WritePin(&pinTrigger, 1);
	delay_ms(1);
	GPIO_WritePin(&pinTrigger, 0);
//	trigger = 0;
//	recibiendo = 1;

	//Activo el timer
	startTimer(&UltrasonidoTimer);

//	trigger = 0;
//	recibiendo = 1;

}

//Timer blinky led de estado
void BasicTimer2_Callback(void) {
	flagDisparo = 1;
	GPIOxTooglePin(&handlerBlinkyPin);

}

void BasicTimer5_Callback(void) {
	if (recibiendo) {
		trigger++;
	}
}

void callback_extInt11(void) {

	stopTimer(&UltrasonidoTimer);
	tiempoP = (trigger / 2) * 2;
	recibiendo = 0;
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
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&handlerBlinkyTimer);

	/* Configuración del TIM5*/
	UltrasonidoTimer.ptrTIMx = TIM5;
	UltrasonidoTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	UltrasonidoTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1us;
	UltrasonidoTimer.TIMx_Config.TIMx_period = 2;
	UltrasonidoTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;
	BasicTimer_Config(&UltrasonidoTimer);

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

	//Sensor ultrasonido
	//Configuracion pin trigger
	pinTrigger.pGPIOx = GPIOC;
	pinTrigger.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	pinTrigger.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	pinTrigger.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinTrigger);

	//Configuracion pin echo
	pinEcho.pGPIOx = GPIOC;
	pinEcho.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	pinEcho.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	pinEcho.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinEcho);

	extiUltrasonido.pGPIOHandler = &pinEcho;
	extiUltrasonido.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiUltrasonido);

}

