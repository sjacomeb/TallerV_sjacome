/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 3, 2023
 *      Author: sjacome
 *
 * Información en este archivo:
 * -Valores del reloj principal
 * -Distribución básica de la memoria (Fig 14, datasheet del micro)
 * -Posiciones de memoria de los periféricos disponibles en el micro (tabla 1, Memory Map)
 * -Incluye los demás registros de los periféricos
 * -Definiciones de las constantes básicas
 */

#ifndef STM32F411XX_HAL_H_
#define STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED   16000000    //Value for the main clock signal (High Speed Internal)
#define HSE_CLOCK_SPEED   4000000     //Value for the main clock signal (High Speed External)

#define NOP()   asm("NOP")
#define _weak     __attribute__(weak)

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory Map, Figure 14
 *(Remember, 1KByte = 1024 bytes
 */

#define FLASH_BASE_ADDE                 0X08000000U     //Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR                  0X20000000U     //Esta es la memoria RAM, 128KB

/*NOTA: Observar que existen unos registros específicos del Cortes M4 en la región 0xE0000000U
 * Los controladores de las interrupciones se encuentran allí, por ejemplo.
 */

/*NOTA:
 * Ahora agregamos la dirección de memoria base para cada uno de los periféricos que posee el micro
 * En el datasheet del micro, fig 14, encontramos el mapa de los buses:
 * -APB1 (Advance Peripheral Bus)
 * -APB2
 * -AHB1 (Advance High-performance Bus)
 * -AHB2
 */

/**
 * AHBx and APBx Bus Peripherals base addresses
 */

#define APB1_BASE_ADDR          0x40000000U
#define APB2_BASE_ADDR          0x40010000U
#define AHB1_BASE_ADDR          0x40020000U
#define AHB2_BASE_ADDR          0x50000000U

/**
 * Y ahora debemos hacer lo mismo para cada una de las posiciones de memoria de cada uno de los periféricos
 * descritos en la tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla está a su vez dividida en cuatro segmentos, cada uno correspondiente a
 * APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 *
 * NOTA: Solo lo va a hacer con los elementos que necesitamos en el ejercicio: RCC y GPIOx.
 */

/* Posiciones de memoria para perféricos del AHB2 */
#define USB_OTG_FS_BASE_ADDR        (AHB2_BASE_ADDR + 0X0000U)

/*Posiciones de memoria de los periféricos del AHB1
 * Observar que NO está completa*/

#define RCC_BASE_ADDR                  (AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR                (AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR                (AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR                (AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR                (AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR                (AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR                (AHB1_BASE_ADDR + 0x0000U)

/**
 *Macros Genéricos
 */
#define ENABLE                 1
#define DISABLE                0
#define SET                    ENABLE
#define CLEAR                  DISABLE
#define RESET                  DISABLE
#define GPIO_PIN_SET           SET
#define GPIO_PIN_RESET         RESET
#define FLAG_SET               SET
#define FLAG_RESET             RESET
#define I2C_WRITE              0
#define I2C_READ               1

/* +++INICIO de la descripción de los elementos del periférico+++ */

/*Definición de la estrcutura de datos que representa a cada uno de los registros del periférico RCC.
 *
 * NOTA: La posición de memoria (offset) debe encajar perfectamente con la posición de memoria indicada en la
 * hoja de datos del equipo.Observe que los elementos "reservedx" también están presentes allí.
 */

typedef struct
{
	volatile uint32_t CR;            //Clock Control Register                  ADDR_OFFSET:  0x00
	volatile uint32_t PLLCFGR;       //PLL Configuration Register              ADDR_OFFSET:  0x04
	volatile uint32_t CFGR;         //Clock Control Register                   ADDR_OFFSET:  0x08
	volatile uint32_t CIR;          //Clock Interrupt Register                 ADDR_OFFSET:  0x0C
	volatile uint32_t AHB1RSTR;     //AHB1 Pheripheral Reset Register          ADDR_OFFSET:  0x10
	volatile uint32_t AHB2RSTR;     //AHB2 Pheripheral Reset Register          ADDR_OFFSET:  0x14
	volatile uint32_t reserved0;    //reserved                                 ADDR_OFFSET:  0x18
	volatile uint32_t reserved1;    //reserved                                 ADDR_OFFSET:  0x1C
	volatile uint32_t APB1RSTR;     //APB1 Peripheral Reset Register           ADDR_OFFSET:  0x20
	volatile uint32_t APB2RSTR;     //APB2 Peripheral Reset Register           ADDR_OFFSET:  0x24
	volatile uint32_t reserved2;    //reserved                                 ADDR_OFFSET:  0x28
	volatile uint32_t reserved3;    //reserved                                 ADDR_OFFSET:  0x2C
	volatile uint32_t AHB1ENR;      //AHB1 Peripheral Clock Enable Register    ADDR_OFFSET:  0x30
	volatile uint32_t AHB2ENR;      //AHB2 Peripheral Clock Enable Register    ADDR_OFFSET:  0x34
	volatile uint32_t reserved4;    //reserved                                 ADDR_OFFSET:  0x38
	volatile uint32_t reserved5;    //reserved                                 ADDR_OFFSET:  0x3C
	volatile uint32_t APB1ENR;      //APB1 Peripheral Clock Enable Register    ADDR_OFFSET:  0x40
	volatile uint32_t APB2ENR;      //APB2 Peripheral Clock Enable Register    ADDR_OFFSET:  0x44
	volatile uint32_t reserved6;    //reserved                                 ADDR_OFFSET:  0x48
	volatile uint32_t reserved7;    //reserved                                 ADDR_OFFSET:  0x4C
	volatile uint32_t AHB1LPENR;    //AHB1 Clock Enable Low Power Register     ADDR_OFFSET:  0x50
	volatile uint32_t AHB2LPENR;    //AHB2 Clock Enable Low Power Register     ADDR_OFFSET:  0x54
	volatile uint32_t reserved8;    //reserved                                 ADDR_OFFSET:  0x58
	volatile uint32_t reserved9;    //reserved                                 ADDR_OFFSET:  0x5C
	volatile uint32_t APB1LPENR;    //APB1 Clock Enable Low Power Register     ADDR_OFFSET:  0x60
	volatile uint32_t APB2LPENR;    //APB2 Clock Enable Low Power Register     ADDR_OFFSET:  0x64
	volatile uint32_t reserved10;   //reserved                                 ADDR_OFFSET:  0x68
	volatile uint32_t reserved11;   //reserved                                 ADDR_OFFSET:  0x6C
	volatile uint32_t BDCR;         //Backup Domain Control Register           ADDR_OFFSET:  0x70
	volatile uint32_t CSR;          //Clock Control & Status Register          ADDR_OFFSET:  0x74
	volatile uint32_t reserved12;   //reserved                                 ADDR_OFFSET:  0x78
	volatile uint32_t reserved13;   //reserved                                 ADDR_OFFSET:  0x7C
	volatile uint32_t SSCGR;        //Spread Spectrum Clock Generation Reg     ADDR_OFFSET:  0x80
	volatile uint32_t PLLI2SCFGR;   //PLLI2S Configuration Register            ADDR_OFFSET:  0x84
	volatile uint32_t reserved14;   //reserved                                 ADDR_OFFSET:  0x88
	volatile uint32_t DCKCFGR;      //Dedicated Clocks Configuration Reg       ADDR_OFFSET:  0x8C

} RCC_RegDef_t;













#endif /* STM32F411XX_HAL_H_ */
