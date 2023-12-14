/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "i2c.h"

int i2c_write(void *dev, uint8_t *buf, size_t len)
{
	struct i2c_dev *i2c = (struct i2c_dev *)dev;

	memcpy(&i2c->mem[buf[0]], &buf[1], len - 1U);

	printf("W: %02X | %02X |", i2c->addr, buf[0]);
	for (size_t idx = 1U; idx < len; idx++) {
		printf(" %02X", buf[idx]);
	}
	printf("\n");

	return 0;
}

int i2c_read(void *dev, uint8_t reg, uint8_t *buf, size_t len)
{
	struct i2c_dev *i2c = (struct i2c_dev *)dev;

	memcpy(buf, &i2c->mem[reg], len);

	printf("R: %02X | %02X |", i2c->addr, reg);
	for (size_t idx = 0U; idx < len; idx++) {
		printf(" %02X", buf[idx]);
	}
	printf("\n");

	return 0;
}

int i2c_reg_write_byte(void *dev, uint8_t reg, uint8_t data)
{
	return i2c_write(dev, (uint8_t[]){reg, data}, 2U);
}

int i2c_reg_read_byte(void *dev, uint8_t reg, uint8_t *data)
{
	return i2c_read(dev, reg, data, 1U);
}

int i2c_reg_update_byte(void *dev, uint8_t reg, uint8_t mask, uint8_t data)
{
	uint8_t byte;
	int ret = i2c_reg_read_byte(dev, reg, &byte);

	if (ret < 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, reg, (byte & ~mask) | (data & mask));
}
