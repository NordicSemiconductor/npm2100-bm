/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "src/led_npm1300.h"

int main(void)
{
	puts("Initialising LEDs");
	led_npm1300_init(NULL, 0, NPM_LED_MODE_ERROR);
	led_npm1300_init(NULL, 0, NPM_LED_MODE_CHARGING);
	led_npm1300_init(NULL, 0, NPM_LED_MODE_HOST);

	while (true) {
		led_npm1300_on(NULL, 0);
		sleep(1);
	}
}
