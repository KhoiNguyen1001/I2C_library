#include "I2C_LCD.h"
#include "string.h"

/* Private helpers */
static void LCD_SendCmd(LCD_HandleTypeDef *lcd, uint8_t cmd);
static void LCD_SendData(LCD_HandleTypeDef *lcd, uint8_t data);

void LCD_Init(LCD_HandleTypeDef *lcd, I2C_HandleTypeDef *hi2c, uint8_t address)
{
    lcd->hi2c = hi2c;
    lcd->address = address;

    HAL_Delay(50);   // wait for >40ms
    LCD_SendCmd(lcd, 0x30);
    HAL_Delay(5);
    LCD_SendCmd(lcd, 0x30);
    HAL_Delay(1);
    LCD_SendCmd(lcd, 0x30);
    HAL_Delay(10);
    LCD_SendCmd(lcd, 0x20);  // 4-bit mode
    HAL_Delay(10);

    LCD_SendCmd(lcd, 0x28);  // Function set: 4-bit, 2 lines, 5x8 font
    HAL_Delay(1);
    LCD_SendCmd(lcd, 0x08);  // Display OFF
    HAL_Delay(1);
    LCD_SendCmd(lcd, 0x01);  // Clear display
    HAL_Delay(2);
    LCD_SendCmd(lcd, 0x06);  // Entry mode set
    HAL_Delay(1);
    LCD_SendCmd(lcd, 0x0C);  // Display ON, cursor OFF, blink OFF
}

void LCD_SendStringAt(LCD_HandleTypeDef *lcd, char *str, uint8_t row, uint8_t col)
{
    LCD_PutCur(lcd, row, col);
    while (*str) LCD_SendData(lcd, *str++);
}

void LCD_SendString(LCD_HandleTypeDef *lcd, char *str)
{
    while (*str) LCD_SendData(lcd, *str++);
}

void LCD_PutCur(LCD_HandleTypeDef *lcd, uint8_t row, uint8_t col)
{
    switch (row)
    {
        case 0: col |= 0x80; break;
        case 1: col |= 0xC0; break;
        case 2: col |= (0x80 + 0x14); break;
        case 3: col |= (0x80 + 0x54); break;
        default: col |= 0x80; break;
    }
    LCD_SendCmd(lcd, col);
}

void LCD_Clear(LCD_HandleTypeDef *lcd)
{
    LCD_SendCmd(lcd, 0x01);
    HAL_Delay(2);
}

/* Private helpers */
static void LCD_SendCmd(LCD_HandleTypeDef *lcd, uint8_t cmd)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xF0);
    data_l = ((cmd << 4) & 0xF0);
    data_t[0] = data_u | 0x0C;
    data_t[1] = data_u | 0x08;
    data_t[2] = data_l | 0x0C;
    data_t[3] = data_l | 0x08;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, data_t, 4, 100);
}

static void LCD_SendData(LCD_HandleTypeDef *lcd, uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = (data & 0xF0);
    data_l = ((data << 4) & 0xF0);
    data_t[0] = data_u | 0x0D;
    data_t[1] = data_u | 0x09;
    data_t[2] = data_l | 0x0D;
    data_t[3] = data_l | 0x09;
    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, data_t, 4, 100);
}
