/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "i2c.h"
#include "gpio_npm2100.h"

#define NPM2100_GPIO_CONFIG 0x80U
#define NPM2100_GPIO_USAGE  0x83U
#define NPM2100_GPIO_OUTPUT 0x86U
#define NPM2100_GPIO_READ   0x89U

#define NPM2100_GPIO_PINS 2U

int gpio_npm2100_set(void *dev, uint8_t pin, bool state)
{
	if (pin >= NPM2100_GPIO_PINS) {
		return -EINVAL;
	}

	return i2c_reg_write_byte(dev, NPM2100_GPIO_OUTPUT + pin, state);
}

int gpio_npm2100_get(void *dev, uint8_t pin, bool *state)
{
	uint8_t data;

	if (pin >= NPM2100_GPIO_PINS) {
		return -EINVAL;
	}

	int ret = i2c_reg_read_byte(dev, NPM2100_GPIO_READ, &data);
	if (ret < 0) {
		return ret;
	}

	*state = (data & (1U << pin)) != 0U;

	return 0;
}

int gpio_npm2100_config(void *dev, uint8_t pin, uint8_t mode, uint8_t flags)
{
	if (pin >= NPM2100_GPIO_PINS) {
		return -EINVAL;
	}

	int ret = i2c_reg_write_byte(dev, NPM2100_GPIO_USAGE + pin, mode);
	if (ret < 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, NPM2100_GPIO_CONFIG + pin, flags);
}
