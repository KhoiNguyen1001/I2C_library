#ifndef AT24C32_H
#define AT24C32_H

#include <stdint.h>
#include "stm32f4xx_hal.h"   // or your MCU HAL

#define AT24C32_PAGE_SIZE 32          // 32 bytes per page
#define AT24C32_SIZE      4096        // 4KB total memory

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t AT24C32_addr;
} AT24C32_HandleTypeDef;

void AT24Cxx_init(AT24C32_HandleTypeDef *AT24C32_Handle, I2C_HandleTypeDef *hi2c, uint8_t AT24C32_ADDR);

HAL_StatusTypeDef AT24C32_WriteByte(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t data);
HAL_StatusTypeDef AT24C32_ReadByte(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data);

HAL_StatusTypeDef AT24C32_WritePage(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data, uint16_t len);
HAL_StatusTypeDef AT24C32_ReadBuffer(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data, uint16_t len);

#endif
