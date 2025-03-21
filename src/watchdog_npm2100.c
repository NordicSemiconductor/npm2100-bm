/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "errno.h"
#include "i2c.h"
#include "mfd_npm2100.h"
#include "watchdog_npm2100.h"

#define TIMER_TASKS_START 0xB0U
#define TIMER_TASKS_STOP  0xB1U
#define TIMER_TASKS_KICK  0xB2U
#define TIMER_CONFIG	  0xB3U
#define TIMER_TARGET_HI	  0xB4U
#define TIMER_TARGET_MID  0xB5U
#define TIMER_TARGET_LO	  0xB6U
#define TIMER_STATUS	  0xB7U
#define TIMER_BOOT_MON	  0xB8U

#define TIMER_CONFIG_GEN    0U
#define TIMER_CONFIG_WDRST  1U
#define TIMER_CONFIG_WDPWRC 2U

int watchdog_npm2100_disable(void *dev)
{
	return mfd_npm2100_stop_timer(dev);
}

int watchdog_npm2100_init(void *dev, uint32_t timeout_ms, enum watchdog_npm2100_mode mode)
{
	enum mfd_npm2100_timer_mode timer_mode;

	switch (mode) {
	case NPM2100_WATCHDOG_PIN_RESET:
		timer_mode = NPM2100_TIMER_MODE_WDT_RESET;
		break;
	case NPM2100_WATCHDOG_POWER_CYCLE:
		timer_mode = NPM2100_TIMER_MODE_WDT_POWER_CYCLE;
		break;
	default:
		return -EINVAL;
	}

	int ret = mfd_npm2100_set_timer(dev, timeout_ms, timer_mode);
	if (ret < 0) {
		return ret;
	}

	return mfd_npm2100_start_timer(dev);
}

int watchdog_npm2100_feed(void *dev)
{
	return i2c_reg_write_byte(dev, TIMER_TASKS_KICK, 1U);
}
