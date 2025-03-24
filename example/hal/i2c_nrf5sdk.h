/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <nrfx_twim.h>

#include "i2c.h"

#ifndef I2C_NRF5SDK_H
#define I2C_NRF5SDK_H

struct i2c_ctx {
	nrfx_twim_t twim;
	nrfx_twim_config_t twim_config;
};

int i2c_init(struct i2c_dev *dev, nrfx_twim_t *twim_inst, uint8_t sda_pin, uint8_t scl_pin);

#endif // I2C_NRF5SDK_H
