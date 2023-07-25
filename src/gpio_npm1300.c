/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT nordic_npm1300_gpio

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "gpio_npm1300.h"
#include "mfd_npm1300.h"

/* nPM1300 GPIO base address */
#define NPM_GPIO_BASE 0x06U

/* nPM1300 GPIO registers offsets */
#define NPM_GPIO_OFFSET_MODE	  0x00U
#define NPM_GPIO_OFFSET_DRIVE	  0x05U
#define NPM_GPIO_OFFSET_PULLUP	  0x0AU
#define NPM_GPIO_OFFSET_PULLDOWN  0x0FU
#define NPM_GPIO_OFFSET_OPENDRAIN 0x14U
#define NPM_GPIO_OFFSET_DEBOUNCE  0x19U
#define NPM_GPIO_OFFSET_STATUS	  0x1EU

/* nPM1300 Channel counts */
#define NPM1300_GPIO_PINS 5U

int gpio_npm1300_set(void *dev, uint8_t pin, bool state)
{
	uint8_t mode = state ? NPM1300_GPIO_GPOLOGIC1 : NPM1300_GPIO_GPOLOGIC0;

	if (pin >= NPM1300_GPIO_PINS) {
		return -EINVAL;
	}

	return mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_MODE + pin, mode);
}

int gpio_npm1300_get(void *dev, uint8_t pin, bool *state)
{
	uint8_t data = 0U;

	if (pin >= NPM1300_GPIO_PINS) {
		return -EINVAL;
	}

	int ret = mfd_npm1300_reg_read(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_STATUS, &data);
	if (ret < 0) {
		return ret;
	}

	*state = (data & (1U << pin)) != 0U;

	return 0;
}

int gpio_npm1300_config(void *dev, uint8_t pin, enum gpio_npm1300_mode mode, uint32_t flags)
{
	if (pin >= NPM1300_GPIO_PINS) {
		return -EINVAL;
	}

	/* Configure mode */
	int ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_MODE + pin, mode);
	if (ret < 0) {
		return ret;
	}

	/* Configure open drain */
	ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_OPENDRAIN + pin,
				    !!(flags & NPM1300_GPIO_OPEN_DRAIN));
	if (ret < 0) {
		return ret;
	}

	/* Configure pulls */
	ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_PULLUP + pin,
				    !!(flags & NPM1300_GPIO_PULLUP));
	if (ret < 0) {
		return ret;
	}

	ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_PULLDOWN + pin,
				    !!(flags & NPM1300_GPIO_PULLDOWN));
	if (ret < 0) {
		return ret;
	}

	/* Configure drive strength and debounce */
	ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_DRIVE + pin,
				    !!(flags & NPM1300_GPIO_DRIVE_6MA));
	if (ret < 0) {
		return ret;
	}

	ret = mfd_npm1300_reg_write(dev, NPM_GPIO_BASE, NPM_GPIO_OFFSET_DEBOUNCE + pin,
				    !!(flags & NPM1300_GPIO_DEBOUNCE));

	return ret;
}
