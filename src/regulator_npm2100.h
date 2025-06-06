/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef REGULATOR_NPM2100_H_
#define REGULATOR_NPM2100_H_

#include <stdint.h>
#include <stdbool.h>

#include "i2c.h"

/* nPM2100 voltage sources */
enum npm2100_regulator_source {
	NPM2100_SOURCE_BOOST,
	NPM2100_SOURCE_LDOSW,
};

/* Load switch selection */
#define NPM2100_REG_LDSW_EN 0x01U

/* Operating mode */
#define NPM2100_REG_OPER_MASK 0x0EU
#define NPM2100_REG_OPER_AUTO 0x00U
#define NPM2100_REG_OPER_HP   0x02U
#define NPM2100_REG_OPER_LP   0x04U
#define NPM2100_REG_OPER_ULP  0x06U
#define NPM2100_REG_OPER_PASS 0x08U
#define NPM2100_REG_OPER_NOHP 0x0AU
#define NPM2100_REG_OPER_OFF  0x0CU

/* Forced mode when GPIO active */
#define NPM2100_REG_FORCE_MASK 0x70U
#define NPM2100_REG_FORCE_HP   0x10U
#define NPM2100_REG_FORCE_LP   0x20U
#define NPM2100_REG_FORCE_ULP  0x30U
#define NPM2100_REG_FORCE_PASS 0x40U
#define NPM2100_REG_FORCE_NOHP 0x50U

/**
 * @brief Set the output voltage.
 *
 * The output voltage will be configured to the closest supported output
 * voltage. regulator_get_voltage() can be used to obtain the actual configured
 * voltage. The voltage will be applied to the active or selected mode.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 * @param min_uv Minimum acceptable voltage in microvolts.
 * @param max_uv Maximum acceptable voltage in microvolts.
 *
 * @return 0 If successful, -EINVAL If the voltage window is not valid, -errno In case of bus error
 */
int regulator_npm2100_set_voltage(struct i2c_dev *dev, enum npm2100_regulator_source source, int32_t min_uv,
				  int32_t max_uv);

/**
 * @brief Obtain output voltage.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 * @param[out] volt_uv Where configured output voltage will be stored.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_get_voltage(struct i2c_dev *dev, enum npm2100_regulator_source source,
				  int32_t *volt_uv);

/**
 * @brief Enable a regulator.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_enable(struct i2c_dev *dev, enum npm2100_regulator_source source);

/**
 * @brief Disable a regulator.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_disable(struct i2c_dev *dev, enum npm2100_regulator_source source);

/**
 * @brief Enter ship mode.
 *
 * Enter lowest power state.
 * Exit from ship mode is by pin transition.
 *
 * @param dev device pointer, passed to i2c hal layer.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_ship_mode(struct i2c_dev *dev);

/**
 * @brief Set pin control.
 *
 * Configure pin control of regulator.
 * The effect of the pin transition depends on the configured mode.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 * @param gpio_pin GPIO pin number used to control regulator.
 * @param active_low Pin polarity.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_pin_ctrl(struct i2c_dev *dev, enum npm2100_regulator_source source, uint8_t gpio_pin,
			       bool active_low);

/**
 * @brief Configure regulator mode.
 * 
 * Mode should be a NPM2100_REG_OPER_ define, optionally combined with one of NPM2100_REG_FORCE_.
 * When combined with a NPM2100_REG_FORCE_ define, use @ref regulator_npm2100_pin_ctrl to
 * choose a pin and its polarity.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param source regulator source identifier.
 * @param mode Mode to set.
 *
 * @return 0 If successful, -errno In case of bus error
 */
int regulator_npm2100_set_mode(struct i2c_dev *dev, enum npm2100_regulator_source source, uint16_t mode);

#endif /* REGULATOR_NPM2100_H_*/
