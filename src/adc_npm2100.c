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
#include "linear_range.h"
#include "util.h"

#define BOOST_DPSCOUNT 0x25U
#define BOOST_DPSLIMIT 0x26U
#define BOOST_VBATMINL 0x2FU
#define BOOST_VBATMINH 0x30U
#define BOOST_VOUTMIN  0x31U
#define BOOST_VOUTWRN  0x32U
#define BOOST_VOUTDPS  0x33U

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

struct adc_attr_t {
	const struct linear_range *range;
	uint8_t reg;
};

static const struct adc_config_t adc_config[] = {
	[NPM2100_ADC_VBAT] = {CONFIG_MODE_INS_VBAT, 3200000, 256, 0},
	[NPM2100_ADC_DIETEMP] = {CONFIG_MODE_TEMP, -2120000, 1, 389500000},
	[NPM2100_ADC_DROOP] = {CONFIG_MODE_DROOP, 1500000, 256, 1800000},
	[NPM2100_ADC_VOUT] = {CONFIG_MODE_VOUT, 1500000, 256, 1800000},
};

static const struct linear_range vbat_range = LINEAR_RANGE_INIT(700000, 50000, 0U, 46U);
static const struct linear_range vout_range = LINEAR_RANGE_INIT(1700000, 50000, 0U, 31U);
static const struct linear_range vdps_range = LINEAR_RANGE_INIT(1800000, 50000, 0U, 31U);
static const struct linear_range dpslim_range = LINEAR_RANGE_INIT(3, 1, 3U, 255U);

static const struct adc_attr_t adc_attr[] = {
	[NPM2100_ADC_VBATMINH] = {&vbat_range, BOOST_VBATMINH},
	[NPM2100_ADC_VBATMINL] = {&vbat_range, BOOST_VBATMINL},
	[NPM2100_ADC_VOUTDPS] = {&vdps_range, BOOST_VOUTDPS},
	[NPM2100_ADC_VOUTMIN] = {&vout_range, BOOST_VOUTMIN},
	[NPM2100_ADC_VOUTWRN] = {&vout_range, BOOST_VOUTWRN},
	[NPM2100_ADC_DPSLIMIT] = {&dpslim_range, BOOST_DPSLIMIT},
};

int adc_npm2100_take_reading(void *dev, enum npm2100_adc_chan chan)
{
	int ret;

	switch (chan) {
	case NPM2100_ADC_VBAT:
	case NPM2100_ADC_DIETEMP:
	case NPM2100_ADC_DROOP:
	case NPM2100_ADC_VOUT:
		ret = i2c_reg_write_byte(dev, ADC_CONFIG, adc_config[chan].mode);
		if (ret < 0) {
			return ret;
		}
		return i2c_reg_write_byte(dev, ADC_TASKS_START, 1U);

	case NPM2100_ADC_DPSCOUNT:
		return 0;

	default:
		return -ENODEV;
	}
}

int adc_npm2100_get_result(void *dev, enum npm2100_adc_chan chan, int32_t *value)
{
	uint8_t data;
	int ret;

	switch (chan) {
	case NPM2100_ADC_VBAT:
	case NPM2100_ADC_DIETEMP:
	case NPM2100_ADC_DROOP:
	case NPM2100_ADC_VOUT:
		ret = i2c_reg_read_byte(dev, ADC_RESULTS + chan, &data);
		if (ret < 0) {
			return ret;
		}

		*value = adc_config[chan].offset +
			 (((int32_t)data * adc_config[chan].mul) / adc_config[chan].div);

		return 0;

	case NPM2100_ADC_DPSCOUNT:
		ret = i2c_reg_read_byte(dev, BOOST_DPSCOUNT, &data);
		if (ret < 0) {
			return ret;
		}

		*value = data;

		return 0;

	default:
		return -ENODEV;
	}
}

int adc_npm2100_attr_get(void *dev, enum npm2100_adc_attr attr, int32_t *value)
{
	if (attr >= ARRAY_SIZE(adc_attr)) {
		return -ENODEV;
	}

	uint8_t data;
	int ret = i2c_reg_read_byte(dev, adc_attr[attr].reg, &data);

	if (ret < 0) {
		return ret;
	}

	return linear_range_get_value(adc_attr[attr].range, data, value);
}

int adc_npm2100_attr_set(void *dev, enum npm2100_adc_attr attr, int32_t value)
{
	if (attr >= ARRAY_SIZE(adc_attr)) {
		return -ENODEV;
	}

	uint16_t data;
	int ret = linear_range_get_index(adc_attr[attr].range, value, &data);

	if (ret < 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, adc_attr[attr].reg, data);
}
