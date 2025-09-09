#ifndef I2C_SCANNER_H
#define I2C_SCANNER_H

#include "stm32f4xx_hal.h"   // Change this to match your MCU family

extern uint8_t i2c_address[127];

uint8_t I2C_Scan(I2C_HandleTypeDef *hi2c);

#endif // I2C_SCANNER_H