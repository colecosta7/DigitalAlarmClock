/*
 * LCD.c
 *
 *  Created on: Jun 3, 2023
 *      Author: colecosta7
 */

#include "LCD.h"

void I2C_init(void){
	//USING DEVICE ADDR 0x51 on the EEPROM
	//enable peripheral clock
	RCC -> APB1ENR1 |= (RCC_APB1ENR1_I2C1EN);
	//disable peripheral / clear PE bit
	I2C1 -> CR1 &= ~(I2C_CR1_PE);
	//Enable Analog Noise Filter
	I2C1 -> CR1 &= ~(I2C_CR1_ANFOFF);
	//Disable Digital Noise Filter
	I2C1 -> CR1 &= ~(I2C_CR1_DNF);
	//configure timing for I2C, from MXTOOL
	I2C1 -> TIMINGR = TIMINGFACTOR100;
	//clock stretching enabled for master mode
	I2C1 -> CR1 &= ~(I2C_CR1_NOSTRETCH);
	//enable peripheral
	I2C1 -> CR1 |= (I2C_CR1_PE);

	//setup GPIO PINS PB8 and PB9 for AF4
	//PB8: SCL, PB9: SDA
	//enable clocks GPIOB
	RCC -> AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
	//Alternate Function mode AF4
	GPIOB -> MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB -> MODER |= (2 << GPIO_MODER_MODE8_Pos |
			2 << GPIO_MODER_MODE9_Pos);
	GPIOB -> AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
	GPIOB -> AFR[1] |= (4 << GPIO_AFRH_AFSEL8_Pos |
			4 << GPIO_AFRH_AFSEL9_Pos);
	//Set open drain SDA and SCL
	GPIOB -> OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT8);
	//very high speed
	GPIOB -> OSPEEDR |= (GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
	//no pull up or pull down - will use a 2k resistor in hardware
	GPIOB -> PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
}

void LCD_init(void){
	//initial 40ms delay
	for(uint16_t i = 0; i < 1000; i++);
	//write the function set
	LCD_writeINSTR(0x30);
	//delay again
	for(uint16_t i = 0; i < 1000; i++);
	//write the function set
	LCD_writeINSTR(0x30);
	//delay again
	for(uint16_t i = 0; i < 1000; i++);
	//write the function set
	LCD_writeINSTR(0x30);
	//delay again
	for(uint16_t i = 0; i < 1000; i++);
	//write the 4 bit interface
	LCD_writeINSTR(0x30);
	//delay again
	for(uint16_t i = 0; i < 1000; i++);
	//set to 2 line 5x8 font
	LCD_writeINSTR(0x28);
	//delay again
	for(uint16_t i = 0; i < 1000; i++);
	//turn off display
	LCD_writeINSTR(0x00);
	//delay
	for(uint16_t i = 0; i < 1000; i++);

}

void LCD_writeINSTR(uint8_t data){
	//set the number of bytes to be sent
	I2C1 -> CR2 &= ~(I2C_CR2_NBYTES);
	I2C1 -> CR2 |= (4 << I2C_CR2_NBYTES_Pos);
	//configure the slave address
	I2C1 -> CR2 &= ~(I2C_CR2_SADD);
	I2C1 -> CR2 |= (LCDADDR << 1);
	//set the data transfer to a write
	I2C1 -> CR2 &= ~(I2C_CR2_RD_WRN);
	//set to 7-bit addressing
	I2C1 -> CR2 &= ~(I2C_CR2_ADD10);
	//set I2C start bit
	I2C1 -> CR2 |= (I2C_CR2_START);

	//send the upper nibble of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	//send data OR with EN = 1
	I2C1 -> TXDR = (data & 0xF0) | 0x0C;

	//send the upper nibble of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	//send data OR with EN = 0
	I2C1 -> TXDR = (data & 0xF0) | 0x08;

	//send the lower nibble of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	//send data OR with EN = 1
	I2C1 -> TXDR = ((data & 0x0F) << 4) | 0x0C;

	//send the lower nibble of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	//send data OR with EN = 0
	I2C1 -> TXDR = ((data & 0x0F) << 4) | 0x08;

	//wait for transfer to complete
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	//send stop condition
	I2C1 -> CR2 |= (I2C_CR2_STOP);

}

void LCD_writeDATA(uint8_t data){
	//set the number of bytes to be sent
	I2C1 -> CR2 &= ~(I2C_CR2_NBYTES);
	I2C1 -> CR2 |= (2 << I2C_CR2_NBYTES_Pos);
	//configure the slave address
	I2C1 -> CR2 &= ~(I2C_CR2_SADD);
	I2C1 -> CR2 |= (LCDADDR << 1);
	//set the data transfer to a write
	I2C1 -> CR2 &= ~(I2C_CR2_RD_WRN);
	//set to 7-bit addressing
	I2C1 -> CR2 &= ~(I2C_CR2_ADD10);
	//set I2C start bit
	I2C1 -> CR2 |= (I2C_CR2_START);

	//send the upper nibble of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	I2C1 -> TXDR = (data & 0xF0);

	//wait for transfer to complete
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	//send stop condition
	I2C1 -> CR2 |= (I2C_CR2_STOP);

}

