/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "i2c.h"
#include "linear_range.h"
#include "regulator_npm2100.h"

#define BOOST_VOUT     0x22U
#define BOOST_VOUTSEL  0x23U
#define BOOST_OPER     0x24U
#define BOOST_GPIO     0x28U
#define BOOST_PIN      0x29U
#define BOOST_CTRLSET  0x2AU
#define BOOST_CTRLCLR  0x2BU
#define BOOST_IBATLIM  0x2DU
#define BOOST_VBATMINL 0x2FU
#define BOOST_VBATMINH 0x30U
#define BOOST_VOUTMIN  0x31U
#define BOOST_VOUTWRN  0x32U
#define BOOST_STATUS0  0x34U
#define BOOST_STATUS1  0x35U
#define BOOST_VSET0    0x36U
#define BOOST_VSET1    0x37U

#define LDOSW_VOUT   0x64U
#define LDOSW_ENABLE 0x65U
#define LDOSW_SEL    0x66U
#define LDOSW_GPIO   0x67U

#define SHIP_TASK_SHIP 0xC0U

#define BOOST_OPER_MODE_MASK 0x07U
#define BOOST_OPER_MODE_AUTO 0x00U
#define BOOST_OPER_MODE_HP   0x01U
#define BOOST_OPER_MODE_LP   0x02U
#define BOOST_OPER_MODE_PASS 0x03U
#define BOOST_OPER_MODE_NOHP 0x04U

#define BOOST_PIN_FORCE_HP   0x00U
#define BOOST_PIN_FORCE_LP   0x01U
#define BOOST_PIN_FORCE_PASS 0x03U
#define BOOST_PIN_FORCE_NOHP 0x04U

#define BOOST_STATUS1_VSET_MASK 0x40U

#define LDOSW_SEL_OPER_MASK 0x06U
#define LDOSW_SEL_OPER_AUTO 0x00U
#define LDOSW_SEL_OPER_ULP  0x02U
#define LDOSW_SEL_OPER_HP   0x04U
#define LDOSW_SEL_OPER_PIN  0x06U

#define LDOSW_GPIO_PIN_MASK	0x07U
#define LDOSW_GPIO_PINACT_MASK	0x18U
#define LDOSW_GPIO_PINACT_HP	0x00U
#define LDOSW_GPIO_PINACT_ULP	0x08U
#define LDOSW_GPIO_PININACT_OFF 0x00U
#define LDOSW_GPIO_PININACT_ULP 0x10U

static const struct linear_range boost_range = LINEAR_RANGE_INIT(1800000, 50000, 0U, 30U);
static const struct linear_range ldosw_range = LINEAR_RANGE_INIT(400000, 50000, 0U, 52U);
static const struct linear_range vset0_range = LINEAR_RANGE_INIT(1800000, 100000, 0U, 6U);
static const struct linear_range vset1_ranges[] = {LINEAR_RANGE_INIT(3000000, 0, 0U, 0U),
						   LINEAR_RANGE_INIT(2700000, 100000, 1U, 3U),
						   LINEAR_RANGE_INIT(3100000, 100000, 4U, 6U)};

int regulator_npm2100_set_voltage(void *dev, enum npm2100_regulator_source source, int32_t min_uv,
				  int32_t max_uv)
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

		return i2c_reg_write_byte(dev, LDOSW_VOUT, idx);

	default:
		return -ENODEV;
	}
}

int regulator_npm2100_get_voltage(void *dev, enum npm2100_regulator_source source, int32_t *volt_uv)
{
	uint8_t idx;
	int ret;

	switch (source) {
	case NPM2100_SOURCE_BOOST:
		ret = i2c_reg_read_byte(dev, BOOST_VOUTSEL, &idx);
		if (ret < 0) {
			return ret;
		}

		if (idx == 1U) {
			/* Voltage is selected by register value */
			ret = i2c_reg_read_byte(dev, BOOST_VOUT, &idx);
			if (ret < 0) {
				return ret;
			}

			return linear_range_get_value(&boost_range, idx, volt_uv);
		}

		/* Voltage is selected by VSET pin */
		ret = i2c_reg_read_byte(dev, BOOST_STATUS1, &idx);
		if (ret < 0) {
			return ret;
		}

		if ((idx & BOOST_STATUS1_VSET_MASK) == 0U) {
			/* VSET low, voltage is selected by VSET0 register */
			ret = i2c_reg_read_byte(dev, BOOST_VSET0, &idx);
			if (ret < 0) {
				return ret;
			}

			return linear_range_get_value(&vset0_range, idx, volt_uv);
		}

		/* VSET high, voltage is selected by VSET1 register */
		ret = i2c_reg_read_byte(dev, BOOST_VSET1, &idx);
		if (ret < 0) {
			return ret;
		}

		return linear_range_group_get_value(vset1_ranges, ARRAY_SIZE(vset1_ranges), idx,
						    volt_uv);

	case NPM2100_SOURCE_LDOSW:
		ret = i2c_reg_read_byte(dev, LDOSW_VOUT, &idx);
		if (ret < 0) {
			return ret;
		}

		return linear_range_get_value(&ldosw_range, idx, volt_uv);

	default:
		return -ENODEV;
	}
}

static int set_boost_mode(void *dev, uint8_t mode)
{
	uint8_t reg;
	int ret;

	/* Normal mode in lower nibble */
	switch (mode & NPM2100_REG_OPER_MASK) {
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
	switch (mode & NPM2100_REG_FORCE_MASK) {
	case 0U:
		return 0;
	case NPM2100_REG_FORCE_HP:
		reg = BOOST_PIN_FORCE_HP;
		break;
	case NPM2100_REG_FORCE_LP:
		reg = BOOST_PIN_FORCE_LP;
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

static int set_ldosw_gpio_mode(void *dev, uint8_t inact, uint8_t act, uint8_t ldsw)
{
	int ret;

	ret = i2c_reg_update_byte(dev, LDOSW_GPIO, LDOSW_GPIO_PINACT_MASK, inact | act);
	if (ret < 0) {
		return ret;
	}

	/* Set operating mode to pin control */
	return i2c_reg_write_byte(dev, LDOSW_SEL, LDOSW_SEL_OPER_PIN | ldsw);
}

static int set_ldosw_mode(void *dev, uint8_t mode)
{
	uint8_t ldsw = mode & NPM2100_REG_LDSW_EN;
	uint8_t oper = mode & NPM2100_REG_OPER_MASK;
	uint8_t force = mode & NPM2100_REG_FORCE_MASK;

	if (force == 0U) {
		/* SW control of mode */
		switch (oper) {
		case NPM2100_REG_OPER_AUTO:
			return i2c_reg_write_byte(dev, LDOSW_SEL, LDOSW_SEL_OPER_AUTO | ldsw);
		case NPM2100_REG_OPER_ULP:
			return i2c_reg_write_byte(dev, LDOSW_SEL, LDOSW_SEL_OPER_ULP | ldsw);
		case NPM2100_REG_OPER_HP:
			return i2c_reg_write_byte(dev, LDOSW_SEL, LDOSW_SEL_OPER_HP | ldsw);
		default:
			return -ENOTSUP;
		}
	}

	switch (oper | force) {
	case NPM2100_REG_OPER_OFF | NPM2100_REG_FORCE_ULP:
		return set_ldosw_gpio_mode(dev, LDOSW_GPIO_PININACT_OFF, LDOSW_GPIO_PINACT_ULP,
					   ldsw);
	case NPM2100_REG_OPER_OFF | NPM2100_REG_FORCE_HP:
		return set_ldosw_gpio_mode(dev, LDOSW_GPIO_PININACT_OFF, LDOSW_GPIO_PINACT_HP,
					   ldsw);
	case NPM2100_REG_OPER_ULP | NPM2100_REG_FORCE_HP:
		return set_ldosw_gpio_mode(dev, LDOSW_GPIO_PININACT_ULP, LDOSW_GPIO_PINACT_HP,
					   ldsw);
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
		return -ENODEV;
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

int regulator_npm2100_pin_ctrl(void *dev, enum npm2100_regulator_source source, uint8_t gpio_pin,
			       bool active_low)
{
	uint8_t pin = gpio_pin << 1U;
	uint8_t offset = active_low ? 0U : 1U;

	switch (source) {
	case NPM2100_SOURCE_BOOST:
		return i2c_reg_write_byte(dev, BOOST_GPIO, pin + offset + 1U);
	case NPM2100_SOURCE_LDOSW:
		return i2c_reg_update_byte(dev, LDOSW_GPIO, LDOSW_GPIO_PIN_MASK, pin + offset);
	default:
		return -ENODEV;
	}
}

int regulator_npm2100_ship_mode(void *dev)
{
	return i2c_reg_write_byte(dev, SHIP_TASK_SHIP, 1U);
}
