/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * Author: colecosta7
  * Alarm Clock that interfaces a DS3231 Real Time Clock Module and a 4x4 keypad
  * with the NUCLEO-L476RG.
  ******************************************************************************
**/

#include "main.h"
#include "LCD_lib/LCD.h"
#include "RTC_lib/RTC.h"
#include "UART_lib/UART.h"
#include "KEYPAD_lib/keypad_lib.h"

void SystemClock_Config(void);
uint8_t HexToDec(uint8_t data);
uint8_t DecToHex(uint8_t data);


int main(void)
{
  HAL_Init();

  SystemClock_Config();

  //initialize I2C to be used for peripheral
  RTC_init();
  //initialize UART
  UART_init();
  //configure keypad
  keypad_config();

  //declare state variables
  typedef enum{
	  GET_KEY,
	  DISPLAY_TIME,
	  DISPLAY_CMDS,
	  DISPLAY_ALARM,
	  ALARM_INT,
	  SET_TIME,
	  SET_ALARM
  }state_type;

  //holds current state
  state_type state = GET_KEY;

  //time variables
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t date;
  uint8_t month;
  uint8_t year;
  uint8_t PM;
  uint8_t data;
  char result[4];
  uint8_t status;


  //holds the keypress
  int8_t keypress;
  //FSM
  while (1)
  {
	  switch (state){
	  case GET_KEY:
		  //get key from keypad
		  keypress = keypad_getKey();

		  //if no keypressed
		  if(keypress == -1)
			  state = DISPLAY_TIME;

		  //if A is pressed
		  else if(keypress == 10)
			  state = SET_ALARM;

		  //if B is pressed
		  else if(keypress == 11)
			  state = SET_TIME;

		  //if C is pressed
		  else if(keypress == 12)
			  state = DISPLAY_CMDS;

		  //if D is pressed
		  else if(keypress == 13)
			  state = DISPLAY_ALARM;

		  //else stay
		  else
			  state = GET_KEY;

		  break;

	  case DISPLAY_TIME:
		  //read and print the times from RTC
		  RTC_readTimes();

		  //read the contents of the status register
		  status = RTC_read(STATUS_REG);
		  //if the Alarm1 flag is high is status register
		  if(status & 0x01)
			  //this means the alarm is going off
			  state = ALARM_INT;

		  //if it is not going off, stay
		  else
			  state = GET_KEY;
		  break;

	  case DISPLAY_CMDS:
		  //prints a list of possible commands to the alarm clock
		  //clear the screen and position cursor top left
		  UART_print("\033[2J\033[H");
		  UART_print("CMDS: ");
		  //move left and down a line
		  UART_print("\033[1B\033[10D");
		  UART_print("Hold A to set an alarm");
		  //move left and down a line
		  UART_print("\033[1B\033[30D");
		  UART_print("Hold B to set the current time");
		  //move left and down a line
		  UART_print("\033[1B\033[30D");
		  UART_print("Hold C to display commands");
		  //move left and down a line
		  UART_print("\033[1B\033[30D");
		  UART_print("Hold D to display alarms");
		  //move left and down 2 lines
		  UART_print("\033[2B\033[30D");
		  UART_print("Press any key to return to clock and enter a new command");

		  //wait for a key to be pressed
		  keypress = keypad_getKey();
		  while(keypress == -1)
			  keypress = keypad_getKey();
		  //once a key is pressed return to GET_KEY
		  state = GET_KEY;

		  break;

	  case DISPLAY_ALARM:
		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");

		  UART_print("Your Alarm is set to: ");
		  //move left and down a line
		  UART_print("\033[1B\033[30D");

		  //display the current alarm hours set
		  data = RTC_read(ALARM_HOUR_ADDR);
		  //check the PM bit in the ALARM1 Hours register
		  if(data & (1<<5))
			  PM = 1;
		  //convert number to decimal and clear settings bits
		  data = HexToDec(data & 0x1F);
		  //convert to string and print
		  uint16ToString(data, result);
		  UART_print(result);
		  UART_print(":");

		  //display the current alarm min set
		  data = RTC_read(ALARM_MIN_ADDR);
		  //convert to decimal
		  data = HexToDec(data);
		  //convert to string and print
		  uint16ToString(data, result);
		  UART_print(result);
		  UART_print(":");


		  //display the current alarm sec set
		  data = RTC_read(ALARM_SEC_ADDR);
		  //convert tp decimal
		  data = HexToDec(data);
		  //convert to string
		  uint16ToString(data, result);
		  //print
		  UART_print(result);

		  //print AM or PM
		  if(PM)
			  UART_print(" PM");
		  else
			  UART_print(" AM");

		  //move cursor down 2 lines and left
		  UART_print("\033[2B\033[20D");
		  UART_print("Press any key to return to clock and enter a new command");
		  //wait for a key to be pressed
		  keypress = keypad_getKey();
		  while(keypress == -1)
			  keypress = keypad_getKey();

		  //reset PM
		  PM = 0;

		  //once a key is pressed return to GET_KEY
		  state = GET_KEY;

		  break;

	  case ALARM_INT:
		  //clear screen, reposition cursor, turn on bold text
		  UART_print("\033[2J\033[H\033[1m");
		  //print message
		  UART_print("WAKE UP!!!!! ");
		  //move cursor down a line and left
		  UART_print("\033[1B\033[30D");
		  UART_print("Press any key to turn off alarm and return to clock screen.");
		  //turn off bold text
		  UART_print("\033[0m");

		  //wait for a key to be pressed
		  keypress = keypad_getKey();
		  while(keypress == -1)
			  keypress = keypad_getKey();

		  //clear the ALARM flag
		  RTC_write(STATUS_REG, 0x00);

		  //once a key is pressed return to GET_KEY
		  state = GET_KEY;

		  break;

	  case SET_TIME:
		  //clear screen, reposition cursor
		  UART_print("\033[2J\033[H");

		  //input seconds
		  UART_print("Enter the current seconds (00-59)");
		  sec = keypad_input();
		  if(sec > 59)
			  sec = 0;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the minutes
		  UART_print("Enter the current minutes (00-59)");
		  min = keypad_input();
		  if(min > 59)
			  min = 0;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the hours
		  UART_print("Enter the current hours (00-12)");
		  hour = keypad_input();
		  if(hour > 12)
			  hour = 12;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input AM or PM
		  UART_print("01 for AM, 02 for PM");
		  PM = keypad_input();
		  //adjust for register setting
		  if(PM == 2)
			  PM = 1;
		  else
			  PM = 0;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the day
		  UART_print("Enter the day (01-07): 01 for Sunday");
		  day = keypad_input();
		  if(day > 7)
			  day = 1;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the date
		  UART_print("Enter the current date (00-31)");
		  date = keypad_input();
		  if(date > 31 || date < 1)
			  date = 1;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the month
		  UART_print("Enter the current month (01-12)");
		  month = keypad_input();
		  if(month > 12 || month < 1)
			  month = 1;

		  //clear screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the year
		  UART_print("Enter the current year (00-99)");
		  year = keypad_input();
		  if(year > 99)
			  year = 0;

		  //convert times to HEX
		  sec = DecToHex(sec);
		  min = DecToHex(min);
		  hour = DecToHex(hour);
		  day = DecToHex(day);
		  date = DecToHex(date);
		  month = DecToHex(month);
		  year = DecToHex(year);

		  //configure the hour register settings
		  if(PM)
			  hour |= 0x60;
		  else
			  hour |= 0x40;

		  //set the times to the RTC
		  RTC_setTime(sec, min, hour, day, date, month, year);

		  //reset PM
		  PM = 0;

		  //get a new cmd
		  state = GET_KEY;

		  break;

	  case SET_ALARM:

		  //Turn on INTCN and A1E in control register
		  RTC_write(CONTROL_REG, 0x05);
		  //delay for writes
		  for(int16_t i = 0; i < 10000; i++);
		  //clear alarm flag
		  RTC_write(STATUS_REG, 0x00);

		  //clear the screen and reposition cursor
		  UART_print("\033[2J\033[H");

		  //input the alarm seconds
		  UART_print("Enter the alarm seconds (00-59)");
		  sec = keypad_input();
		  if(sec > 59)
			  sec = 0;

		  //clear the screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the alarm minutes
		  UART_print("Enter the alarm minutes (00-59)");
		  min = keypad_input();
		  if(min > 59)
			  min = 0;

		  //clear the screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  //input the alarm hours
		  UART_print("Enter the alarm hours (00-12)");
		  hour = keypad_input();
		  if(hour > 12)
			  hour = 12;

		  //clear the screen and reposition cursor
		  UART_print("\033[2J\033[H");
		  UART_print("01 for AM, 02 for PM");
		  PM = keypad_input();
		  if(PM == 2)
			  PM = 1;
		  else
			  PM = 0;

		  //convert times to HEX
		  sec = DecToHex(sec);
		  min = DecToHex(min);
		  hour = DecToHex(hour);

		  //configure the hour
		  if(PM)
			  hour |= 0x60;
		  else
			  hour |= 0x40;


		  //RTC_config();
		  RTC_setAlarmTime(sec, min, hour);

		  //reset PM
		  PM = 0;

		  //get a new cmd
		  state = GET_KEY;

		  break;

	  default:
		  state = DISPLAY_TIME;
	  }
  }

}


uint8_t HexToDec(uint8_t data){
	return ((data / 16 * 10) + (data % 16));
}
uint8_t DecToHex(uint8_t data){
	return ((data / 10 * 16) + (data % 10));
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
