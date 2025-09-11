# I2C Scanner simple library for STM32 (HAL)
- A simple utility for scanning all possible I2C device addresses on STM32 microcontrollers using the HAL library.
- Detected addresses are stored in the global i2c_address[] array. You can inspect them via a debugger or send them out over UART/USB for logging.

# Features
- Scans the I2C bus for connected devices.
- Saves found I2C addresses into a global array.
- Works with any STM32 series that supports HAL.

## Configuration
Update the HAL include in I2C_scanner.h for your target MCU family.

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series

Initialize your I2C peripheral in CubeMX or manually in your code before calling the scanner.

### 1. Add Files
Copy `I2C_scanner.c` and `I2C_scanner.h` into your project.

### 2. Include Header
In your application code:  #include "I2C_scanner.h"

### 3. Run the Scanner
Call the function after initializing I2C.

	I2C_Scan(&hi2c1);		// hi2c1 must be initialized in CubeMX
