/** @file
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MFD_NPM2100_H_
#define MFD_NPM2100_H_

#include <stddef.h>
#include <stdint.h>

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

#endif /* MFD_NPM2100_H_ */
