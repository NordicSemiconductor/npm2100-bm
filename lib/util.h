/** @file
 * Copyright (c) 2011-2014, Wind River Systems, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIB_UTIL_H_
#define LIB_UTIL_H_

/** Unsigned integer with bit position @p n set */
#define BIT(n) (1UL << (n))

/**
 * @brief Bit mask with bits 0 through <tt>n-1</tt> (inclusive) set,
 * or 0 if @p n is 0.
 */
#define BIT_MASK(n) (BIT(n) - 1UL)

/**
 * @brief Divide and round up.
 *
 * Example:
 * @code{.c}
 * DIV_ROUND_UP(1, 2); // 1
 * DIV_ROUND_UP(3, 2); // 2
 * @endcode
 *
 * @param n Numerator.
 * @param d Denominator.
 *
 * @return The result of @p n / @p d, rounded up.
 */
#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

/**
 * @brief Divide and round to the nearest integer.
 *
 * Example:
 * @code{.c}
 * DIV_ROUND_CLOSEST(5, 2); // 3
 * DIV_ROUND_CLOSEST(5, -2); // -3
 * DIV_ROUND_CLOSEST(5, 3); // 2
 * @endcode
 *
 * @param n Numerator.
 * @param d Denominator.
 *
 * @return The result of @p n / @p d, rounded to the nearest integer.
 */
#define DIV_ROUND_CLOSEST(n, d)                                                                    \
	((((n) < 0) ^ ((d) < 0)) ? ((n) - ((d) / 2)) / (d) : ((n) + ((d) / 2)) / (d))

/**
 * @brief Number of elements in the given @p array
 *
 * In C++, due to language limitations, this will accept as @p array
 * any type that implements <tt>operator[]</tt>. The results may not be
 * particularly meaningful in this case.
 *
 * In C, passing a pointer as @p array causes a compile error.
 */
#define ARRAY_SIZE(array) ((size_t)((sizeof(array) / sizeof((array)[0]))))

#endif /* LIB_UTIL_H_ */
