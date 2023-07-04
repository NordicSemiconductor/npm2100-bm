/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>

int i2c_write(void *dev, const uint8_t *buf, uint32_t num_bytes);

int i2c_write_read(void *dev, const void *write_buf, size_t num_write, void *read_buf,
		   size_t num_read);
