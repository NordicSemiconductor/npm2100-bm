/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "i2c.h"
#include "adc_npm2100.h"
#include "gpio_npm2100.h"
#include "regulator_npm2100.h"
#include "watchdog_npm2100.h"

int main(void)
{
	struct i2c_dev dev = {.addr = 0x74U};

	puts("Disabling boot monitor");
	watchdog_npm2100_disable(&dev);

	puts("Initialising regulators");
	regulator_npm2100_set_voltage(&dev, NPM2100_SOURCE_BOOST, 2000000, 2000000);
	regulator_npm2100_set_voltage(&dev, NPM2100_SOURCE_LDOSW, 2000000, 2000000);

	puts("Initialising GPIOs");
	gpio_npm2100_config(&dev, 0, NPM2100_GPIO_MODE_GPIO,
			    NPM2100_GPIO_CONFIG_INPUT | NPM2100_GPIO_CONFIG_PULLUP);
	gpio_npm2100_config(&dev, 1, NPM2100_GPIO_MODE_GPIO, NPM2100_GPIO_CONFIG_OUTPUT);

	puts("Initialising Watchdog");
	watchdog_npm2100_init(&dev, 5000, NPM2100_WATCHDOG_RESET_PIN);

	while (true) {
		gpio_npm2100_set(&dev, 1, true);
		adc_npm2100_take_reading(&dev, NPM2100_ADC_VBAT);
		sleep(1);

		gpio_npm2100_set(&dev, 1, false);
		adc_npm2100_take_reading(&dev, NPM2100_ADC_DIETEMP);
		sleep(1);

		int32_t vbat;
		int32_t dietemp;
		adc_npm2100_get_result(&dev, NPM2100_ADC_VBAT, &vbat);
		adc_npm2100_get_result(&dev, NPM2100_ADC_DIETEMP, &dietemp);

		watchdog_npm2100_feed(&dev);
	}
}
