#include "i2c-lcd.h"

I2C_HandleTypeDef *lcd_hi2c;
uint8_t lcd_addr;

void lcd_init(I2C_HandleTypeDef *hi2c, uint8_t address)
{
    lcd_hi2c = hi2c;
    lcd_addr = address;

	// 4 bit initialization
    HAL_Delay(50);   // wait for > 40ms
    lcd_send_cmd(0x30);
    HAL_Delay(5);	 // wait for > 4.1ms
    lcd_send_cmd(0x30);
    HAL_Delay(1);	 // wait for > 100us
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20);   // 4bit mode
    HAL_Delay(10);

	// Display initialization
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(2);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string_at (char *str, uint8_t row_ptr, uint8_t col_ptr)
{
	lcd_put_cur(row_ptr, col_ptr);
	while (*str) lcd_send_data (*str++);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

void lcd_put_cur(uint8_t row, uint8_t col)
{
	switch (row)
	{
		case 0:
			col |= 0x80;
			break;
		case 1:
			col |= 0xC0;
			break;
		case 2:
			col |= (0x80 + 0x14);
			break;
		case 3:
			col |= (0x80 + 0x54);
			break;
		default:
			col |= 0x80;
	}
	lcd_send_cmd (col);
}

void lcd_clear (void)
{
	 lcd_send_cmd (0x01);  // clear display
	 HAL_Delay(2);
}

void lcd_send_cmd (uint8_t cmd)
{
	uint8_t data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[1] = data_u|0x08;  //en=0, rs=0 -> bxxxx1000
	data_t[2] = data_l|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[3] = data_l|0x08;  //en=0, rs=0 -> bxxxx1000
	HAL_I2C_Master_Transmit (lcd_hi2c, lcd_addr, (uint8_t *) data_t, 4, 100);
}

void lcd_send_data (uint8_t data)
{
	uint8_t data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0 -> bxxxx1101
	data_t[1] = data_u|0x09;  //en=0, rs=0 -> bxxxx1001
	data_t[2] = data_l|0x0D;  //en=1, rs=0 -> bxxxx1101
	data_t[3] = data_l|0x09;  //en=0, rs=0 -> bxxxx1001
	HAL_I2C_Master_Transmit (lcd_hi2c, lcd_addr, (uint8_t *) data_t, 4, 100);
}
