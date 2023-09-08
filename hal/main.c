/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "regulator_npm2100.h"
// #include "led_npm1300.h"

int main(void)
{
	void *dev = NULL;

	puts("Initialising BUCKs");
	regulator_npm2100_set_voltage(dev, NPM2100_SOURCE_BOOST, 2000000, 2000000);
	regulator_npm2100_set_voltage(dev, NPM2100_SOURCE_LDOSW, 2000000, 2000000);

	// puts("Initialising LEDs");
	// led_npm1300_init(dev, 0, NPM_LED_MODE_ERROR);
	// led_npm1300_init(dev, 1, NPM_LED_MODE_CHARGING);
	// led_npm1300_init(dev, 2, NPM_LED_MODE_HOST);

	while (true) {
		// led_npm1300_on(dev, 2);
		sleep(1);
	}
}
