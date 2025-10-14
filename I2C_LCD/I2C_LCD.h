/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_LCD_H
#define I2C_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"   // or your MCU HAL

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint8_t address;
} LCD_HandleTypeDef;

/* Public API */
void LCD_Init(LCD_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c, uint8_t address);
void LCD_SendStringAt(LCD_HandleTypeDef *lcd, char *str, uint8_t row, uint8_t col);
void LCD_PutCur(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col);
void LCD_SendString(LCD_HandleTypeDef *lcd, char *str);
void LCD_Clear(LCD_HandleTypeDef *lcd);

#ifdef __cplusplus
}
#endif

#endif /* I2C_LCD_H */
