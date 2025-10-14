# I2C LCD library for STM32 (HAL)
- Supports HD44780-compatible LCDs with an I2C expander (PCF8574).

## Features
- Initialize LCD via I2C
- Print strings at current or specified cursor location
- Clear display
- Send raw commands or data to LCD
- Works with STM32 HAL (`stm32fxxx_hal.h`)

## Configuration
Change the HAL include line in I2C_LCD.h to match your target MCU.

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series

### 1. Add Files
Copy `I2C_LCD.c` and `I2C_LCD.h` into your STM32CubeIDE project.

### 2. Include Header
In your application code:  

	#include "I2C_LCD.h"

### 3. Declare a struct variable
Declare a variable of type LCD_HandleTypeDef as a local or global variable.

	LCD_HandleTypeDef lcd_handler;

### 4. Initialize LCD

	LCD_Init(&lcd_handler, &hi2c1, 0x27<<1);   // hi2c1 must be initialized in CubeMX, 0x27 is the I2C address of I2C module.
	LCD_Clear(&lcd_handler);
	LCD_PutCur(&lcd_handler, 0, 0);
	LCD_SendString(&lcd_handler, "Hello, World!");
	LCD_SendStringAt(&lcd_handler, "STM32 LCD", 1, 0);  // row 1, col 0
