#ifndef I2C_LCD_H
#define I2C_LCD_H

#include "stm32f4xx_hal.h"  //	Target MCU family (changeable)

extern I2C_HandleTypeDef *lcd_hi2c;
extern uint8_t lcd_addr;

// Handle and address are configurable by user
void lcd_init(I2C_HandleTypeDef *hi2c, uint8_t address);
void lcd_send_string_at (char *str, uint8_t row_ptr, uint8_t col_ptr);

// Optional helpers
void lcd_clear(void);
void lcd_put_cur(uint8_t row, uint8_t col);
void lcd_send_string(char *str);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);

#endif
