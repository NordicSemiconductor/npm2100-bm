/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPIO_NPM1300_H_
#define GPIO_NPM1300_H_

#include <stdint.h>

/* nPM1300 GPIO mode enumeration */
enum gpio_npm1300_mode {
	NPM1300_GPIO_GPIINPUT,
	NPM1300_GPIO_GPILOGIC1,
	NPM1300_GPIO_GPILOGIC0,
	NPM1300_GPIO_GPIEVENTRISE,
	NPM1300_GPIO_GPIEVENTFALL,
	NPM1300_GPIO_GPOIRQ,
	NPM1300_GPIO_GPORESET,
	NPM1300_GPIO_GPOPWRLOSSWARN,
	NPM1300_GPIO_GPOLOGIC1,
	NPM1300_GPIO_GPOLOGIC0
};

/* nPM1300 GPIO flags */
#define NPM1300_GPIO_OPEN_DRAIN 1U
#define NPM1300_GPIO_PULLUP	2U
#define NPM1300_GPIO_PULLDOWN	3U
#define NPM1300_GPIO_DRIVE_6MA	4U
#define NPM1300_GPIO_DEBOUNCE	5U

int gpio_npm1300_config(void *dev, uint8_t pin, enum gpio_npm1300_mode mode, uint32_t flags);

int gpio_npm1300_set(void *dev, uint8_t pin, bool state);

int gpio_npm1300_get(void *dev, uint8_t pin, bool *state);

#endif /* GPIO_NPM1300_H_ */
