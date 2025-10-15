# I2C Real-Time Clock Library for STM32 (HAL)
- This library provides functions to interface with the DS3231 real-time clock (RTC) module using the STM32 HAL I2C driver.

## Features
- Initialize the DS3231 via I2C
- Read and set time and date
- Read temperature (°C)
  
## Configuration
Change the HAL include line in DS3231.h to match your target MCU.

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series

### 1. Add Files
Copy `DS3231.c` and `DS3231.h` into your STM32CubeIDE project.

### 2. Include Header
In your application code:  

	#include "DS3231.h"

### 3. Declare Variables
Create a DS3231 handler and a time/date structure as either local or global variables:

	DS3231_HandleTypeDef DS3231_Handler;
  DS3231_TimeDate td;

### 4. Initialize and Use
	
	DS3231_Init(&DS3231_Handler, &hi2c, 0x68 << 1);   // hi2c1 must be initialized in CubeMX, 0x68 is the I2C address of DS3231.
	
	memset(&td,0,sizeof(td));
	td.day = 10;
	td.dayOfWeek = 5;
	td.hours = 10;
	td.year = 2025;
	
	DS3231_SetTimeDate(&DS3231_Handler, &td);
	DS3231_GetTimeDate(&DS3231_Handler, &td);
	
	float temperature;
	DS3231_GetTemperature(&DS3231_Handler, &temperature);   // returns temperature in °C
