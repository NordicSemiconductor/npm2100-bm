/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "watchdog_npm1300.h"
#include "mfd_npm1300.h"

/* nPM1300 TIMER base address */
#define TIME_BASE 0x07U

/* nPM1300 timer register offsets */
#define TIME_OFFSET_START     0x00U
#define TIME_OFFSET_STOP      0x01U
#define TIME_OFFSET_WDOG_KICK 0x04U
#define TIME_OFFSET_MODE      0x05U

int watchdog_npm1300_disable(void *dev)
{
	return mfd_npm1300_reg_write(dev, TIME_BASE, TIME_OFFSET_STOP, 1U);
}

int watchdog_npm1300_init(void *dev, uint32_t timeout_ms, enum watchdog_npm1300_mode mode)
{
	int ret = mfd_npm1300_set_timer(dev, timeout_ms);
	if (ret < 0) {
		return ret;
	}

	ret = mfd_npm1300_reg_write(dev, TIME_BASE, TIME_OFFSET_MODE, mode);
	if (ret < 0) {
		return ret;
	}

	return mfd_npm1300_reg_write(dev, TIME_BASE, TIME_OFFSET_START, 1U);
}

int watchdog_npm1300_feed(void *dev)
{
	return mfd_npm1300_reg_write(dev, TIME_BASE, TIME_OFFSET_WDOG_KICK, 1U);
}
