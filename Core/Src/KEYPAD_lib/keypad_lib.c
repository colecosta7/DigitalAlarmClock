/*
 * keypad_lib.c
 *
 *  Created on: Apr 24, 2023
 *      Author: colecosta7
 */

#include "keypad_lib.h"
//2d constant lookup table to find each number
const int8_t keypad_matrix[4][4] = {{1,2,3,10}, {4,5,6,11}, {7,8,9,12}, {14, 0, 15, 13}};

void keypad_config(void){
	//Enable Clock
	RCC -> AHB2ENR |= (RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN);

	//Enable Rows and Cols MODE Register
	//Sets ROW as INPUT type
	ROW_PORT -> MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 |
			GPIO_MODER_MODE7);

	//Sets COL as OUTPUT type
	COL_PORT -> MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 |
				GPIO_MODER_MODE7);
	COL_PORT -> MODER |= (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 |
			GPIO_MODER_MODE7_0);

	//Enable PUPDR as PULL DOWN for ROWS
	ROW_PORT -> PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 |
			GPIO_PUPDR_PUPD7);
	ROW_PORT -> PUPDR |= (GPIO_PUPDR_PUPD4_1 | GPIO_PUPDR_PUPD5_1 | GPIO_PUPDR_PUPD6_1 |
			GPIO_PUPDR_PUPD7_1);

	//Enable PUPDR as OFF for COLS
	COL_PORT -> PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 |
			GPIO_PUPDR_PUPD7);

	//Enable LOW Output Speed for COLS
	COL_PORT -> OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 |
			GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

	//Enable OTYPE to Push Pull for COLS
	COL_PORT -> OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 |
			GPIO_OTYPER_OT7);

	//Turn on all COLS
	COL_PORT -> BSRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);


}

int8_t keypad_getKey(void){
	//if a row is detected high
	if(ROW_PORT -> IDR & ROW_MASK){
		int8_t row_num = 0;
		int8_t col_num = 0;
		//turn off all columns
		COL_PORT -> BRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
		//iterate through each column
		for(int col = GPIO_PIN_4; col <= GPIO_PIN_7; col <<= 1){
			//turn on individual column
			COL_PORT -> BSRR = col;
			//if a Row is high on this column
			if(ROW_PORT -> IDR & ROW_MASK){
				//iterate through each row
				for(int row = GPIO_PIN_4; row <= GPIO_PIN_7; row <<= 1){
					//if the row is high
					if(ROW_PORT -> IDR & row){
						//Turn on all the columns to prepare for the next key press
						COL_PORT -> BSRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
								GPIO_PIN_7);
						//return the right number from 2d lookup table
						return keypad_matrix[row_num][col_num];
					}
					//else increment
					row_num++;
				}
			}
			//else increment column # and turn off the column
			col_num++;
			COL_PORT -> BRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |
					GPIO_PIN_7);
		}
	}
	//if no key press, return -1
	COL_PORT -> BSRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

	return -1;
}

uint8_t keypad_input(void){
	 int8_t keypad_pressed = 0;
	 uint8_t total_time = 0;

	  while (keypad_pressed < 2)
	  {
		  int8_t key_pressed = keypad_getKey();
		  if (key_pressed != -1 && key_pressed < 10) {
			  keypad_pressed += 1;
			  if (keypad_pressed == 1) {
				  total_time += (uint8_t) key_pressed * 10;
			  }
			  if (keypad_pressed == 2) {
				  total_time += (uint8_t) key_pressed;
			  }
			  for(int i =0; i < 1000000; i++);
		  }
	  }

	  return total_time;

}
