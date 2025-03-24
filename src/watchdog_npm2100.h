/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WATCHDOG_NPM2100_H_
#define WATCHDOG_NPM2100_H_

#include <stdint.h>

#include "i2c.h"

/* nPM2100 watchdog mode enumeration */
enum watchdog_npm2100_mode {
	NPM2100_WATCHDOG_PIN_RESET = 1,   //!< Watchdog expiry generates a host reset on PG/RESET
	NPM2100_WATCHDOG_POWER_CYCLE = 2, //!< Watchdog expiry causes full power cycle of PMIC
};

/**
 * @brief Disable watchdog and boot monitor
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm2100_disable(struct i2c_dev *dev);

/**
 * @brief Initialise watchdog
 *
 * Install timer, and start watchdog in specified mode
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param timeout_ms Watchdog timeout in ms
 * @param mode Watchdog expiry behaviour
 * @return 0 If successful, -EINVAL if time is out of range, -errno In case of any bus error
 */
int watchdog_npm2100_init(struct i2c_dev *dev, uint32_t timeout_ms, enum watchdog_npm2100_mode mode);

/**
 * @brief Feed watchdog
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm2100_feed(struct i2c_dev *dev);

#endif /* GPIO_NPM2100_H_ */
