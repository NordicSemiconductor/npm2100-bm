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

/* Operating mode */
#define NPM2100_REG_OPER_AUTO 0x00U
#define NPM2100_REG_OPER_HP   0x01U
#define NPM2100_REG_OPER_LP   0x02U
#define NPM2100_REG_OPER_ULP  0x03U
#define NPM2100_REG_OPER_PASS 0x04U
#define NPM2100_REG_OPER_NOHP 0x05U
#define NPM2100_REG_OPER_OFF  0x06U

/* Forced mode when GPIO active */
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

#endif /* REGULATOR_NPM2100_H_*/
