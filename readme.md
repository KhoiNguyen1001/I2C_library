# I2C LCD library for STM32 (HAL)
- Supports HD44780-compatible LCDs with an I2C expander (PCF8574).

---

## Features
- Initialize LCD via I2C
- Print strings at current or specified cursor location
- Clear display
- Send raw commands or data to LCD
- Works with STM32 HAL (`stm32fxxx_hal.h`)

---

## Configuration
Change the HAL include line in i2c_lcd.h to match your target MCU.

	#include "stm32f4xx_hal.h"   // e.g., stm32f1xx_hal.h for F1 series

---

### 1. Add Files
- Copy `i2c_lcd.c` and `i2c_lcd.h` into your STM32CubeIDE project.

### 2. Include Header
- In your application code:  #include "i2c_lcd.h"


### 3. Initialize LCD

	lcd_init(&hi2c1, 0x27);   // hi2c1 must be initialized in CubeMX
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Hello, World!");
	lcd_send_string_at("STM32 LCD", 1, 0);  // row 1, col 0
