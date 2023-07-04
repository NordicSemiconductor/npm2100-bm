/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LED_NPM1300_H_
#define LED_NPM1300_H_

#include <stdint.h>

/* nPM1300 LED mode enumeration */
enum led_npm1300_mode {
	NPM_LED_MODE_ERROR,
	NPM_LED_MODE_CHARGING,
	NPM_LED_MODE_HOST
};

/**
 * Turn on specified LED
 *
 * @param dev Device pointer
 * @param led LED to turn on
 * @return 0 on success, -errno on failure
 */
int led_npm1300_on(void *dev, uint32_t led);

/**
 * Turn off specified LED
 *
 * @param dev Device pointer
 * @param led LED to turn on
 * @return 0 on success, -errno on failure
 */
int led_npm1300_off(void *dev, uint32_t led);

/**
 * Initialise specified LED
 *
 * @param dev Device pointer
 * @param led LED to initialise
 * @param mode LED mode to set
 * @return 0 on success, -errno on failure
 */
int led_npm1300_init(void *dev, uint32_t led, enum led_npm1300_mode mode);

#endif /* LED_NPM1300_H_ */
