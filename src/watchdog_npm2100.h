/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef WATCHDOG_NPM2100_H_
#define WATCHDOG_NPM2100_H_

#include <stdint.h>

/* nPM2100 watchdog mode enumeration */
enum watchdog_npm2100_mode {
	NPM2100_WATCHDOG_RESET_NONE = 0,  //!< Watchdog expiry causes event only
	NPM2100_WATCHDOG_RESET_PIN = 1,	  //!< Watchdog expiry strobes pin output
	NPM2100_WATCHDOG_RESET_POWER = 2, //!< Watchdog expiry causes full power cycle of PMIC
};

/**
 * @brief Disable watchdog and boot monitor
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm2100_disable(void *dev);

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
int watchdog_npm2100_init(void *dev, uint32_t timeout_ms, enum watchdog_npm2100_mode mode);

/**
 * @brief Feed watchdog
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm2100_feed(void *dev);

#endif /* GPIO_NPM2100_H_ */
