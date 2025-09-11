#include "I2C_scanner.h"
#include <stdbool.h>
#include <stdio.h>   // for printf

uint8_t i2c_address[127];

uint8_t I2C_Scan(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef result;
    uint8_t i, temp = 0;
    //printf("Scanning I2C bus...\r\n");

    for (i = 1; i < 127; i++)
    {
		i2c_address[i-1] = 0x00;
        /*
         * The HAL_I2C_IsDeviceReady function checks
         * if a device responds at the given address.
         * Timeout is set to 10 ms here.
         */
        result = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i << 1), 1, 500);

        if (result == HAL_OK)
        {
			i2c_address[temp++] = i<<1;
            //printf("Device found at 0x%02X\r\n", i << 1);
        }
    }

	if(temp > 0)
		return temp;
	else
		return false;
    //printf("Scan complete.\r\n");
}
