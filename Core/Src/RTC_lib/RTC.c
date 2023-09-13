/*
 * RTC.c
 *
 *  Created on: Jun 3, 2023
 *      Author: colecosta7
 */

#include "RTC.h"

void RTC_init(void){
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
	I2C1 -> TIMINGR = TIMING_FACTOR_RTC;
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
	GPIOB -> OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	//high speed
	GPIOB -> OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED8_Pos | 2 << GPIO_OSPEEDR_OSPEED9_Pos);
	//no pull up or pull down - will use a 5.1k resistor in hardware
	GPIOB -> PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
}

void RTC_write(uint8_t addr, uint8_t data){
	//set the number of bytes to be sent
	I2C1 -> CR2 &= ~(I2C_CR2_NBYTES);
	I2C1 -> CR2 |= (2 << I2C_CR2_NBYTES_Pos);
	//configure the slave address
	I2C1 -> CR2 &= ~(I2C_CR2_SADD);
	I2C1 -> CR2 |= (RTC_ADDR << 1);
	//set the data transfer to a write
	I2C1 -> CR2 &= ~(I2C_CR2_RD_WRN);
	//set to 7-bit addressing
	I2C1 -> CR2 &= ~(I2C_CR2_ADD10);
	//set I2C start bit
	I2C1 -> CR2 |= (I2C_CR2_START);

	//send the address byte
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	I2C1 -> TXDR = addr;

	//send a byte of data
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	I2C1 -> TXDR = data;
	//wait for transfer to complete
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	//send stop condition
	I2C1 -> CR2 |= (I2C_CR2_STOP);

}

uint8_t RTC_read(uint8_t addr){
	uint8_t data;
	//set the number of bytes to be sent
	I2C1 -> CR2 &= ~(I2C_CR2_NBYTES);
	I2C1 -> CR2 |= (1 << I2C_CR2_NBYTES_Pos);

	//configure the slave address
	I2C1 -> CR2 &= ~(I2C_CR2_SADD);
	I2C1 -> CR2 |= (RTC_ADDR << 1);
	//set the data transfer to a write
	I2C1 -> CR2 &= ~(I2C_CR2_RD_WRN);
	//set to 7-bit addressing
	I2C1 -> CR2 &= ~(I2C_CR2_ADD10);
	//set I2C start bit
	I2C1 -> CR2 |= (I2C_CR2_START);

	//send the address byte
	//wait for TX to be empty
	while(!(I2C1 -> ISR & I2C_ISR_TXIS));
	I2C1 -> TXDR = addr;

	//wait for transfer to complete
	while(!(I2C1 -> ISR & I2C_ISR_TC));

	//setup read
	//set the number of bytes to be read
	I2C1 -> CR2 &= ~(I2C_CR2_NBYTES);
	I2C1 -> CR2 |= (1 << I2C_CR2_NBYTES_Pos);
	//configure the slave address
	I2C1 -> CR2 &= ~(I2C_CR2_SADD);
	I2C1 -> CR2 |= (RTC_ADDR << 1);
	//set the data transfer to a read
	I2C1 -> CR2 |= (I2C_CR2_RD_WRN);
	//set I2C start bit
	I2C1 -> CR2 |= (I2C_CR2_START);

	//wait for RX to not be empty
	while(!(I2C1 -> ISR & I2C_ISR_RXNE));
	//read a byte
	data = I2C1 -> RXDR;

	//wait for transfer to complete
	while(!(I2C1 -> ISR & I2C_ISR_TC));
	//send stop condition
	I2C1 -> CR2 |= (I2C_CR2_STOP);

	return data;
}

void RTC_setTime(uint8_t sec, uint8_t min, uint8_t hour,
		uint8_t day, uint8_t date, uint8_t month, uint8_t year){
	//write the seconds to the RTC
	RTC_write(SEC_ADDR, sec);
	for(int16_t i = 0; i < 1000; i++);
	//write the minutes to the RTC
	RTC_write(MIN_ADDR, min);
	for(int16_t i = 0; i < 1000; i++);
	//write the hour to the RTC
	RTC_write(HOUR_ADDR, hour);
	for(int16_t i = 0; i < 1000; i++);
	//write the day to the RTC
	RTC_write(DAY_ADDR, day);
	for(int16_t i = 0; i < 1000; i++);
	//write the date to the RTC
	RTC_write(DATE_ADDR, date);
	for(int16_t i = 0; i < 1000; i++);
	//write the month to the RTC
	RTC_write(MONTH_ADDR, month );
	for(int16_t i = 0; i < 1000; i++);
	//write the year to the RTC
	RTC_write(YEAR_ADDR, year);
	for(int16_t i = 0; i < 1000; i++);

}

void RTC_readTimes(void){
	//variables to hold times
	  uint8_t sec;
	  uint8_t min;
	  uint8_t hour;
	  uint8_t day;
	  uint8_t date;
	  uint8_t month;
	  uint8_t year;
	  uint8_t PM;
	  //variable to hold string
	  char result[4];

	  //clear the screen and cursor to left corner
	  UART_print("\033[2J\033[H");

	  //read convert then print the day of week
	  UART_print("Today is ");
	  //turn on underline
	  UART_print("\033[4m");
	  day = RTC_read(DAY_ADDR);
	  day = HexToDec(day);
	  if(day == 1)
		  UART_print("Sunday");
	  else if(day == 2)
		  UART_print("Monday");
	  else if(day == 3)
		  UART_print("Tuesday");
	  else if(day == 4)
		  UART_print("Wednesday");
	  else if(day == 5)
		  UART_print("Thursday");
	  else if(day == 6)
		  UART_print("Friday");
	  else
		  UART_print("Saturday");

	  //move cursor down and to the left and turn off underline
 	  UART_print("\033[1B\033[20D\033[0m");

	  //read,convert, then print min, sec, hours
	  UART_print("TIME -  ");

	  hour = RTC_read(HOUR_ADDR);
	  if((1 << 5) & hour)
		  PM = 1;
	  hour = HexToDec(hour & (0x1F));
	  uint16ToString(hour, result);
	  UART_print(result);
	  UART_print(":");

	  min = RTC_read(MIN_ADDR);
	  min = HexToDec(min);
	  uint16ToString(min, result);
 	  UART_print(result);
 	  UART_print(":");

 	  sec = RTC_read(SEC_ADDR);
 	  sec = HexToDec(sec);
 	  uint16ToString(sec, result);
 	  UART_print(result);

 	  if(PM)
 		  UART_print(" PM");
 	  else
 		  UART_print(" AM");

 	  //move cursor to the left and down 1 line
 	  UART_print("\033[1B\033[20D");

 	  //read, convert, then print month, date, year
 	  UART_print("DATE - ");
 	  month = RTC_read(MONTH_ADDR);
 	  month = HexToDec(month & 0x1F);
 	  uint16ToString(month, result);
 	  UART_print(result);
 	  UART_print("/");

 	  date = RTC_read(DATE_ADDR);
 	  date = HexToDec(date);
 	  uint16ToString(date, result);
 	  UART_print(result);
 	  UART_print("/");

 	  year = RTC_read(YEAR_ADDR);
 	  year = HexToDec(year);
 	  uint16ToString(year, result);
 	  UART_print(result);

 	  //delay between prints to the screen
	  for(uint32_t i = 0; i < 1000000; i++);

}

void RTC_setAlarmTime(uint8_t sec, uint8_t min, uint8_t hour){
	//write the alarm seconds to the RTC
	RTC_write(ALARM_SEC_ADDR, sec);
	for(int16_t i = 0; i < 1000; i++);
	//write the alarm minutes to the RTC
	RTC_write(ALARM_MIN_ADDR, min);
	for(int16_t i = 0; i < 1000; i++);
	//write the alarm hour to the RTC
	RTC_write(ALARM_HOUR_ADDR, hour);
	for(int16_t i = 0; i < 1000; i++);
	//write the alarm day to the RTC
	RTC_write(ALARM_DAY_ADDR, 0x81);
	for(int16_t i = 0; i < 1000; i++);

}

uint16_t uint16ToString(uint16_t value, char *str){
    // Handle special case for 0
	for (uint8_t i = 0; i < 5; i++) {
	        str[i] = '\0';
	}
    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }

    // Convert the value to a string in reverse order
    uint16_t num = value;
    int i = 0;
    while (num != 0)
    {
        uint16_t remainder = num % 10;
        str[i++] = '0' + remainder;
        num /= 10;
    }

    // Reverse the string
    int j, len = i;
    for (i = 0, j = len - 1; i < j; i++, j--)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }

    // Add null terminator
    str[4] = '\0';

    return 1;
}




