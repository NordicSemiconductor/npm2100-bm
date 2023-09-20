/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "i2c.h"

int i2c_reg_write_byte(void *dev, uint8_t reg, uint8_t data)
{
	(void)dev;

	printf("W: %02X | %02X\n", reg, data);

	return 0;
}

int i2c_reg_read_byte(void *dev, uint8_t reg, uint8_t *data)
{
	(void)dev;

	printf("R: %02X | 1 byte\n", reg);

	*data = 0U;

	return 0;
}

int i2c_reg_update_byte(void *dev, uint8_t reg, uint8_t mask, uint8_t data)
{
	(void)dev;

	printf("U: %02X | %02X | %02X\n", reg, mask, data);

	return 0;
}
