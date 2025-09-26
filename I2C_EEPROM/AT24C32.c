/* Includes ------------------------------------------------------------------*/
#include "AT24C32.h"

void AT24C32_init(AT24C32_HandleTypeDef *AT24C32_Handle, I2C_HandleTypeDef *hi2c, uint8_t AT24C32_ADDR)
{
	AT24C32_Handle->hi2c = hi2c;
	AT24C32_Handle->AT24C32_addr = AT24C32_ADDR;
}

HAL_StatusTypeDef AT24C32_WriteByte(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t data) {
    uint8_t buf[3];
    buf[0] = (uint8_t)(memAddr >> 8);   // High address
    buf[1] = (uint8_t)(memAddr & 0xFF); // Low address
    buf[2] = data;
    return HAL_I2C_Master_Transmit(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, buf, 3, HAL_MAX_DELAY);
}

HAL_StatusTypeDef AT24C32_ReadByte(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data) {
    uint8_t addr[2];
    addr[0] = (uint8_t)(memAddr >> 8);
    addr[1] = (uint8_t)(memAddr & 0xFF);
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, addr, 2, HAL_MAX_DELAY);
    if (status != HAL_OK) return status;
    return HAL_I2C_Master_Receive(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef AT24C32_WritePage(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data, uint16_t len) {
    if (len > AT24C32_PAGE_SIZE) len = AT24C32_PAGE_SIZE;
    uint8_t buf[2 + AT24C32_PAGE_SIZE];
    buf[0] = (uint8_t)(memAddr >> 8);
    buf[1] = (uint8_t)(memAddr & 0xFF);
    for (uint16_t i = 0; i < len; i++) {
        buf[2 + i] = data[i];
    }
    return HAL_I2C_Master_Transmit(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, buf, len + 2, HAL_MAX_DELAY);
}

HAL_StatusTypeDef AT24C32_ReadBuffer(AT24C32_HandleTypeDef *AT24C32_Handle, uint16_t memAddr, uint8_t *data, uint16_t len) {
    uint8_t addr[2];
    addr[0] = (uint8_t)(memAddr >> 8);
    addr[1] = (uint8_t)(memAddr & 0xFF);
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, addr, 2, HAL_MAX_DELAY);
    if (status != HAL_OK) return status;
    return HAL_I2C_Master_Receive(AT24C32_Handle->hi2c, AT24C32_Handle->AT24C32_addr, data, len, HAL_MAX_DELAY);
}
