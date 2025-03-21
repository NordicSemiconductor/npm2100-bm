/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nrfx_twim.h>

#include "i2c.h"

static const nrfx_twim_t twim = NRFX_TWIM_INSTANCE(0);
static nrfx_twim_config_t twim_config = NRFX_TWIM_DEFAULT_CONFIG;

int i2c_init(void *dev, uint8_t sda_pin, uint8_t scl_pin)
{
	(void)dev;
	twim_config.sda = sda_pin;
	twim_config.scl = scl_pin;

	nrfx_err_t err = nrfx_twim_init(&twim, &twim_config, NULL, NULL);
	if (err != NRFX_SUCCESS) {
		return -err;
	}

	nrfx_twim_enable(&twim);

	return 0;
}

int i2c_write(void *dev, uint8_t *buf, size_t len)
{
	struct i2c_dev *i2c = (struct i2c_dev *)dev;
	nrfx_twim_xfer_desc_t write_desc = {
		.type = NRFX_TWIM_XFER_TX,
		.address = i2c->addr,
		.primary_length = len,
		.secondary_length = 0,
		.p_primary_buf = buf,
		.p_secondary_buf = NULL
	};

	nrfx_err_t err = nrfx_twim_xfer(&twim, &write_desc, 0U);
	
	if (err != NRFX_SUCCESS) {
		return -err;
	}

	return 0;
}

int i2c_read(void *dev, uint8_t reg, uint8_t *buf, size_t len)
{
	struct i2c_dev *i2c = (struct i2c_dev *)dev;
	// TX-RX transfer type is not supported in blocking mode
	nrfx_twim_xfer_desc_t write_desc = {
		.type = NRFX_TWIM_XFER_TX,
		.address = i2c->addr,
		.primary_length = 1,
		.secondary_length = 0,
		.p_primary_buf = &reg,
		.p_secondary_buf = NULL
	};
	nrfx_twim_xfer_desc_t read_desc = {
		.type = NRFX_TWIM_XFER_RX,
		.address = i2c->addr,
		.primary_length = len,
		.secondary_length = 0,
		.p_primary_buf = buf,
		.p_secondary_buf = NULL
	};

	nrfx_err_t err = nrfx_twim_xfer(&twim, &write_desc, 0U);
	err = nrfx_twim_xfer(&twim, &read_desc, 0U);
	
	if (err != NRFX_SUCCESS) {
		return -err;
	}

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
