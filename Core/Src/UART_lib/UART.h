/*
 * UART.h
 *
 *  Created on: May 10, 2023
 *      Author: colecosta7
 */

#ifndef SRC_UART_LIB_UART_H_
#define SRC_UART_LIB_UART_H_

#define CLEAR_BRR 0xFFFF
#define SET_BRR 0xD1
void UART_init(void);
void UART_print_char(char character);
void UART_print(char* string);
void UART_ESC_code(char* string);

#endif /* SRC_UART_LIB_UART_H_ */
