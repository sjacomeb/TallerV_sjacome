/*
 * USARTxDriver.c
 *
 *      Author: sjacome
 */

#include <stm32f4xx.h>
#include <USARTxDriver.h>

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */

uint8_t auxRxData = 0;
uint8_t tipoDato = 0;
uint8_t cont = 0;
char *msg = {0};
char dataTX = 0;


/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){

		RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;
	}

    /* 1.2 Configuramos el USART2 */

	if(ptrUsartHandler-> ptrUSARTx  == USART2){
		RCC -> APB1ENR |= RCC_APB1ENR_USART2EN;
	}

    /* 1.3 Configuramos el USART6 */

	if(ptrUsartHandler-> ptrUSARTx == USART6){
		RCC -> APB2ENR |= RCC_APB2ENR_USART6EN;
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada

			ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PS); //Even=0

		}else{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion

			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;  //ODD=1
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check

		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_PCE);
	}

	// 2.3 Configuramos el tamaño del dato

	if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_9BIT){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M ; //8 databits = 0
	}else{
		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_M); //9 databits = 1
	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemoscargar el valor 0b00 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP_0);
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP_1);
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 |=   USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP_1);
		break;
	}
	case USART_STOPBIT_2: {
		// Debemoscargar el valor 0b10 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 &= ~(USART_CR2_STOP_0);
		ptrUsartHandler->ptrUSARTx->CR2 |=   USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemos cargar el valor 0b11 en los dos bits de STOP

		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;

		break;
	}
	default: {
		// En el caso por defecto seleccionamos 1 bit de parada

		break;
	}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)

	//Frecuencia 16 MHz
	if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
		// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
		// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
		// Valor a cargar 0x0683
		// Configurando el Baudrate generator para una velocidad de 9600bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
	}
	else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
		// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
		// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
		// Valor a cargar 0x341
		// Configurando el Baudrate generator para una velocidad de 19200bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x341;
	}
	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
		// El valor a cargar es 8.6875 -> Mantiza = 8, fraction = 0.6875
		// Mantiza = 8 = 0X8, fraction = 16* 0.6875 = 0xB
		// Valor a cargar 0x8B
		// Configurando el Baudrate generator para una velocidad de 115200bps
		ptrUsartHandler->ptrUSARTx->BRR = 0x8B;
	}


//	//Frecuencia 80 MHz
//	if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
//		// El valor a cargar es 520,833
//		// Mantiza = 520 = 0x208, fraction = 16 * 0.8333 = 13 = 0xD
//		// Valor a cargar 0x208D
//		ptrUsartHandler->ptrUSARTx->BRR = 0x208D;
//	}
//	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200){
//		// El valor a cargar es 260,41666
//		// Mantiza = 260 = 0x104, fraction = 16 * 0,41666 = 7 = 0x7
//		// Valor a cargar 0x1047
//		ptrUsartHandler->ptrUSARTx->BRR = 0x1047;
//	}
//	else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
//		// El valor a cargar es 43,40277
//		// Mantiza = 43 = 0x2B, fraction = 16 * 0,40277 = 7 = 0x7
//		// Valor a cargar 0x02B7
//		ptrUsartHandler->ptrUSARTx->BRR = 0x02B7;
//	}

	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;


		break;
	}

	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;

		ptrUsartHandler->ptrUSARTx->CR1 &= ~(USART_CR1_TE);
		break;
	}
	}

	//2.8 Otra cosa que no salió en el video pero tenia que saberlo
	if(ptrUsartHandler-> USART_Config.USART_enableIntRX == USART_RX_INTERRUPT_ENABLE){
		//Desactivamos las interrupciones globales
		__disable_irq();

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RXNEIE;
		//Activamos la interrupcion usart
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;

		//Matriculamos la interrupcion en el NVIC
		if(ptrUsartHandler->ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		//Activar las interrupciones globales
		__enable_irq();

	}

	//2.9 Transmisión
	if(ptrUsartHandler-> USART_Config.USART_enableIntTX == ENABLE){
		//Desactivamos las interrupciones globales
		__disable_irq();

		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TXEIE;
		//Activamos la interrupcion usart
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;

		//Matriculamos la interrupcion en el NVIC
		if(ptrUsartHandler->ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}
		//Activar las interrupciones globales
		__enable_irq();
	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){

		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}


}

/* Funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, char dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){  //Verifica que el Tx este vacio
		__NOP();
	}

	ptrUsartHandler->ptrUSARTx->DR = dataToSend;

	return dataToSend;
}

/* Función para escribir un string */
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend ){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

/* Función para escribir un char (interrupcion Tx) */
void writeCharTX(USART_Handler_t *ptrUsartHandler, char dataToSend ){
	tipoDato = 0;   //Char
	dataTX = dataToSend;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;
}

/* Función para escribir un string (interrupcion Tx) */
void writeMsgTX(USART_Handler_t *ptrUsartHandler, char *msgToSend){
	tipoDato = 1;  //String
	msg = msgToSend;
	ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TXEIE;

}

//Lectura del caracter que llega por la interface serial
uint8_t getRxData(void){
	return auxRxData;
}


/* Handler de la interrupcion del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE
 */

void USART1_IRQHandler(void){
	//Evaluamos si la interrupcion que se dio es por RX
	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1-> DR;
		usart1Rx_Callback();
	}
	//Si la interrupción es TX
	else if(USART1->SR & USART_SR_TXE){
		//Si es un char
		if(tipoDato == 0){
			USART1->DR = dataTX;
			USART1->CR1 &= ~(USART_CR1_TXEIE);
		//Si es un string
		}else if(tipoDato == 1){
			if(msg[cont] != '\0'){
				USART1->DR = msg[cont];
				cont++;
			}else if(msg[cont] == '\0'){
				USART1->CR1 &= ~(USART_CR1_TXEIE);
				cont = 0;
			}
		}else{
			__NOP();
		}
	}
}

void USART2_IRQHandler(void){
	//Evaluamos si la interrupcion que se dio es por RX
	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2-> DR;
		usart2Rx_Callback();
	}
	//Si la interrupción es TX
	else if(USART2->SR & USART_SR_TXE){
		if(tipoDato == 0){
			USART2->DR = dataTX;
			USART2->CR1 &= ~(USART_CR1_TXEIE);
		}else if(tipoDato == 1){
			if(msg[cont] != '\0'){
				USART2->DR = msg[cont];
				cont++;
			}else if(msg[cont] == '\0'){
				USART2->CR1 &= ~(USART_CR1_TXEIE);
				cont = 0;
			}
		}else{
			__NOP();
		}
	}
}

void USART6_IRQHandler(void){
	//Evaluamos si la interrupcion que se dio es por RX
	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6-> DR;
		usart6Rx_Callback();
	}
}

__attribute__((weak)) void usart1Rx_Callback(void){

	__NOP();
}

__attribute__((weak)) void usart2Rx_Callback(void){

	__NOP();
}

__attribute__((weak)) void usart6Rx_Callback(void){

	__NOP();
}
