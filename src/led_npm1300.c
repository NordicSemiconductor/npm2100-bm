/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "led_npm1300.h"
#include "mfd_npm1300.h"

/* nPM1300 LED base address */
#define NPM_LED_BASE 0x0AU

/* nPM1300 LED register offsets */
#define NPM_LED_OFFSET_MODE 0x00U
#define NPM_LED_OFFSET_SET  0x03U
#define NPM_LED_OFFSET_CLR  0x04U

/* nPM1300 Channel counts */
#define NPM1300_LED_PINS 3U

int led_npm1300_on(void *dev, uint32_t led)
{
	if (led >= NPM1300_LED_PINS) {
		return -EINVAL;
	}

	return mfd_npm1300_reg_write(dev, NPM_LED_BASE, NPM_LED_OFFSET_SET + (led * 2U), 1U);
}

int led_npm1300_off(void *dev, uint32_t led)
{
	if (led >= NPM1300_LED_PINS) {
		return -EINVAL;
	}

	return mfd_npm1300_reg_write(dev, NPM_LED_BASE, NPM_LED_OFFSET_CLR + (led * 2U), 1U);
}

int led_npm1300_init(void *dev, uint32_t led, enum led_npm1300_mode mode)
{
	if (led >= NPM1300_LED_PINS) {
		return -EINVAL;
	}

	return mfd_npm1300_reg_write(dev, NPM_LED_BASE, NPM_LED_OFFSET_MODE + led, mode);
}
