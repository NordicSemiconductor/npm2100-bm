/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "adc_npm2100.h"
#include "i2c.h"
#include "linear_range.h"
#include "util.h"

#define BOOST_VBATSEL  0x2EU
#define BOOST_VBATMINH 0x30U

#define BOOST_VBATMINH_MASK    0x3FU
#define BOOST_VBATMINHSEL_MASK 0x02U

#define ADC_TASKS_ADC      0x90U
#define ADC_CONFIG         0x91U
#define ADC_DELAY          0x92U
#define ADC_OFFSETCFG      0x93U
#define ADC_READVBAT       0x96U
#define ADC_READTEMP       0x97U
#define ADC_READVOUT       0x99U
#define ADC_AVERAGE        0x9BU
#define ADC_OFFSETMEASURED 0x9FU

#define ADC_CONFIG_MODE_MASK 0x07U
#define ADC_CONFIG_AVG_MASK  0x38U
#define ADC_SELOFFSET_MASK   0x02U

#define CONFIG_MODE_INS_VBAT 0x00U
#define CONFIG_MODE_DEL_VBAT 0x01U
#define CONFIG_MODE_TEMP     0x02U
#define CONFIG_MODE_VOUT     0x04U
#define CONFIG_MODE_OFFSET   0x05U

struct adc_config_t {
	uint8_t config;
	uint8_t delay;
	uint8_t result_reg;
	int32_t mul;
	int32_t div;
	int32_t offset;
};

static struct adc_config_t adc_config[] = {
	[NPM2100_ADC_CHAN_VBAT]    = {CONFIG_MODE_INS_VBAT, 0, ADC_READVBAT,       3200000, 256, 0},
	[NPM2100_ADC_CHAN_DIETEMP] = {CONFIG_MODE_TEMP,     0, ADC_READTEMP,       2120000, -1,  389500000},
	[NPM2100_ADC_CHAN_VOUT]    = {CONFIG_MODE_VOUT,     0, ADC_READVOUT,       1500000, 256, 1800000},
	[NPM2100_ADC_CHAN_OFFSET]  = {CONFIG_MODE_OFFSET,   0, ADC_OFFSETMEASURED, 1,       1,   0},
};

static const struct linear_range vbat_range = LINEAR_RANGE_INIT(650000, 50000, 0U, 50U);
static const struct linear_range oversampling_range = LINEAR_RANGE_INIT(0, 1, 0U, 4U);
static const struct linear_range delay_range = LINEAR_RANGE_INIT(5000, 4000, 0U, 255U);

int adc_npm2100_take_reading(struct i2c_dev *dev, enum npm2100_adc_chan chan)
{
	int ret;

	switch (chan) {
	case NPM2100_ADC_CHAN_VBAT:
		if (adc_config[chan].delay > 0) {
			ret = i2c_reg_write_byte(dev, ADC_DELAY, adc_config[chan].delay);
			if (ret < 0) {
				return ret;
			}
		}
	/* fall through */
	case NPM2100_ADC_CHAN_DIETEMP:
	case NPM2100_ADC_CHAN_VOUT:
	case NPM2100_ADC_CHAN_OFFSET:
		ret = i2c_reg_write_byte(dev, ADC_CONFIG, adc_config[chan].config);
		if (ret < 0) {
			return ret;
		}
		return i2c_reg_write_byte(dev, ADC_TASKS_ADC, 1U);
	default:
		return -ENODEV;
	}
}

int adc_npm2100_get_result(struct i2c_dev *dev, enum npm2100_adc_chan chan, int32_t *value)
{
	uint8_t data;
	int ret;
	uint8_t reg_addr;

	switch (chan) {
	case NPM2100_ADC_CHAN_VBAT:
	case NPM2100_ADC_CHAN_DIETEMP:
	case NPM2100_ADC_CHAN_VOUT:
	case NPM2100_ADC_CHAN_OFFSET:
		if (FIELD_GET(ADC_CONFIG_AVG_MASK, adc_config[chan].config) == 0) {
			reg_addr = adc_config[chan].result_reg;
		} else {
			reg_addr = ADC_AVERAGE;
		}

		ret = i2c_reg_read_byte(dev, reg_addr, &data);
		if (ret < 0) {
			return ret;
		}

		*value = adc_config[chan].offset +
			 (((int32_t)data * adc_config[chan].mul) / adc_config[chan].div);

		return 0;

	default:
		return -ENODEV;
	}
}

int adc_npm2100_attr_get(struct i2c_dev *dev, enum npm2100_adc_chan chan, enum npm2100_adc_attr attr, int32_t *value)
{
	uint8_t data;
	int ret;

	switch (attr) {
	case NPM2100_ADC_ATTR_OVERSAMPLING:
		if (chan == NPM2100_ADC_CHAN_OFFSET || chan >= ARRAY_SIZE(adc_config)) {
			break;
		}

		data = FIELD_GET(ADC_CONFIG_AVG_MASK, adc_config[chan].config);

		return linear_range_get_value(&oversampling_range, data, value);

	case NPM2100_ADC_ATTR_DELAY:
		if (chan != NPM2100_ADC_CHAN_VBAT) {
			break;
		}

		return linear_range_get_value(&delay_range, adc_config[chan].delay, value);

	case NPM2100_ADC_ATTR_VBATMIN:
		if (chan != NPM2100_ADC_CHAN_VBAT) {
			break;
		}

		ret = i2c_reg_read_byte(dev, BOOST_VBATMINH, &data);
		if (ret < 0) {
			return ret;
		}

		return linear_range_get_value(&vbat_range, data, value);

	case NPM2100_ADC_ATTR_OFFSET_SOURCE:
		ret = i2c_reg_read_byte(dev, ADC_OFFSETCFG, &data);
		if (ret < 0) {
			return ret;
		}

		*value = FIELD_GET(ADC_SELOFFSET_MASK, data);
		return 0;

	default:
		break;
	}

	return -ENOTSUP;
}

int adc_npm2100_attr_set(struct i2c_dev *dev, enum npm2100_adc_chan chan, enum npm2100_adc_attr attr, int32_t value)
{
	uint16_t data;
	int ret;

	switch (attr) {
	case NPM2100_ADC_ATTR_OVERSAMPLING:
		if (chan == NPM2100_ADC_CHAN_OFFSET || chan >= ARRAY_SIZE(adc_config)) {
			break;
		}

		ret = linear_range_get_index(&oversampling_range, value, &data);
		if (ret < 0) {
			return ret;
		}

		adc_config[chan].config &= ~ADC_CONFIG_AVG_MASK;
		adc_config[chan].config |= FIELD_PREP(ADC_CONFIG_AVG_MASK, data);

		return 0;

	case NPM2100_ADC_ATTR_DELAY:
		if (chan != NPM2100_ADC_CHAN_VBAT) {
			break;
		}

		ret = linear_range_get_index(&delay_range, value, &data);
		if (ret < 0) {
			return ret;
		}

		adc_config[chan].delay = (uint8_t)data;
		adc_config[chan].config &= ~ADC_CONFIG_MODE_MASK;
		if (value > 0) {
			adc_config[chan].config |= FIELD_PREP(ADC_CONFIG_MODE_MASK, CONFIG_MODE_DEL_VBAT);
		} else {
			adc_config[chan].config |= FIELD_PREP(ADC_CONFIG_MODE_MASK, CONFIG_MODE_INS_VBAT);
		}

		return 0;

	case NPM2100_ADC_ATTR_VBATMIN:
		if (chan != NPM2100_ADC_CHAN_VBAT) {
			break;
		}

		ret = linear_range_get_index(&vbat_range, value, &data);
		if (ret < 0) {
			return ret;
		}

		ret = i2c_reg_write_byte(dev, BOOST_VBATMINH, (uint8_t)data);
		if (ret < 0) {
			return ret;
		}

		return i2c_reg_update_byte(dev, BOOST_VBATMINH, BOOST_VBATMINHSEL_MASK, BOOST_VBATMINHSEL_MASK);

	case NPM2100_ADC_ATTR_OFFSET_SOURCE:
		if (value != NPM2100_ADC_OFFSET_FACTORY && value != NPM2100_ADC_OFFSET_MEASURED) {
			return -EINVAL;
		}

		return i2c_reg_update_byte(dev, ADC_OFFSETCFG, ADC_SELOFFSET_MASK, FIELD_PREP(ADC_SELOFFSET_MASK, value));

	default:
		break;
	}

	return -ENOTSUP;
}
