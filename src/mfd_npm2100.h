/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MFD_NPM2100_H_
#define MFD_NPM2100_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum mfd_npm2100_event_t {
	NPM2100_EVENT_SYS_DIETEMP_WARN,
	NPM2100_EVENT_SYS_SHIPHOLD_FALL,
	NPM2100_EVENT_SYS_SHIPHOLD_RISE,
	NPM2100_EVENT_SYS_PGRESET_FALL,
	NPM2100_EVENT_SYS_PGRESET_RISE,
	NPM2100_EVENT_SYS_TIMER_EXPIRY,
	NPM2100_EVENT_ADC_VBAT_READY,
	NPM2100_EVENT_ADC_DIETEMP_READY,
	NPM2100_EVENT_ADC_DROOP_DETECT,
	NPM2100_EVENT_ADC_VOUT_READY,
	NPM2100_EVENT_GPIO0_FALL,
	NPM2100_EVENT_GPIO0_RISE,
	NPM2100_EVENT_GPIO1_FALL,
	NPM2100_EVENT_GPIO1_RISE,
	NPM2100_EVENT_BOOST_VBAT_WARN,
	NPM2100_EVENT_BOOST_VOUT_MIN,
	NPM2100_EVENT_BOOST_VOUT_WARN,
	NPM2100_EVENT_BOOST_VOUT_DPS,
	NPM2100_EVENT_BOOST_VOUT_OK,
	NPM2100_EVENT_LDOSW_OCP,
	NPM2100_EVENT_LDOSW_VINTFAIL,
	NPM2100_EVENT_MAX
};

/**
 * @brief Write npm2100 timer register
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param time_ms timer value in ms
 * @return 0 If successful, -EINVAL if time value is too large, -errno In case of any bus error
 */
int mfd_npm2100_set_timer(void *dev, uint32_t time_ms);

/**
 * @brief npm2100 full power reset
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int mfd_npm2100_reset(void *dev);

/**
 * @brief npm2100 hibernate
 *
 * Enters low power state, and wakes after specified time
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param time_ms timer value in ms
 * @return 0 If successful, -EINVAL if time value is too large, -errno In case of any bus error
 */
int mfd_npm2100_hibernate(void *dev, uint32_t time_ms);

/**
 * @brief  Enable npm1300 event interrupt
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param events bitfield of events to enable (bits are defined by mfd_npm2100_event_t)
 * @return 0 on success, -errno on failure
 */
int mfd_npm2100_enable_events(void *dev, uint32_t events);

/**
 * @brief  Enable npm1300 event interrupt
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param events bitfield of events to disable (bits are defined by mfd_npm2100_event_t)
 * @return 0 on success, -errno on failure
 */
int mfd_npm2100_disable_events(void *dev, uint32_t events);

/**
 * @brief  Process npm1300 event interrupt
 *
 * Reads and clears active interrupt events.
 *
 * If an event occurs during processing, the interrupt will not be cleared.
 * The application should check the pin state and call the process function
 * again if the interrupt remains active.
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param events bitfield of detected events (bits are defined by mfd_npm2100_event_t)
 * @return 0 on success, -errno on failure
 */
int mfd_npm2100_process_events(void *dev, uint32_t *events);

#endif /* MFD_NPM2100_H_ */
