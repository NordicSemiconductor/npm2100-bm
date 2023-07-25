/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef WATCHDOG_NPM1300_H_
#define WATCHDOG_NPM1300_H_

#include <stdint.h>

/* nPM1300 watchdog mode enumeration */
enum watchdog_npm1300_mode {
	NPM1300_WATCHDOG_RESET_GPIO = 1,  //!< Watchdog expiry strobes GPIO RESET output
	NPM1300_WATCHDOG_RESET_POWER = 2, //!< Watchdog expiry causes full power cycle of PMIC
	NPM1300_WATCHDOG_RESET_NONE = 3	  //!< Watchdog expiry causes event only
};

/**
 * @brief Disable watchdog
 *
 * @param dev i2c device
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm1300_disable(void *dev);

/**
 * @brief Initialise watchdog
 *
 * Install timer, and start watchdog in specified mode
 *
 * @param dev i2c device
 * @param timeout_ms Watchdog timeout in ms
 * @param mode Watchdog expiry behaviour
 * @return 0 If successful, -EINVAL if time is out of range, -errno In case of any bus error
 */
int watchdog_npm1300_init(void *dev, uint32_t timeout_ms, enum watchdog_npm1300_mode mode);

/**
 * @brief Feed watchdog
 *
 * @param dev i2c device
 * @return 0 If successful, -errno In case of any bus error
 */
int watchdog_npm1300_feed(void *dev);

#endif /* GPIO_NPM1300_H_ */
