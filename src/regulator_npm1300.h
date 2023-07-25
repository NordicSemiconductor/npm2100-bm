/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef REGULATOR_NPM1300_H_
#define REGULATOR_NPM1300_H_

#include <stdint.h>

/* nPM1300 voltage sources */
enum npm1300_regulator_source {
	NPM1300_SOURCE_BUCK1,
	NPM1300_SOURCE_BUCK2,
	NPM1300_SOURCE_LDO1,
	NPM1300_SOURCE_LDO2,
};

enum npm1300_regulator_mode {
	NPM1300_BUCK_MODE_AUTO,
	NPM1300_BUCK_MODE_PWM,
	NPM1300_LDSW_MODE_LDO,
	NPM1300_LDSW_MODE_LDSW
};

enum npm1300_regulator_pinctrl {
	NPM1300_PINCTRL_ENABLE,
	NPM1300_PINCTRL_PWM,
	NPM1300_PINCTRL_RETENTION
};

int regulator_npm1300_set_voltage(void *dev, enum npm1300_regulator_source source, bool retention,
				  int32_t min_uv, int32_t max_uv);

int regulator_npm1300_set_mode(void *dev, enum npm1300_regulator_source source,
			       enum npm1300_regulator_mode mode);

int regulator_npm1300_set_pin_ctrl(void *dev, enum npm1300_regulator_source source,
				   enum npm1300_regulator_pinctrl pinctrl, uint8_t pin, bool inv);

int regulator_npm1300_enable(void *dev, enum npm1300_regulator_source source);

int regulator_npm1300_disable(void *dev, enum npm1300_regulator_source source);

#endif /* REGULATOR_NPM1300_H_*/
