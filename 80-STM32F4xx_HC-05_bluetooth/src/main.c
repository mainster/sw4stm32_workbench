/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint-gcc.h>

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "delay.h"
#include "string.h"

char usart_buffer[100];
char gelen;
int sayac = 0;
char buff[100];

void init_USART1(uint32_t baudrate) {

	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIOD->MODER = 0x55000000; // GPIOD nin 15, 14, 13, 12 pinleri cikis tanimlandi (LEDler icin)
	GPIOD->OSPEEDR = 0xFFFFFFFF;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins B6 (TX) and B7 (RX)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	USART_InitStruct.USART_BaudRate = baudrate;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStruct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

void USART_puts(USART_TypeDef* USARTx, volatile char *s) {

	while (*s) {

		while (!(USARTx->SR & 0x00000040))
			;
		USART_SendData(USARTx, *s);
		*s++;
	}
}

void send_char(char c) {
	while (!(USART1->SR & 0x80))
		;
	USART1->DR = c;
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE)) {
		gelen = USART1->DR;
		if (gelen != '\n') {
			usart_buffer[sayac] = gelen;
			sayac++;
		} else {

			usart_buffer[sayac] = '\0';
			USART_puts(USART1, usart_buffer);
			sayac = 0;
		}
		if (gelen == '1')
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
		if (gelen == '2')
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
		if (gelen == '3')
			GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
		if (gelen == '4')
			GPIO_ToggleBits(GPIOD, GPIO_Pin_15);

	}
}

int main(void) {
	init_USART1(38400);
	char buffer[35];

	while (1) {
		sprintf(buffer, "Stm32F4 HC 05 Bluetooth Deneme %d \n", sayac);
		USART_puts(USART1, buffer);
		delay_ms(1000);
		sayac++;
	}
}
