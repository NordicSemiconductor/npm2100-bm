/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "linear_range.h"
#include "mfd_npm1300.h"
#include "regulator_npm1300.h"

// /* nPM1300 gpio control channels */
// enum npm1300_gpio_type {
// 	NPM1300_GPIO_TYPE_ENABLE,
// 	NPM1300_GPIO_TYPE_RETENTION,
// 	NPM1300_GPIO_TYPE_PWM
// };

/* nPM1300 regulator base addresses */
#define BUCK_BASE 0x04U
#define LDSW_BASE 0x08U

/* nPM1300 regulator register offsets */
#define BUCK_OFFSET_EN_SET    0x00U
#define BUCK_OFFSET_EN_CLR    0x01U
#define BUCK_OFFSET_PWM_SET   0x04U
#define BUCK_OFFSET_PWM_CLR   0x05U
#define BUCK_OFFSET_VOUT_NORM 0x08U
#define BUCK_OFFSET_VOUT_RET  0x09U
#define BUCK_OFFSET_EN_CTRL   0x0CU
#define BUCK_OFFSET_VRET_CTRL 0x0DU
#define BUCK_OFFSET_PWM_CTRL  0x0EU
#define BUCK_OFFSET_SW_CTRL   0x0FU
#define BUCK_OFFSET_VOUT_STAT 0x10U
#define BUCK_OFFSET_CTRL0     0x15U

/* nPM1300 ldsw register offsets */
#define LDSW_OFFSET_EN_SET  0x00U
#define LDSW_OFFSET_EN_CLR  0x01U
#define LDSW_OFFSET_STATUS  0x04U
#define LDSW_OFFSET_GPISEL  0x05U
#define LDSW_OFFSET_CONFIG  0x07U
#define LDSW_OFFSET_LDOSEL  0x08U
#define LDSW_OFFSET_VOUTSEL 0x0CU

/* Linear range for output voltage, common for all bucks and LDOs on this device */
static const struct linear_range buckldo_range = LINEAR_RANGE_INIT(1000000, 100000, 0U, 23U);

int regulator_npm1300_set_voltage(void *dev, enum npm1300_regulator_source source, bool retention,
				  int32_t min_uv, int32_t max_uv)
{
	uint16_t idx;
	uint8_t chan;

	int ret = linear_range_get_win_index(&buckldo_range, min_uv, max_uv, &idx);
	if (ret == -EINVAL) {
		return ret;
	}

	switch (source) {
	case NPM1300_SOURCE_BUCK1:
	case NPM1300_SOURCE_BUCK2:
		chan = source - NPM1300_SOURCE_BUCK1;

		if (retention) {
			/* Set retention threshold */
			return mfd_npm1300_reg_write(dev, BUCK_BASE,
						     BUCK_OFFSET_VOUT_RET + (chan * 2U), idx);
		}

		/* Set normal threshold */
		ret = mfd_npm1300_reg_write(dev, BUCK_BASE, BUCK_OFFSET_VOUT_NORM + (chan * 2U),
					    idx);
		if (ret < 0) {
			return ret;
		}

		/* Enable SW control of buck output */
		uint8_t mask = BIT(chan);
		return mfd_npm1300_reg_update(dev, BUCK_BASE, BUCK_OFFSET_SW_CTRL, mask, mask);

	case NPM1300_SOURCE_LDO1:
	case NPM1300_SOURCE_LDO2:
		chan = source - NPM1300_SOURCE_LDO1;

		if (retention) {
			/* Retention not supported for LDO sources */
			return -EINVAL;
		}
		return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_VOUTSEL + chan, idx);

	default:
		return -ENOTSUP;
	}
}

int regulator_npm1300_set_mode(void *dev, enum npm1300_regulator_source source,
			       enum npm1300_regulator_mode mode)
{
	uint8_t chan;

	switch (source) {
	case NPM1300_SOURCE_BUCK1:
	case NPM1300_SOURCE_BUCK2:
		chan = source - NPM1300_SOURCE_BUCK1;

		switch (mode) {
		case NPM1300_BUCK_MODE_PWM:
			return mfd_npm1300_reg_write(dev, BUCK_BASE,
						     BUCK_OFFSET_PWM_SET + (chan * 2U), 1U);
		case NPM1300_BUCK_MODE_AUTO:
			return mfd_npm1300_reg_write(dev, BUCK_BASE,
						     BUCK_OFFSET_PWM_CLR + (chan * 2U), 1U);
		default:
			return -ENOTSUP;
		}

	case NPM1300_SOURCE_LDO1:
	case NPM1300_SOURCE_LDO2:
		chan = source - NPM1300_SOURCE_LDO1;

		switch (mode) {
		case NPM1300_LDSW_MODE_LDO:
			return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_LDOSEL + chan, 1U);
		case NPM1300_LDSW_MODE_LDSW:
			return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_LDOSEL + chan, 0U);
		default:
			return -ENOTSUP;
		}

	default:
		return -ENOTSUP;
	}
}

int regulator_npm1300_enable(void *dev, enum npm1300_regulator_source source)
{
	switch (source) {
	case NPM1300_SOURCE_BUCK1:
		return mfd_npm1300_reg_write(dev, BUCK_BASE, BUCK_OFFSET_EN_SET, 1U);
	case NPM1300_SOURCE_BUCK2:
		return mfd_npm1300_reg_write(dev, BUCK_BASE, BUCK_OFFSET_EN_SET + 2U, 1U);
	case NPM1300_SOURCE_LDO1:
		return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_EN_SET, 1U);
	case NPM1300_SOURCE_LDO2:
		return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_EN_SET + 2U, 1U);
	default:
		return 0;
	}
}

int regulator_npm1300_disable(void *dev, enum npm1300_regulator_source source)
{
	switch (source) {
	case NPM1300_SOURCE_BUCK1:
		return mfd_npm1300_reg_write(dev, BUCK_BASE, BUCK_OFFSET_EN_CLR, 1U);
	case NPM1300_SOURCE_BUCK2:
		return mfd_npm1300_reg_write(dev, BUCK_BASE, BUCK_OFFSET_EN_CLR + 2U, 1U);
	case NPM1300_SOURCE_LDO1:
		return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_EN_CLR, 1U);
	case NPM1300_SOURCE_LDO2:
		return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_EN_CLR + 2U, 1U);
	default:
		return 0;
	}
}

static int set_buck_pin_ctrl(void *dev, uint8_t chan, uint8_t pin, uint8_t inv,
			     enum npm1300_regulator_pinctrl type)
{
	uint8_t ctrl;
	uint8_t mask;

	switch (chan) {
	case 0:
		/* Invert control in bit 6, pin control in bits 2-0 */
		ctrl = (inv << 6U) | (pin + 1U);
		mask = BIT(6U) | BIT_MASK(3U);
		break;
	case 1:
		/* Invert control in bit 7, pin control in bits 5-3 */
		ctrl = (inv << 7U) | ((pin + 1U) << 3U);
		mask = BIT(7U) | (BIT_MASK(3U) << 3U);
		break;
	default:
		return -EINVAL;
	}

	switch (type) {
	case NPM1300_PINCTRL_ENABLE:
		return mfd_npm1300_reg_update(dev, BUCK_BASE, BUCK_OFFSET_EN_CTRL, ctrl, mask);
	case NPM1300_PINCTRL_PWM:
		return mfd_npm1300_reg_update(dev, BUCK_BASE, BUCK_OFFSET_PWM_CTRL, ctrl, mask);
	case NPM1300_PINCTRL_RETENTION:
		return mfd_npm1300_reg_update(dev, BUCK_BASE, BUCK_OFFSET_VRET_CTRL, ctrl, mask);
	default:
		return -ENOTSUP;
	}
}

static int set_ldsw_pin_ctrl(void *dev, uint8_t chan, uint8_t pin, uint8_t inv,
			     enum npm1300_regulator_pinctrl type)
{
	if (type != NPM1300_PINCTRL_ENABLE) {
		return -EINVAL;
	}

	uint8_t ctrl = (pin + 1U) | (inv << 3U);

	return mfd_npm1300_reg_write(dev, LDSW_BASE, LDSW_OFFSET_GPISEL + chan, ctrl);
}

int regulator_npm1300_set_pin_ctrl(void *dev, enum npm1300_regulator_source source,
				   enum npm1300_regulator_pinctrl pinctrl, uint8_t pin, bool inv)
{
	switch (source) {
	case NPM1300_SOURCE_BUCK1:
		return set_buck_pin_ctrl(dev, 0, pin, inv, pinctrl);
	case NPM1300_SOURCE_BUCK2:
		return set_buck_pin_ctrl(dev, 1, pin, inv, pinctrl);
	case NPM1300_SOURCE_LDO1:
		return set_ldsw_pin_ctrl(dev, 0, pin, inv, pinctrl);
	case NPM1300_SOURCE_LDO2:
		return set_ldsw_pin_ctrl(dev, 1, pin, inv, pinctrl);
	default:
		return -ENODEV;
	}
}
