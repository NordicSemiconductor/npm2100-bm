/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ADC_NPM2100_H_
#define ADC_NPM2100_H_

#include <stdint.h>

/* nPM2100 adc channels */
enum npm2100_adc_chan {
	NPM2100_ADC_VBAT = 0,
	NPM2100_ADC_DIETEMP = 1,
	NPM2100_ADC_DROOP = 2,
	NPM2100_ADC_VOUT = 3,
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

#endif /* ADC_NPM2100_H_*/
