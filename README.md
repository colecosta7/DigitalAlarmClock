# DigitalAlarmClock
The digital alarm clock designed in this experiment utilizes a Nucleo-L476RG board [1-2], a DS3231 Real Time Clock Module [3], a buzzer, a 4x4 hard-key keypad, and a UART terminal. 
The digital alarm clock is organized into a finite state machine consisting of 7 states. The clock features 4 different commands input from the keypad: set the current time/date, 
set an alarm, display a list of commands, and display the current alarm. Upon connecting to power, the clock and alarm are not set, therefore the initial time and alarm set display 
incorrect values. This means that upon disconnecting from power, the current time/date and alarms are reset. The initial state of the device is the GET_KEY state.

//System Schematic
![P3Schematic](https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/1b88e920-25b5-4c80-8180-06e934fae376)

//Digital Alarm Clock FSM
![P3FSM (1)](https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/8b80d360-15fb-44aa-bce8-3a38e5b3d99f)


//UART Terminal Screens
//Clock Display
<img width="405" alt="P3ClockDisplay" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/27715f1c-f511-4b57-baaf-585072c09082">

//Set Clock Display
<img width="405" alt="P3SetClockScreen" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/747a7e5b-d1e7-4d99-824f-e3844d0d1dad">

//Set Alarm Display
<img width="405" alt="P3AlarmSetScreen" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/990ae955-225d-494f-b7ed-822d5a637c51">

//Commands Display
<img width="405" alt="P3CMDScreen" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/dc2e714f-f840-4bd4-8f34-2e20d305bdb4">

//Alarm Going Off Display
<img width="481" alt="P3Alarm" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/00e26dce-447e-45ac-bd8a-25ad33524fe2">

//Alarm Display
<img width="405" alt="P3AlarmDisplay" src="https://github.com/colecosta7/DigitalAlarmClock/assets/81593806/069f5196-2a37-4ed5-98b8-c4dffec45a5d">
