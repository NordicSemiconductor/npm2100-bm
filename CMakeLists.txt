# Copyright (c) 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

project(npm2100 VERSION 0.1)

include_directories(
        src
	hal
	lib
)

add_executable(npm2100
	       src/regulator_npm2100.c
	       hal/main.c
	       hal/i2c.c
)
