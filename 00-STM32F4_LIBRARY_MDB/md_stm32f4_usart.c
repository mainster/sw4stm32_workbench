#include "md_stm32f4_usart.h"

int8_t MD_USART_Buff[8][MD_USART_DEFAULT_BUFFER_SIZE];
uint16_t md_usart_buf_in[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t md_usart_buf_out[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t md_usart_buf_num[8] = {0, 0, 0, 0, 0, 0, 0, 0};


uint8_t MD_USART_GetUsartNumber(USART_TypeDef* USARTx) {
	if (USARTx == USART1) {
		return 0;
	}
	if (USARTx == USART2) {
		return 1;
	}
	if (USARTx == USART3) {
		return 2;
	}
	if (USARTx == UART4) {
		return 3;
	}
	if (USARTx == UART5) {
		return 4;
	}
	return 0;
}

void MD_USART_Init(USART_TypeDef* USARTx, MD_USART_PinsPack_t pinspack, uint32_t baudrate) {
	USART_InitTypeDef 	USART_InitStruct;
	NVIC_InitTypeDef	NVIC_InitStruct;

	USART_InitStruct.USART_BaudRate = baudrate;
	
	/*
	 * Initialize USARTx pins
	 * Set channel for USARTx NVIC
	 */


	USART_Init(USARTx, &USART_InitStruct);
	USART_Cmd(USARTx, ENABLE);
	
	/**
	 * Enable RX interrupt
	 */
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);

	/**
	 * Set Channel Cmd to enable. That will enable USARTx channel in NVIC
	 * Set NVIC_IRQChannelPreemptionPriority. This means high priority for USARTs
	 * USARTx with lower x has high subpriority
	 *
	 * Initialize NVIC
	 */
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = MD_USART_NVIC_PRIORITY;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = MD_USART_GetUsartNumber(USARTx);
	NVIC_Init(&NVIC_InitStruct);
}

uint8_t TM_USART_BufferEmpty(USART_TypeDef* USARTx) {
	uint8_t usart_num = MD_USART_GetUsartNumber(USARTx);
	return !(md_usart_buf_num[usart_num] > 0);
}

uint8_t MD_USART_Getc(USART_TypeDef* USARTx) {
	uint8_t usart_num = MD_USART_GetUsartNumber(USARTx);
	uint8_t c = 0;
	//Check if we have any data in buffer
	if (md_usart_buf_num[usart_num] > 0) {
		if (md_usart_buf_out[usart_num] > (MD_USART_DEFAULT_BUFFER_SIZE - 1)) {
			md_usart_buf_out[usart_num] = 0;
		}
		c = MD_USART_Buff[usart_num][md_usart_buf_out[usart_num]];
		MD_USART_Buff[usart_num][md_usart_buf_out[usart_num]] = 0;
		md_usart_buf_out[usart_num]++;
		md_usart_buf_num[usart_num]--;
	}
	return c;
}

uint8_t MD_USART_BufferEmpty(USART_TypeDef* USARTx) {
	uint8_t usart_num = MD_USART_GetUsartNumber(USARTx);
	return !(md_usart_buf_num[usart_num] > 0);
}

uint16_t MD_USART_Gets(USART_TypeDef* USARTx, char* buffer, uint16_t bufsize) {
	uint16_t i = 0;                             
	uint8_t eol = 0;
	if (MD_USART_BufferEmpty(USARTx)) {
		return 0;
	}
	if (bufsize > 0) {
		while (!eol) {
			while (TM_USART_BufferEmpty(USARTx));
			buffer[i] = (char) TM_USART_Getc(USARTx);   
			if (buffer[i] == '\n') {
				eol = 1;                
			} else {            
				if (i < (bufsize - 1)) {
					i++; 	
				}
			}
		}
		//Add zero to the end of string
		buffer[i] = 0;               
	}

	return (i);
}

//uint8_t TM_USART_BufferEmpty(USART_TypeDef* USARTx) {
//	uint8_t usart_num = TM_USART_GetUsartNumber(USARTx);
//	return !(tm_usart_buf_num[usart_num] > 0);
//}

//void TM_USART_ClearBuffer(USART_TypeDef* USARTx) {
//	uint8_t usart_num = TM_USART_GetUsartNumber(USARTx);
//	tm_usart_buf_num[usart_num] = 0;
//	tm_usart_buf_in[usart_num] = 0;
//	tm_usart_buf_out[usart_num] = 0;
//}
