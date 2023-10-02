/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "i2c.h"

int i2c_reg_write_byte(void *dev, uint8_t reg, uint8_t data)
{
	printf("W: %02X | %02X | %02X\n", ((struct i2c_dev *)dev)->addr, reg, data);

	return 0;
}

int i2c_reg_read_byte(void *dev, uint8_t reg, uint8_t *data)
{
	printf("R: %02X | %02X - 1 byte\n", ((struct i2c_dev *)dev)->addr, reg);

	*data = 0U;

	return 0;
}

int i2c_reg_update_byte(void *dev, uint8_t reg, uint8_t mask, uint8_t data)
{
	printf("U: %02X | %02X | %02X | %02X\n", ((struct i2c_dev *)dev)->addr, reg, mask, data);

	return 0;
}

int i2c_write(void *dev, uint8_t *buf, size_t len)
{
	printf("W: %02X | %02X |", ((struct i2c_dev *)dev)->addr, buf[0]);

	for (size_t idx = 1U; idx < len; idx++) {
		printf(" %02X", buf[idx]);
	}

	printf("\n");

	return 0;
}
