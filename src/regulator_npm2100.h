/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef REGULATOR_NPM2100_H_
#define REGULATOR_NPM2100_H_

#include <stdint.h>

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

int regulator_npm2100_set_voltage(void *dev, enum npm2100_regulator_source source, int32_t min_uv,
				  int32_t max_uv);

int regulator_npm2100_enable(void *dev, enum npm2100_regulator_source source);

int regulator_npm2100_disable(void *dev, enum npm2100_regulator_source source);

int regulator_npm2100_ship_mode(void *dev);

int regulator_npm2100_pin_ctrl(void *dev, enum npm2100_regulator_source source, uint8_t gpio_pin,
			       bool active_low);

int regulator_npm2100_set_mode(void *dev, enum npm2100_regulator_source source, uint8_t mode);

#endif /* REGULATOR_NPM2100_H_*/
