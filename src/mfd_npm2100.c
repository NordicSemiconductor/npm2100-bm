/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>

#include "byteorder.h"
#include "i2c.h"
#include "mfd_npm2100.h"
#include "util.h"

#define EVENTS_SET        0x00U
#define EVENTS_CLR        0x05U
#define INTEN_SET         0x0AU
#define INTEN_CLR         0x0FU
#define TIMER_STATUS      0xB7U

#define TIMER_TASKS_START       0xB0U
#define TIMER_TASKS_STOP        0xB1U
#define TIMER_CONFIG            0xB3U
#define TIMER_TARGET            0xB4U
#define HIBERNATE_TASKS_HIBER   0xC8U
#define HIBERNATE_TASKS_HIBERPT 0xC9U
#define RESET_TASKS_RESET       0xD0U

#define TIMER_STATUS_IDLE 0U

#define TIMER_PRESCALER_MUL 64ULL
#define TIMER_PRESCALER_DIV 1000ULL
#define TIMER_MAX           0xFFFFFFU

#define EVENTS_SIZE 5U

#define SHIP_WAKEUP 0xC1U
#define SHIP_SHPHLD 0xC2U

#define WAKEUP_EDGE_MASK       0x01U
#define WAKEUP_HIBERNATE_MASK  0x02U
#define WAKEUP_EDGE_RISING     0x01U
#define WAKEUP_HIBERNATE_NOPIN 0x01U

#define SHPHLD_RESISTOR_MASK 0x03U
#define SHPHLD_CURR_MASK     0x0CU
#define SHPHLD_PULL_MASK     0x10U
#define RESISTOR_PULL_NONE   0x01U
#define RESISTOR_PULL_DOWN   0x02U
#define SHPHLD_PULL_ENABLE   0x01U

#define SHIP_BUTTON_DISABLE 0x01U

#define RESET_BUTTON       0xD2U
#define RESET_PIN          0xD3U
#define RESET_DEBOUNCE     0xD4U
#define RESET_WRITESTICKY  0xDBU
#define RESET_STROBESTICKY 0xDCU

#define LONGPRESS_DISABLE 0x01U
#define RESET_PIN_SHPHLD  0x01U

#define PWRBUTTON_MASK     0x04U
#define PWRBUTTON_DISABLED 0x01U
#define STROBE             0x01U

struct event_reg_t {
	uint8_t offset;
	uint8_t mask;
};

static const struct event_reg_t event_reg[NPM2100_EVENT_MAX] = {
	[NPM2100_EVENT_SYS_DIETEMP_WARN] = {0x00U, 0x01U},
	[NPM2100_EVENT_SYS_SHIPHOLD_FALL] = {0x00U, 0x02U},
	[NPM2100_EVENT_SYS_SHIPHOLD_RISE] = {0x00U, 0x04U},
	[NPM2100_EVENT_SYS_PGRESET_FALL] = {0x00U, 0x08U},
	[NPM2100_EVENT_SYS_PGRESET_RISE] = {0x00U, 0x10U},
	[NPM2100_EVENT_SYS_TIMER_EXPIRY] = {0x00U, 0x20U},
	[NPM2100_EVENT_ADC_VBAT_READY] = {0x01U, 0x01U},
	[NPM2100_EVENT_ADC_DIETEMP_READY] = {0x01U, 0x02U},
	[NPM2100_EVENT_ADC_DROOP_DETECT] = {0x01U, 0x04U},
	[NPM2100_EVENT_ADC_VOUT_READY] = {0x01U, 0x08U},
	[NPM2100_EVENT_GPIO0_FALL] = {0x02U, 0x01U},
	[NPM2100_EVENT_GPIO0_RISE] = {0x02U, 0x02U},
	[NPM2100_EVENT_GPIO1_FALL] = {0x02U, 0x04U},
	[NPM2100_EVENT_GPIO1_RISE] = {0x02U, 0x08U},
	[NPM2100_EVENT_BOOST_VBAT_WARN] = {0x03U, 0x01U},
	[NPM2100_EVENT_BOOST_VOUT_MIN] = {0x03U, 0x02U},
	[NPM2100_EVENT_BOOST_VOUT_WARN] = {0x03U, 0x04U},
	[NPM2100_EVENT_BOOST_VOUT_DPS] = {0x03U, 0x08U},
	[NPM2100_EVENT_BOOST_VOUT_OK] = {0x03U, 0x10U},
	[NPM2100_EVENT_LDOSW_OCP] = {0x04U, 0x01U},
	[NPM2100_EVENT_LDOSW_VINTFAIL] = {0x04U, 0x02U},
};

int mfd_npm2100_set_timer(struct i2c_dev *dev, uint32_t time_ms, enum mfd_npm2100_timer_mode mode)
{
	uint8_t buff[4] = {TIMER_TARGET};
	int64_t ticks = DIV_ROUND_CLOSEST(((int64_t)time_ms * TIMER_PRESCALER_MUL),
						     TIMER_PRESCALER_DIV);
	uint8_t timer_status;
	int ret;

	if (ticks > TIMER_MAX) {
		return -EINVAL;
	}

	ret = i2c_reg_read_byte(dev, TIMER_STATUS, &timer_status);
	if (ret < 0) {
		return ret;
	}

	if (timer_status != TIMER_STATUS_IDLE) {
		return -EBUSY;
	}

	sys_put_be24(ticks, &buff[1]);

	ret = i2c_write(dev, buff, sizeof(buff));
	if (ret < 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, TIMER_CONFIG, mode);
}

int mfd_npm2100_start_timer(struct i2c_dev *dev)
{
	return i2c_reg_write_byte(dev, TIMER_TASKS_START, 1U);
}

int mfd_npm2100_stop_timer(struct i2c_dev *dev)
{
	return i2c_reg_write_byte(dev, TIMER_TASKS_STOP, 1U);
}

int mfd_npm2100_reset(struct i2c_dev *dev)
{
	return i2c_reg_write_byte(dev, RESET_TASKS_RESET, 1U);
}

int mfd_npm2100_hibernate(struct i2c_dev *dev, uint32_t time_ms, bool pass_through)
{
	if (time_ms > 0) {
		int ret = mfd_npm2100_set_timer(dev, time_ms, NPM2100_TIMER_MODE_WAKEUP);

		if (ret < 0) {
			return ret;
		}

		ret = mfd_npm2100_start_timer(dev);

		if (ret < 0) {
			return ret;
		}
	}

	return i2c_reg_write_byte(dev, pass_through ? HIBERNATE_TASKS_HIBERPT : HIBERNATE_TASKS_HIBER, 1U);
}

int mfd_npm2100_enable_events(struct i2c_dev *dev, uint32_t events)
{
	/* Enable interrupts for specified events */
	for (int i = 0; i < NPM2100_EVENT_MAX; i++) {
		if ((events & BIT(i)) != 0U) {
			/* Clear pending interrupt */
			int ret = i2c_reg_write_byte(dev, event_reg[i].offset + EVENTS_CLR,
						     event_reg[i].mask);

			if (ret < 0) {
				return ret;
			}

			ret = i2c_reg_write_byte(dev, event_reg[i].offset + INTEN_SET,
						 event_reg[i].mask);
			if (ret < 0) {
				return ret;
			}
		}
	}

	return 0;
}

int mfd_npm2100_disable_events(struct i2c_dev *dev, uint32_t events)
{
	/* Disable interrupts for specified events */
	for (int i = 0; i < NPM2100_EVENT_MAX; i++) {
		if ((events & BIT(i)) != 0U) {
			int ret = i2c_reg_write_byte(dev, event_reg[i].offset + INTEN_CLR,
						     event_reg[i].mask);
			if (ret < 0) {
				return ret;
			}

			/* Clear pending interrupt */
			ret = i2c_reg_write_byte(dev, event_reg[i].offset + EVENTS_CLR,
						 event_reg[i].mask);

			if (ret < 0) {
				return ret;
			}
		}
	}

	return 0;
}

int mfd_npm2100_process_events(struct i2c_dev *dev, uint32_t *events)
{
	uint8_t buf[EVENTS_SIZE + 1U];
	*events = 0U;

	/* Read MAIN SET registers into buffer, leaving space for register address */
	int ret = i2c_read(dev, EVENTS_SET, &buf[1], EVENTS_SIZE);
	if (ret < 0) {
		return ret;
	}

	for (int i = 0; i < NPM2100_EVENT_MAX; i++) {
		if ((buf[event_reg[i].offset + 1U] & event_reg[i].mask) != 0U) {
			*events |= BIT(i);
		}
	}

	/* Write read buffer back to clear registers to clear all processed events */
	buf[0] = EVENTS_CLR;
	ret = i2c_write(dev, buf, EVENTS_SIZE + 1U);

	return ret;
}

int mfd_npm2100_config_shphld(struct i2c_dev *dev, const struct mfd_npm2100_shphld_config *config)
{
	uint8_t reg = 0U;
	int ret;

	if (config->wakeup_on_rising_edge) {
		reg |= FIELD_PREP(WAKEUP_EDGE_MASK, WAKEUP_EDGE_RISING);
	}
	if (config->disable_wakeup_from_hiber) {
		reg |= FIELD_PREP(WAKEUP_HIBERNATE_MASK, WAKEUP_HIBERNATE_NOPIN);
	}

	ret = i2c_reg_write_byte(dev, SHIP_WAKEUP, reg);
	if (ret < 0) {
		return ret;
	}

	switch (config->pull) {
	case NPM2100_SHPHLD_PULL_UP_RESISTOR:
		reg = 0U;
		break;
	case NPM2100_SHPHLD_PULL_DOWN_RESISTOR:
		reg = FIELD_PREP(SHPHLD_RESISTOR_MASK, RESISTOR_PULL_DOWN);
		break;
	case NPM2100_SHPHLD_PULL_CURR_WEAK:
	case NPM2100_SHPHLD_PULL_CURR_LOW:
	case NPM2100_SHPHLD_PULL_CURR_MODERATE:
	case NPM2100_SHPHLD_PULL_CURR_HIGH:
		reg = FIELD_PREP(SHPHLD_RESISTOR_MASK, RESISTOR_PULL_NONE);
		reg |= FIELD_PREP(SHPHLD_PULL_MASK, SHPHLD_PULL_ENABLE);
		reg |= FIELD_PREP(SHPHLD_CURR_MASK, config->pull - NPM2100_SHPHLD_PULL_CURR_WEAK);
		break;
	case NPM2100_SHPHLD_PULL_NONE:
		reg = FIELD_PREP(SHPHLD_RESISTOR_MASK, RESISTOR_PULL_NONE);
		break;
	default:
		return -EINVAL;
	}

	ret = i2c_reg_write_byte(dev, SHIP_SHPHLD, reg);
	if (ret < 0) {
		return ret;
	}

	if (config->disable_power_off) {
		reg = FIELD_PREP(PWRBUTTON_MASK, PWRBUTTON_DISABLED);
		ret = i2c_reg_update_byte(dev, RESET_WRITESTICKY, PWRBUTTON_MASK, reg); 
		if (ret < 0) {
			return ret;
		}
		ret = i2c_reg_write_byte(dev, RESET_STROBESTICKY, STROBE);
	}

	return ret;
}

int mfd_npm2100_config_reset(struct i2c_dev *dev, const struct mfd_npm2100_reset_config *config) {
	int ret;

	uint8_t reg = (config->use_shphld_pin) ? RESET_PIN_SHPHLD : 0U;
	ret = i2c_reg_write_byte(dev, RESET_PIN, reg);
	if (ret < 0) {
		return ret;
	}

	reg = (config->disable_long_press) ? LONGPRESS_DISABLE : 0U;
	ret = i2c_reg_write_byte(dev, RESET_BUTTON, reg);
	if (ret < 0) {
		return ret;
	}

	reg = (uint8_t)config->debounce;
	return i2c_reg_write_byte(dev, RESET_DEBOUNCE, reg);
}
