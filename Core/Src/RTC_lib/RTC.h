/*
 * RTC.h
 *
 *  Created on: Jun 3, 2023
 *      Author: colecosta7
 */

#ifndef SRC_RTC_LIB_RTC_H_
#define SRC_RTC_LIB_RTC_H_
#include "main.h"
#include "../UART_lib/UART.h"


#define RTC_ADDR 0x68
#define TIMING_FACTOR_RTC 0x00200C28
#define SEC_ADDR 0x00
#define MIN_ADDR 0x01
#define HOUR_ADDR 0x02
#define DAY_ADDR 0x03
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06
#define ALARM_SEC_ADDR 0x07
#define ALARM_MIN_ADDR 0x08
#define ALARM_HOUR_ADDR 0x09
#define ALARM_DAY_ADDR 0x0A
#define CONTROL_REG 0x0E
#define STATUS_REG 0x0F


void RTC_init(void);
void RTC_write(uint8_t addr, uint8_t data);
uint8_t RTC_read(uint8_t addr);
void RTC_setTime(uint8_t sec, uint8_t min, uint8_t hour,
		uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void RTC_readTimes(void);
void RTC_setAlarmTime(uint8_t sec, uint8_t min, uint8_t hour);
uint16_t uint16ToString(uint16_t value, char *str);


#endif /* SRC_RTC_LIB_RTC_H_ */
