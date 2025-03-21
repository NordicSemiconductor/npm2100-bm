/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ADC_NPM2100_H_
#define ADC_NPM2100_H_

#include <stdint.h>

/* nPM2100 adc channels */
enum npm2100_adc_chan {
	NPM2100_ADC_CHAN_VBAT,
	NPM2100_ADC_CHAN_DIETEMP,
	NPM2100_ADC_CHAN_VOUT,
	NPM2100_ADC_CHAN_OFFSET,
};

/* nPM2100 adc attributes */
enum npm2100_adc_attr {
	NPM2100_ADC_ATTR_VBATMIN,
	/* Oversample factor is 2^value */
	NPM2100_ADC_ATTR_OVERSAMPLING,
	NPM2100_ADC_ATTR_DELAY,
	NPM2100_ADC_ATTR_OFFSET_SOURCE,
};

enum npm2100_adc_offset_src {
	NPM2100_ADC_OFFSET_FACTORY =  0,
	NPM2100_ADC_OFFSET_MEASURED = 1,
};

/**
 * @brief Trigger reading of ADC channel
 *
 * Triggers a reading of the specified ADC channel.
 * Call adc_npm2100_get_result to get the results after waiting for at least 100us.
 *
 * The caller must wait for at least 100us before calling this function again for another channel.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param chan adc channel.
 *
 * @return 0 If successful, -ENODEV If the channel is invalid, -errno In case of bus error
 */
int adc_npm2100_take_reading(void *dev, enum npm2100_adc_chan chan);

/**
 * @brief Get result from ADC channel
 *
 * Gets result from specified ADC channel.
 * adc_npm2100_take_reading must be called at least 100us before calling this function.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param chan adc channel.
 * @param value Result value, in micro units.
 *
 * @return 0 If successful, -ENODEV If the channel is invalid, -errno In case of bus error
 */
int adc_npm2100_get_result(void *dev, enum npm2100_adc_chan chan, int32_t *value);

/**
 * @brief Get ADC attribute
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param attr adc attribute.
 * @param value Result value, in micro units.
 *
 * @return 0 If successful
 * @return -ENOTSUP If the channel/attribute combination is invalid, or no such attribute
 * @return -errno In case of bus/conversion errors
 */
int adc_npm2100_attr_get(void *dev, enum npm2100_adc_chan chan, enum npm2100_adc_attr attr, int32_t *value);

/**
 * @brief Set ADC attribute
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param attr adc attribute.
 * @param value Value to set, in micro units.
 *
 * @return 0 If successful
 * @return -ENOTSUP If the channel/attribute combination is invalid, or no such attribute
 * @return -errno In case of bus/conversion errors
 */
int adc_npm2100_attr_set(void *dev, enum npm2100_adc_chan chan, enum npm2100_adc_attr attr, int32_t value);

#endif /* ADC_NPM2100_H_*/
