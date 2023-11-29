/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "adc_npm2100.h"
#include "i2c.h"
#include "util.h"

#define ADC_TASKS_START 0x90U
#define ADC_CONFIG	0X91U
#define ADC_RESULTS	0x96U

#define CONFIG_MODE_INS_VBAT 0x00U
#define CONFIG_MODE_DEL_VBAT 0x01U
#define CONFIG_MODE_TEMP     0x02U
#define CONFIG_MODE_DROOP    0x03U
#define CONFIG_MODE_VOUT     0x04U
#define CONFIG_MODE_OFFSET   0x05U

struct adc_config_t {
	uint8_t mode;
	int32_t mul;
	int32_t div;
	int32_t offset;
};

static const struct adc_config_t adc_config[] = {
	[NPM2100_ADC_VBAT] = {CONFIG_MODE_INS_VBAT, 3200000, 256, 0},
	[NPM2100_ADC_DIETEMP] = {CONFIG_MODE_TEMP, -2120000, 1, 389500000},
	[NPM2100_ADC_DROOP] = {CONFIG_MODE_DROOP, 1500000, 256, 1800000},
	[NPM2100_ADC_VOUT] = {CONFIG_MODE_VOUT, 1500000, 256, 1800000},
};

int adc_npm2100_take_reading(void *dev, enum npm2100_adc_chan chan)
{
	if (chan >= ARRAY_SIZE(adc_config)) {
		return -ENODEV;
	}

	int ret = i2c_reg_write_byte(dev, ADC_CONFIG, adc_config[chan].mode);

	if (ret < 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, ADC_TASKS_START, 1U);
}

int adc_npm2100_get_result(void *dev, enum npm2100_adc_chan chan, int32_t *value)
{
	if (chan >= ARRAY_SIZE(adc_config)) {
		return -ENODEV;
	}

	uint8_t data;

	int ret = i2c_reg_read_byte(dev, ADC_RESULTS + chan, &data);

	if (ret < 0) {
		return ret;
	}

	*value = adc_config[chan].offset +
		 (((int32_t)data * adc_config[chan].mul) / adc_config[chan].div);

	return 0;
}
