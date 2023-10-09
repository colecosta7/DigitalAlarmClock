/*
 * LCD.h
 *
 *  Created on: Jun 3, 2023
 *      Author: colecosta7
 */

#ifndef SRC_LCD_LIB_LCD_H_
#define SRC_LCD_LIB_LCD_H_
#include "main.h"
//Timing constant for 100 kHz I2C communication
#define TIMINGFACTOR100 0x00506682
#define LCDADDR 0x27

void I2C_init(void);
void LCD_init(void);
void LCD_writeINSTR(uint8_t data);
void LCD_writeDATA(uint8_t data);
void LCD_writeString(char *str);
void LCD_clearDisplay(void);
void LCD_replaceCursor(void);
void LCD_putCursor(uint8_t row, uint8_t col);




#endif /* SRC_LCD_LIB_LCD_H_ */
