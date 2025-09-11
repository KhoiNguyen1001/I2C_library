# I2C Scanner simple library for STM32 (HAL)
- Supports finding I2C address.
- All found I2C addresses will be saved in array i2c_address, you can enter debug mode or use any ways of communication between MCU and your computer to know its values.
---

## Configuration
- Change the HAL include line in i2c_lcd.h to match your target MCU:

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series
- Init all things in STM32MXCube or init I2C by yourself in your code. 
---

### 1. Add Files
- Copy `I2C_scanner.c` and `I2C_scanner.h` into your project.

### 2. Include Header
- In your application code:  #include "i2c_lcd.h"

### 3. Begin scanning
	I2C_Scan(&hi2c1); 		// hi2c1 must be initialized in CubeMX



