/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPIO_NPM2100_H_
#define GPIO_NPM2100_H_

#include <stdint.h>

/* nPM2100 GPIO mode flags */
#define NPM2100_GPIO_CONFIG_INPUT     0x01U
#define NPM2100_GPIO_CONFIG_OUTPUT    0x02U
#define NPM2100_GPIO_CONFIG_OPENDRAIN 0x04U
#define NPM2100_GPIO_CONFIG_PULLDOWN  0x08U
#define NPM2100_GPIO_CONFIG_PULLUP    0x10U
#define NPM2100_GPIO_CONFIG_DRIVE     0x20U
#define NPM2100_GPIO_CONFIG_DEBOUNCE  0x40U

int gpio_npm2100_config(void *dev, uint8_t pin, uint8_t flags);

int gpio_npm2100_set(void *dev, uint8_t pin, bool state);

int gpio_npm2100_get(void *dev, uint8_t pin, bool *state);

#endif /* GPIO_NPM2100_H_ */
