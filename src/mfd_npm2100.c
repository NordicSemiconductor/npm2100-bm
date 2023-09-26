/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "byteorder.h"
#include "i2c.h"
#include "mfd_npm2100.h"

#define TIMER_CONFIG	      0xB3U
#define TIMER_TARGET	      0xB4U
#define HIBERNATE_TASKS_HIBER 0xC8U
#define RESET_TASKS_RESET     0xD0U

#define TIMER_CONFIG_WKUP 3U

#define TIMER_PRESCALER_MS 16U
#define TIMER_MAX	   0xFFFFFFU

int mfd_npm2100_set_timer(void *dev, uint32_t time_ms)
{
	uint8_t buff[4] = {TIMER_TARGET};
	uint32_t ticks = time_ms / TIMER_PRESCALER_MS;

	if (ticks > TIMER_MAX) {
		return -EINVAL;
	}

	sys_put_be24(ticks, &buff[1]);

	return i2c_write(dev, buff, sizeof(buff));
}

int mfd_npm2100_reset(void *dev)
{
	return i2c_reg_write_byte(dev, RESET_TASKS_RESET, 1U);
}

int mfd_npm2100_hibernate(void *dev, uint32_t time_ms)
{
	int ret = mfd_npm2100_set_timer(dev, time_ms);

	if (ret != 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, HIBERNATE_TASKS_HIBER, 1U);
}
