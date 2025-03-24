/** @file
 * Copyright (c) 2025 Nordic Semiconductor ASA
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stddef.h>
#include <stdint.h>

#ifndef I2C_H
#define I2C_H

/**
 * @brief i2c device structure.
 *
 * An instance of this structure must be passed to all npm2100 driver function calls.
 */
struct i2c_dev {
	uint8_t addr;  /* I2C device address */
	void* context; /* optional user context */
};

/**
 * @brief Write multiple bytes to I2C peripheral
 *
 * @param dev i2c device.
 * @param buf data buffer to write.
 * @param len Number of bytes to write.
 *
 * @return 0 If successful, -errno In case of error
 */
int i2c_write(struct i2c_dev *dev, uint8_t *buf, size_t len);

/**
 * @brief Write / read transaction with I2C peripheral
 *
 * @param dev i2c device.
 * @param reg first register to read.
 * @param buf buffer for read data.
 * @param len Number of bytes to read.
 *
 * @return 0 If successful, -errno In case of error
 */
int i2c_read(struct i2c_dev *dev, uint8_t reg, uint8_t *buf, size_t len);

/**
 * @brief Write byte to I2C peripheral
 *
 * @param dev i2c device.
 * @param reg register to write.
 * @param data Data byte to write.
 *
 * @return 0 If successful, -errno In case of error
 */
int i2c_reg_write_byte(struct i2c_dev *dev, uint8_t reg, uint8_t data);

/**
 * @brief Read byte from I2C peripheral
 *
 * @param dev i2c device.
 * @param reg register to write.
 * @param[out] data Where read data byte will be stored.
 *
 * @return 0 If successful, -errno In case of error
 */
int i2c_reg_read_byte(struct i2c_dev *dev, uint8_t reg, uint8_t *data);

/**
 * @brief Modify I2C peripheral register
 *
 * @param dev i2c device.
 * @param reg register to write.
 * @param[out] mask Mask of bits to be modified.
 * @param[out] data Data byte to write.
 *
 * @return 0 If successful, -errno In case of error
 */
int i2c_reg_update_byte(struct i2c_dev *dev, uint8_t reg, uint8_t mask, uint8_t data);

#endif // I2C_H
