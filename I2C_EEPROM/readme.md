# I2C EEPROM Library for STM32 (HAL)
- This library provides functions to interface with the AT24C32 EEPROM using the STM32 HAL I2C driver.

## Features
- Initialize the AT24C32 via I2C.
- Read and write single byte or bytes
  
## Configuration
Change the HAL include line in AT24C32.h to match your target MCU.

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series

### 1. Add Files
Copy `AT24C32.c` and `AT24C32.h` into your STM32CubeIDE project.

### 2. Include Header
In your application code:  

	#include "AT24C32.h"

### 3. Declare Variables
Create a AT24C32 handler, as either local or global variables:

	AT24C32_HandleTypeDef AT24C32_Handler;

### 4. Initialize and Use
	
	AT24C32_Init(&AT24C32_Handler, &hi2c1, 0x57 << 1);   // hi2c1 must be initialized in CubeMX; 0x57 is the I2C address of the AT24C32
	
	uint8_t eeprom_data[4];
	
	// Write a single byte
	AT24C32_WriteByte(&AT24C32_Handler, 0x00, 0xF0);
	
	// Read a single byte
	AT24C32_ReadByte(&AT24C32_Handler, 0x00, &eeprom_data[0]);
	
	// Read multiple bytes into buffer
	AT24C32_ReadBuffer(&AT24C32_Handler, 0x00, eeprom_data, sizeof(eeprom_data));
	
	// Write multiple bytes to one page
	uint8_t eeprom_write[4] = {0x04, 0x05, 0x06, 0x07};
	AT24C32_WritePage(&AT24C32_Handler, 0x04, eeprom_write, sizeof(eeprom_write));

