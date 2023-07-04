/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "i2c.h"

int i2c_write(void *dev, const uint8_t *buf, uint32_t num_bytes)
{
	(void)dev;

	printf("W: ");
	for (int i = 0; i < num_bytes; i++) {
		printf("%02X ", buf[i]);
	}
	printf("\n");

	return 0;
}

int i2c_write_read(void *dev, const void *write_buf, size_t num_write, void *read_buf,
		   size_t num_read)
{
	(void)dev;
	(void)read_buf;

	printf("W: ");
	for (int i = 0; i < num_write; i++) {
		printf("%02X ", ((uint8_t *)write_buf)[i]);
	}
	printf("| R: %lu bytes\n", num_read);

	return 0;
}
