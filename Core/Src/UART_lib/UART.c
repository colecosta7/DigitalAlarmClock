/*
 * UART.c
 *
 *  Created on: May 10, 2023
 *      Author: colecosta7
 */


#include "UART.h"
#include "stm32l476xx.h"
#include "string.h"

void UART_init(void)
{
	// configure UART settings
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // clock enable
	USART2->BRR &= ~(CLEAR_BRR); // clear baud rate
	USART2->BRR |= (SET_BRR); // set baud rate to 115.2 kbps
	USART2->CR1 &= ~(USART_CR1_M); // 8 bit word length
	USART2->CR1 &= ~(USART_CR2_STOP); // 1 stop bit
	USART2->CR1 &= ~(USART_CR1_TE | USART_CR1_RE);
	USART2->CR1 |= (USART_CR1_TE); // transmit enable
	USART2->CR1 |= (USART_CR1_RE); // receive enable
	USART2->CR1 &= ~(USART_CR1_OVER8); // oversampling by 16
	USART2->CR1 |= (USART_CR1_RXNEIE); // enable receive interrupts
	USART2->CR1 |= (USART_CR1_UE); // USART enable
	// set GPIO pins for USART transmit and receive
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // clock enable
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
       // AF mode
	GPIOA->MODER |= (2 << GPIO_MODER_MODE2_Pos | 2 << GPIO_MODER_MODE3_Pos);
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
       // USART2 AF select
	GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFSEL2_Pos | 7 << GPIO_AFRL_AFSEL3_Pos);
	GPIOA->OSPEEDR &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
       // very fast speed
	GPIOA->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED2_Pos | 3 << GPIO_OSPEEDR_OSPEED3_Pos); }

void UART_print_char(char character)
{
	while (USART2->ISR & (USART_ISR_TXE == 0))
	{
	}
	USART2->TDR = character;  // write data to transit register
	for (int delay = 0; delay < 1000; delay++);
}

void UART_print(char* string)
{
	while (USART2->ISR & (USART_ISR_TXE == 0))
	{
	}
	for (int i = 0; i < strlen(string) + 1; i++)
	{
		UART_print_char(string[i]);
		for (int delay = 0; delay < 1000; delay++);
	}
	for (int delay = 0; delay < 1000; delay++);
}

void UART_ESC_code(char* string)
{
	while (USART2->ISR & (USART_ISR_TXE == 0))
	{
	}
	USART2->TDR = 0x1b;  // write escape character
	for (int delay = 0; delay < 1000; delay++);
	for (int i = 0; i < strlen(string) + 1; i++)
	{
		USART2->TDR = string[i];
		for (int delay = 0; delay < 1000; delay++);
	}
	for (int delay = 0; delay < 1000; delay++);
}

