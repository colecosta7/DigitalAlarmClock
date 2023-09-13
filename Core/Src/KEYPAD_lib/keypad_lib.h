/*
 * keypad_lib.h
 *
 *  Created on: Apr 12, 2023
 *      Author: colecosta7
 */

#ifndef SRC_KEYPAD_LIB_H_
#define SRC_KEYPAD_LIB_H_
#include "main.h"
#define ROW_PORT GPIOC
#define COL_PORT GPIOB
#define ROW_MASK (GPIO_IDR_ID4 | GPIO_IDR_ID5 | GPIO_IDR_ID6 | GPIO_IDR_ID7)

void keypad_config(void);

int8_t keypad_getKey(void);

uint8_t keypad_input(void);

#endif /* SRC_KEYPAD_LIB_H_ */
