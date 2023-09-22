/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "regulator_npm2100.h"
#include "gpio_npm2100.h"

int main(void)
{
	void *dev = NULL;

	puts("Initialising BUCKs");
	regulator_npm2100_set_voltage(dev, NPM2100_SOURCE_BOOST, 2000000, 2000000);
	regulator_npm2100_set_voltage(dev, NPM2100_SOURCE_LDOSW, 2000000, 2000000);

	puts("Initialising GPIOs");
	gpio_npm2100_config(dev, 0, NPM2100_GPIO_CONFIG_INPUT | NPM2100_GPIO_CONFIG_PULLUP);
	gpio_npm2100_config(dev, 1, NPM2100_GPIO_CONFIG_OUTPUT);

	while (true) {
		gpio_npm2100_set(dev, 1, true);
		sleep(1);
		gpio_npm2100_set(dev, 1, false);
		sleep(1);
	}
}
