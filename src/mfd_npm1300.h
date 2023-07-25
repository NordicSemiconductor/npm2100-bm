/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MFD_NPM1300_H_
#define MFD_NPM1300_H_

#include <stddef.h>
#include <stdint.h>

int mfd_npm1300_reg_read_burst(void *dev, uint8_t base, uint8_t offset, void *data, size_t len);
int mfd_npm1300_reg_read(void *dev, uint8_t base, uint8_t offset, uint8_t *data);
int mfd_npm1300_reg_write(void *dev, uint8_t base, uint8_t offset, uint8_t data);
int mfd_npm1300_reg_write2(void *dev, uint8_t base, uint8_t offset, uint8_t data1, uint8_t data2);
int mfd_npm1300_reg_update(void *dev, uint8_t base, uint8_t offset, uint8_t data, uint8_t mask);
int mfd_npm1300_set_timer(void *dev, uint32_t time_ms);

#endif /* MFD_NPM1300_H_ */
