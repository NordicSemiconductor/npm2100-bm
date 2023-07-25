/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include "byteorder.h"
#include "i2c.h"
#include "mfd_npm1300.h"

#define TIME_BASE 0x07U

#define TIME_OFFSET_LOAD  0x03U
#define TIME_OFFSET_TIMER 0x08U

#define TIMER_PRESCALER_MS 16U
#define TIMER_MAX          0xFFFFFFU

int mfd_npm1300_reg_read_burst(void *dev, uint8_t base, uint8_t offset, void *data, size_t len)
{
	uint8_t buff[] = {base, offset};

	return i2c_write_read(dev, buff, sizeof(buff), data, len);
}

int mfd_npm1300_reg_read(void *dev, uint8_t base, uint8_t offset, uint8_t *data)
{
	return mfd_npm1300_reg_read_burst(dev, base, offset, data, 1U);
}

int mfd_npm1300_reg_write(void *dev, uint8_t base, uint8_t offset, uint8_t data)
{
	uint8_t buff[] = {base, offset, data};

	return i2c_write(dev, buff, sizeof(buff));
}

int mfd_npm1300_reg_write2(void *dev, uint8_t base, uint8_t offset, uint8_t data1, uint8_t data2)
{
	uint8_t buff[] = {base, offset, data1, data2};

	return i2c_write(dev, buff, sizeof(buff));
}

int mfd_npm1300_reg_update(void *dev, uint8_t base, uint8_t offset, uint8_t data, uint8_t mask)
{
	uint8_t reg = 0U;

	int ret = mfd_npm1300_reg_read(dev, base, offset, &reg);

	if (ret == 0) {
		reg = (reg & ~mask) | (data & mask);
		ret = mfd_npm1300_reg_write(dev, base, offset, reg);
	}

	return ret;
}

int mfd_npm1300_set_timer(void *dev, uint32_t time_ms)
{
	uint8_t buff[5] = {TIME_BASE, TIME_OFFSET_TIMER};
	uint32_t ticks = time_ms / TIMER_PRESCALER_MS;

	if (ticks > TIMER_MAX) {
		return -EINVAL;
	}

	sys_put_be24(ticks, &buff[2]);

	int ret = i2c_write(dev, buff, sizeof(buff));

	if (ret != 0) {
		return ret;
	}

	return mfd_npm1300_reg_write(dev, TIME_BASE, TIME_OFFSET_LOAD, 1U);
}
