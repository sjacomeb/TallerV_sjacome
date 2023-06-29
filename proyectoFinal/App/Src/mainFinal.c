/*
 * mainFinal.c
 *
 *  Created on: Jun 29, 2023
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

#define PASOS_360_MOTOR		1600
#define PASOS_270_MOTOR		1200
#define PASOS_180_MOTOR		800
#define PASOS_90_MOTOR		400

#define LCD_ADDRESS			0x26

// Definición de los handlers necesarios
GPIO_Handler_t handlerDireccion = {0};
GPIO_Handler_t handlerBlinkyPin = {0};
GPIO_Handler_t pinInductivo = {0};
GPIO_Handler_t pinCapacitivo = {0};
GPIO_Handler_t pinRain = {0};
GPIO_Handler_t pinTrigger = {0};
GPIO_Handler_t pinEcho = {0};
BasicTimer_Handler_t handlerBlinkyTimer = {0};
BasicTimer_Handler_t motorTimer = {0};
BasicTimer_Handler_t cincoTimer = {0};
EXTI_Config_t extiCapacitivo = {0};
EXTI_Config_t extiUltrasonido = {0};

/* Elementos para el PWM*/
GPIO_Handler_t handlerPwmMotor = {0};
PWM_Handler_t signalPwmMotor = {0};
GPIO_Handler_t handlerPwmServo = {0};
PWM_Handler_t signalPwmServo = {0};

/* Configuracion para el I2C de la LCD*/
GPIO_Handler_t handlerI2cSDA2 = {0};
GPIO_Handler_t handlerI2cSCL2 = {0};
I2C_Handler_t handlerLCD = {0};

/* Variables y arreglos*/
uint16_t contadorMotor = 0;
uint8_t flagServo = 0;
uint8_t flagPosicionInicial = 2;
uint8_t flagLCD_Metal = 0;
uint8_t flagLCD_NoMetal = 0;
uint8_t flagLCD_Humedo = 0;
uint8_t flagLCD_MetalHumedo = 0;
uint8_t flag3Sensores = 0;
uint8_t flagTerminado = 0;
uint8_t flagInterrupcion = 0;
uint32_t pasos = 0;
uint8_t sensorInductivo = 0;
uint8_t sensorCapacitivo = 0;
uint8_t sensorHumedad = 0;
uint8_t direccion = 0;

//Definición de las cabeceras de las funciones del main
void initSystem(void);
void mensajeInicio_LCD(void);

int main(void) {

	//Inicializamos todos los elementos del sistema
	initSystem();
	mensajeInicio_LCD();

	while (1) {

		//Lectura de sensores
		if(flagInterrupcion == 1){

			delay_ms(2000);

			if(GPIO_ReadPin(&pinInductivo) == 0 && GPIO_ReadPin(&pinRain) == 0){

				flag3Sensores = 1;
				flagLCD_MetalHumedo = 1;

			}
			else if(GPIO_ReadPin(&pinInductivo) == 0){

				enableOutput(&signalPwmMotor);
				pasos = PASOS_180_MOTOR;
				startPwmSignal(&signalPwmMotor);
				flagPosicionInicial = 0;
				flagLCD_Metal = 1;

			}
			else if(GPIO_ReadPin(&pinRain) == 0){

				enableOutput(&signalPwmMotor);
				pasos = PASOS_90_MOTOR;
				startPwmSignal(&signalPwmMotor);
				flagPosicionInicial = 0;
				flagLCD_Humedo = 1;

			}
			else{

				enableOutput(&signalPwmMotor);
				pasos = PASOS_270_MOTOR;
				startPwmSignal(&signalPwmMotor);
				delay_ms(20);
				flagPosicionInicial = 0;
				flagLCD_NoMetal = 1;
			}
			flagInterrupcion = 0;
		}

		//Funcionamiento servo
		if(flagServo == 1){

			delay_ms(1000);
			enableOutput(&signalPwmServo);
			startPwmSignal(&signalPwmServo);
			updateDuttyCycle(&signalPwmServo, 2400);
			delay_ms(2000);
			updateDuttyCycle(&signalPwmServo, 600);
			delay_ms(1000);

			flagServo = 0;
			startPwmSignal(&signalPwmMotor);
			flagPosicionInicial = 1;

		}

		//Para los tres sensores
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

		//Mensajes
		if(flagTerminado == 1){
			delay_ms(1000);
			lcd_i2c_init(&handlerLCD);
			delay_ms(10);
			lcd_i2c_gotoxy(&handlerLCD, 1, 0);
			lcd_i2c_putc(&handlerLCD, "Esperando residuo...");
			delay_ms(500);

			flagTerminado = 0;
		}

		if(flagLCD_Metal == 1){
			delay_ms(20);
			lcd_i2c_init(&handlerLCD);
			lcd_i2c_gotoxy(&handlerLCD, 1, 2);
			lcd_i2c_putc(&handlerLCD, "Metal detectado");
			delay_ms(500);
			flagLCD_Metal = 0;

		}
		if(flagLCD_NoMetal == 1){
			delay_ms(20);
			lcd_i2c_init(&handlerLCD);
			lcd_i2c_gotoxy(&handlerLCD, 1, 1);
			lcd_i2c_putc(&handlerLCD, "No Metal detectado");
			delay_ms(500);
			flagLCD_NoMetal = 0;
		}
		if(flagLCD_MetalHumedo == 1){
			delay_ms(20);
			lcd_i2c_init(&handlerLCD);
			lcd_i2c_gotoxy(&handlerLCD, 1, 4);
			lcd_i2c_putc(&handlerLCD, "Metal Humedo");
			lcd_i2c_gotoxy(&handlerLCD, 2, 5);
			lcd_i2c_putc(&handlerLCD, "detectado");
			delay_ms(500);
			flagLCD_MetalHumedo = 0;
		}
		if(flagLCD_Humedo == 1){
			delay_ms(20);
			lcd_i2c_init(&handlerLCD);
			lcd_i2c_gotoxy(&handlerLCD, 1, 3);
			lcd_i2c_putc(&handlerLCD, "Residuo Humedo");
			lcd_i2c_gotoxy(&handlerLCD, 2, 5);
			lcd_i2c_putc(&handlerLCD, "detectado");
			delay_ms(500);
			flagLCD_Humedo = 0;
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


//Timer blinky led de estado
void BasicTimer2_Callback(void) {

	GPIOxTooglePin(&handlerBlinkyPin);
	sensorInductivo = GPIO_ReadPin(&pinInductivo);
	sensorHumedad = GPIO_ReadPin(&pinRain);
	sensorCapacitivo = GPIO_ReadPin(&pinCapacitivo);
	direccion = GPIO_ReadPin(&handlerDireccion);

}

//Timer para el motor
void BasicTimer3_Callback(void) {

	if(flagPosicionInicial == 1){

		GPIO_WritePin(&handlerDireccion,1);
		startPwmSignal(&signalPwmMotor);

		if (contadorMotor <= pasos) {
			contadorMotor++;
		}else {
			stopPwmSignal(&signalPwmMotor);
			contadorMotor = 0;
			flagPosicionInicial = 2;
			GPIO_WritePin(&handlerDireccion,0);
			flagTerminado = 1;
		}
	}
	else if(flagPosicionInicial == 0){

		if (contadorMotor <= pasos) {
			contadorMotor++;
		}else{
			stopPwmSignal(&signalPwmMotor);
			contadorMotor = 0;
			flagServo = 1;
			flagPosicionInicial = 2;
		}
	}

}

//Interrupción sensor capacitivo
void callback_extInt0(void) {

	flagInterrupcion = 1;

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

	/* Configuración del sensor inductivo */
	pinInductivo.pGPIOx = GPIOC;
	pinInductivo.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	pinInductivo.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	pinInductivo.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&pinInductivo);

	/* Configuración del sensor capacitivo */
	pinCapacitivo.pGPIOx = GPIOC;
	pinCapacitivo.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
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
	handlerPwmMotor.pGPIOx = GPIOC;
	handlerPwmMotor.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
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
	handlerDireccion.pGPIOx = GPIOB;
	handlerDireccion.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	handlerDireccion.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerDireccion.GPIO_PinConfig.GPIO_PinOType = GPIO_OTYPE_PUSHPULL;
	handlerDireccion.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerDireccion.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerDireccion);
	GPIO_WritePin(&handlerDireccion, 0);

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




