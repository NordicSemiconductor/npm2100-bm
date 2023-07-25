/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "regulator_npm1300.h"
#include "led_npm1300.h"

int main(void)
{
	void *dev = NULL;

	puts("Initialising BUCKs");
	regulator_npm1300_set_voltage(dev, NPM1300_SOURCE_BUCK1, false, 2000000, 2000000);
	regulator_npm1300_set_voltage(dev, NPM1300_SOURCE_BUCK2, false, 3300000, 3300000);

	puts("Initialising LEDs");
	led_npm1300_init(dev, 0, NPM_LED_MODE_ERROR);
	led_npm1300_init(dev, 1, NPM_LED_MODE_CHARGING);
	led_npm1300_init(dev, 2, NPM_LED_MODE_HOST);

	while (true) {
		led_npm1300_on(dev, 2);
		sleep(1);
	}
}
