/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "byteorder.h"
#include "i2c.h"
#include "mfd_npm2100.h"
#include "util.h"

#define EVENTS_SET	      0x00U
#define EVENTS_CLR	      0x05U
#define INTEN_SET	      0x0AU
#define INTEN_CLR	      0x0FU

#define TIMER_CONFIG	      0xB3U
#define TIMER_TARGET	      0xB4U
#define HIBERNATE_TASKS_HIBER 0xC8U
#define RESET_TASKS_RESET     0xD0U

#define TIMER_CONFIG_WKUP 3U

#define TIMER_PRESCALER_MS 16U
#define TIMER_MAX	   0xFFFFFFU

#define EVENTS_SIZE 5U

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

int mfd_npm2100_set_timer(void *dev, uint32_t time_ms)
{
	uint8_t buff[4] = {TIMER_TARGET};
	uint32_t ticks = time_ms / TIMER_PRESCALER_MS;

	if (ticks > TIMER_MAX) {
		return -EINVAL;
	}

	sys_put_be24(ticks, &buff[1]);

	return i2c_write(dev, buff, sizeof(buff));
}

int mfd_npm2100_reset(void *dev)
{
	return i2c_reg_write_byte(dev, RESET_TASKS_RESET, 1U);
}

int mfd_npm2100_hibernate(void *dev, uint32_t time_ms)
{
	int ret = mfd_npm2100_set_timer(dev, time_ms);

	if (ret != 0) {
		return ret;
	}

	return i2c_reg_write_byte(dev, HIBERNATE_TASKS_HIBER, 1U);
}

int mfd_npm2100_enable_events(void *dev, uint32_t events)
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

int mfd_npm2100_disable_events(void *dev, uint32_t events)
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

int mfd_npm2100_process_events(void *dev, uint32_t *events)
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
