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

enum mfd_npm2100_timer_mode {
	NPM2100_TIMER_MODE_GENERAL_PURPOSE,
	NPM2100_TIMER_MODE_WDT_RESET,
	NPM2100_TIMER_MODE_WDT_POWER_CYCLE,
	NPM2100_TIMER_MODE_WAKEUP,
};

enum mfd_npm2100_shphld_pull {
	NPM2100_SHPHLD_PULL_NONE,
	NPM2100_SHPHLD_PULL_UP_RESISTOR,
	NPM2100_SHPHLD_PULL_DOWN_RESISTOR,
	NPM2100_SHPHLD_PULL_CURR_WEAK,
	NPM2100_SHPHLD_PULL_CURR_LOW,
	NPM2100_SHPHLD_PULL_CURR_MODERATE,
	NPM2100_SHPHLD_PULL_CURR_HIGH,
};

enum mfd_npm2100_reset_debounce {
	NPM2100_RESET_DEBOUNCE_10S,
	NPM2100_RESET_DEBOUNCE_5S,
	NPM2100_RESET_DEBOUNCE_20S,
	NPM2100_RESET_DEBOUNCE_30S,
};

struct mfd_npm2100_shphld_config {
	bool wakeup_on_rising_edge;
	bool disable_wakeup_from_hiber;
	bool disable_power_off;
	enum mfd_npm2100_shphld_pull pull;
};

struct mfd_npm2100_reset_config {
	bool use_shphld_pin;
	bool disable_long_press;
	enum mfd_npm2100_reset_debounce debounce;
};

/**
 * @brief Write npm2100 timer register
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param time_ms timer value in ms
 * @param mode timer mode
 * @return 0 If successful, -EINVAL if time value is too large, -errno In case of any bus error
 */
int mfd_npm2100_set_timer(void *dev, uint32_t time_ms, enum mfd_npm2100_timer_mode mode);

/**
 * @brief Start npm2100 timer
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int mfd_npm2100_start_timer(void *dev);

/**
 * @brief Stop npm2100 timer
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @return 0 If successful, -errno In case of any bus error
 */
int mfd_npm2100_stop_timer(void *dev);

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
 * @param pass_through set to use pass-through hibernate mode.
 * @return 0 If successful, -EINVAL if time value is too large, -errno In case of any bus error
 */
int mfd_npm2100_hibernate(void *dev, uint32_t time_ms, bool pass_through);

/**
 * @brief  Enable npm2100 event interrupt
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param events bitfield of events to enable (bits are defined by mfd_npm2100_event_t)
 * @return 0 on success, -errno on failure
 */
int mfd_npm2100_enable_events(void *dev, uint32_t events);

/**
 * @brief  Enable npm2100 event interrupt
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param events bitfield of events to disable (bits are defined by mfd_npm2100_event_t)
 * @return 0 on success, -errno on failure
 */
int mfd_npm2100_disable_events(void *dev, uint32_t events);

/**
 * @brief  Process npm2100 event interrupt
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

/**
 * @brief Configure npm2100 SHPHLD pin
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param config configuration options to be used.
 * @return 0 on success, -EINVAL If config is invalid, -errno In case of bus error
 */
int mfd_npm2100_config_shphld(void *dev, const struct mfd_npm2100_shphld_config *config);

/**
 * @brief Configure npm2100 reset behaviour
 *
 * @param dev device pointer, passed to i2c hal layer.
 * @param config configuration options to be used.
 * @return 0 on success, -errno In case of bus error
 */
int mfd_npm2100_config_reset(void *dev, const struct mfd_npm2100_reset_config *config);

#endif /* MFD_NPM2100_H_ */
