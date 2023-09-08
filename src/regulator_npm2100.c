/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "linear_range.h"
#include "i2c.h"
#include "regulator_npm2100.h"

#define BOOST_VOUT     0x20U
#define BOOST_VOUTSEL  0x21U
#define BOOST_OPER     0x22U
#define BOOST_GPIO     0x23U
#define BOOST_PIN      0x24U
#define BOOST_CTRLSET  0x25U
#define BOOST_CTRLCLR  0x26U
#define BOOST_IBATLIM  0x28U
#define BOOST_VBATMINL 0x29U
#define BOOST_VBATMINH 0x2AU
#define BOOST_VOUTMIN  0x2BU
#define BOOST_VOUTWRN  0x2CU
#define BOOST_STATUS   0x2DU

#define LDOSW_LVOUT    0x48U
#define LDOSW_ENABLE   0x49U
#define LDOSW_SEL      0x4AU
#define LDOSW_GPIO     0x4BU
#define LDOSW_PINACT   0x4CU
#define LDOSW_PININACT 0x4DU

#define SHIP_TASK_SHIP 0x90U

#define BOOST_OPER_MODE_MASK 0x07U
#define BOOST_OPER_MODE_AUTO 0x00U
#define BOOST_OPER_MODE_LP   0x01U
#define BOOST_OPER_MODE_HP   0x02U
#define BOOST_OPER_MODE_PASS 0x03U
#define BOOST_OPER_MODE_NOHP 0x04U

#define BOOST_PIN_FORCE_HP   0x00U
#define BOOST_PIN_FORCE_LP   0x01U
#define BOOST_PIN_FORCE_ULP  0x02U
#define BOOST_PIN_FORCE_PASS 0x03U
#define BOOST_PIN_FORCE_NOHP 0x04U

#define LDOSW_SEL_OPER_MASK 0x06U
#define LDOSW_SEL_OPER_AUTO 0x00U
#define LDOSW_SEL_OPER_ULP  0x02U
#define LDOSW_SEL_OPER_HP   0x04U
#define LDOSW_SEL_OPER_PIN  0x06U

#define LDOSW_PINACT_HP	 0x00U
#define LDOSW_PINACT_ULP 0x01U

#define LDOSW_PININACT_OFF 0x00U
#define LDOSW_PININACT_ULP 0x01U

static const struct linear_range boost_range = LINEAR_RANGE_INIT(1800000, 50000, 0U, 30U);
static const struct linear_range ldosw_range = LINEAR_RANGE_INIT(800000, 100000, 0U, 22U);

int regulator_npm2100_set_voltage(void *dev, enum npm2100_regulator_source source,
					 int32_t min_uv, int32_t max_uv)
{
	uint16_t idx;
	int ret;

	switch (source) {
	case NPM2100_SOURCE_BOOST:
		ret = linear_range_get_win_index(&boost_range, min_uv, max_uv, &idx);
		if (ret == -EINVAL) {
			return ret;
		}

		ret = i2c_reg_write_byte(dev, BOOST_VOUT, idx);
		if (ret < 0) {
			return ret;
		}

		/* Enable SW control of boost voltage */
		return i2c_reg_write_byte(dev, BOOST_VOUTSEL, 1U);

	case NPM2100_SOURCE_LDOSW:
		ret = linear_range_get_win_index(&ldosw_range, min_uv, max_uv, &idx);
		if (ret == -EINVAL) {
			return ret;
		}

		return i2c_reg_write_byte(dev, LDOSW_LVOUT, idx);

	default:
		return -ENODEV;
	}
}

static int set_boost_mode(void *dev, uint8_t mode)
{
	uint8_t reg;
	int ret;

	/* Normal mode in lower nibble */
	switch (mode & 0x0FU) {
	case NPM2100_REG_OPER_AUTO:
		reg = BOOST_OPER_MODE_AUTO;
		break;
	case NPM2100_REG_OPER_HP:
		reg = BOOST_OPER_MODE_HP;
		break;
	case NPM2100_REG_OPER_LP:
		reg = BOOST_OPER_MODE_LP;
		break;
	case NPM2100_REG_OPER_PASS:
		reg = BOOST_OPER_MODE_PASS;
		break;
	case NPM2100_REG_OPER_NOHP:
		reg = BOOST_OPER_MODE_NOHP;
		break;
	default:
		return -ENOTSUP;
	}

	ret = i2c_reg_update_byte(dev, BOOST_OPER, BOOST_OPER_MODE_MASK, reg);
	if (ret < 0) {
		return ret;
	}

	/* Forced mode in upper nibble */
	switch (mode & 0xF0U) {
	case 0U:
		return 0;
	case NPM2100_REG_FORCE_HP:
		reg = BOOST_PIN_FORCE_HP;
		break;
	case NPM2100_REG_FORCE_LP:
		reg = BOOST_PIN_FORCE_LP;
		break;
	case NPM2100_REG_FORCE_ULP:
		reg = BOOST_PIN_FORCE_ULP;
		break;
	case NPM2100_REG_FORCE_PASS:
		reg = BOOST_PIN_FORCE_PASS;
		break;
	case NPM2100_REG_FORCE_NOHP:
		reg = BOOST_PIN_FORCE_NOHP;
		break;
	default:
		return -ENOTSUP;
	}

	return i2c_reg_write_byte(dev, BOOST_PIN, reg);
}

static int set_ldosw_gpio_mode(void *dev, uint8_t inact, uint8_t act)
{
	int ret;

	ret = i2c_reg_write_byte(dev, LDOSW_PININACT, inact);
	if (ret < 0) {
		return ret;
	}

	ret = i2c_reg_write_byte(dev, LDOSW_PINACT, act);
	if (ret < 0) {
		return ret;
	}

	/* Set operating mode to pin control */
	return i2c_reg_write_byte(dev, LDOSW_SEL, LDOSW_SEL_OPER_PIN);
}

static int set_ldosw_mode(void *dev, uint8_t mode)
{
	if ((mode & 0xF0U) == 0U) {
		/* SW control of mode */
		switch (mode) {
		case NPM2100_REG_OPER_AUTO:
			return i2c_reg_write_byte(dev, LDOSW_SEL, BOOST_PIN_FORCE_HP);
		case NPM2100_REG_OPER_ULP:
			return i2c_reg_write_byte(dev, LDOSW_SEL, BOOST_PIN_FORCE_ULP);
		case NPM2100_REG_OPER_HP:
			return i2c_reg_write_byte(dev, LDOSW_SEL, BOOST_PIN_FORCE_LP);
		default:
			return -ENOTSUP;
		}
	}

	switch (mode) {
	case NPM2100_REG_OPER_OFF | NPM2100_REG_FORCE_ULP:
		return set_ldosw_gpio_mode(dev, LDOSW_PININACT_OFF, LDOSW_PINACT_ULP);
	case NPM2100_REG_OPER_OFF | NPM2100_REG_FORCE_HP:
		return set_ldosw_gpio_mode(dev, LDOSW_PININACT_OFF, LDOSW_PINACT_HP);
	case NPM2100_REG_OPER_ULP | NPM2100_REG_FORCE_HP:
		return set_ldosw_gpio_mode(dev, LDOSW_PININACT_ULP, LDOSW_PINACT_HP);
	default:
		return -ENOTSUP;
	}
}

int regulator_npm2100_set_mode(void *dev, enum npm2100_regulator_source source, uint8_t mode)
{
	switch (source) {
	case NPM2100_SOURCE_BOOST:
		return set_boost_mode(dev, mode);
	case NPM2100_SOURCE_LDOSW:
		return set_ldosw_mode(dev, mode);
	default:
		return -ENOTSUP;
	}
}

int regulator_npm2100_enable(void *dev, enum npm2100_regulator_source source)
{
	if (source != NPM2100_SOURCE_LDOSW) {
		return 0;
	}

	return i2c_reg_write_byte(dev, LDOSW_ENABLE, 1U);
}

int regulator_npm2100_disable(void *dev, enum npm2100_regulator_source source)
{
	if (source != NPM2100_SOURCE_LDOSW) {
		return 0;
	}

	return i2c_reg_write_byte(dev, LDOSW_ENABLE, 0U);
}

int regulator_npm2100_ship_mode(void *dev)
{
	return i2c_reg_write_byte(dev, SHIP_TASK_SHIP, 1U);
}
