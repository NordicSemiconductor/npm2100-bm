/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GPIO_NPM2100_H_
#define GPIO_NPM2100_H_

#include <stdint.h>
#include <stdbool.h>

/* nPM2100 GPIO modes */
#define NPM2100_GPIO_MODE_GPIO     0x00U
#define NPM2100_GPIO_MODE_IRQ_LOW  0x01U
#define NPM2100_GPIO_MODE_IRQ_HIGH 0x02U

/* nPM2100 GPIO flags */
#define NPM2100_GPIO_CONFIG_INPUT      0x01U
#define NPM2100_GPIO_CONFIG_OUTPUT     0x02U
#define NPM2100_GPIO_CONFIG_OPENDRAIN  0x04U
#define NPM2100_GPIO_CONFIG_PULLDOWN   0x08U
#define NPM2100_GPIO_CONFIG_PULLUP     0x10U
#define NPM2100_GPIO_CONFIG_HIGH_DRIVE 0x20U
#define NPM2100_GPIO_CONFIG_DEBOUNCE   0x40U

/**
 * @brief Configure GPIO pin.
 *
 * Configure GPIO pin.
 * Pin mode should be one of NPM2100_GPIO_MODE_ defines.
 * Pin flags should be ORed list of NPM2100_GPIO_CONFIG_ defines.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param pin pin to configure.
 * @param mode Configuration mode.
 * @param flags Configuration flags.
 *
 * @return 0 If successful, -errno In case of error
 */
int gpio_npm2100_config(void *dev, uint8_t pin, uint8_t mode, uint8_t flags);

/**
 * @brief Set GPIO pin state.
 *
 * Set state of GPIO pin.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param pin pin to configure.
 * @param state State to set.
 *
 * @return 0 If successful, -errno In case of error
 */
int gpio_npm2100_set(void *dev, uint8_t pin, bool state);

/**
 * @brief Set GPIO pin state.
 *
 * Get state of GPIO pin.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param pin pin to configure.
 * @param[out] state Where retrieved pin state will be stored.
 *
 * @return 0 If successful, -errno In case of error
 */
int gpio_npm2100_get(void *dev, uint8_t pin, bool *state);

#endif /* GPIO_NPM2100_H_ */
