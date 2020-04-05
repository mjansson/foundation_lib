/* random.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#pragma once

/*! \file random.h
\brief Pseudorandom number generator

Pseudorandom number generator. The generator is thread-safe and lock-free, except for the
thread init/shutdown sequences which will use a mutex to synchronize threads. Memory
allocated for thread-local state buffers will be reused by new threads once an old
thread has exited and released ownership of such buffers.

All random functions generate values in ranges where low limit of the range is included
in the set of value, while the high limit is excluded. This is denoted [low,high) in the
documentation for each function, as per https://en.wikipedia.org/wiki/ISO_31-11 notation. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Generate 32 bit random number in full [0,2^32) range
\return 32-bit pseudorandom number in [0,2^32) range */
FOUNDATION_API uint32_t
random32(void);

/*! Generate 32 bit random number in [low,high) range.
\param low Lower limit of range
\param high Upper limit of range
\return 32-bit pseudorandom number in [low,high) range */
FOUNDATION_API uint32_t
random32_range(uint32_t low, uint32_t high);

/*! Generate 64 bit random number in full [0,2^64) range.
\return 64-bit pseudorandom number in [0,2^64) range */
FOUNDATION_API uint64_t
random64(void);

/*! Generate 64 bit random number in [low,high) range.
\param low Lower limit of range
\param high Upper limit of range
\return 64-bit pseudorandom number in [low,high) range */
FOUNDATION_API uint64_t
random64_range(uint64_t low, uint64_t high);

/*! Generate normalized floating point random number with maximum of 54 bits
of precision in [0,1) range.
\return Floating point pseudorandom number with maximum of 54 bits
        of precision in [0,1) range */
FOUNDATION_API real
random_normalized(void);

/*! Generate floating point random number in [low,high) range.
\param low Lower limit of range
\param high Upper limit of range
\return Floating point pseudorandom number in [low,high) range */
FOUNDATION_API real
random_range(real low, real high);

/*! Generate 32 bit normal distribution random number in the [low, high) range.
\param low Lower limit of range
\param high Upper limit of range
\return 32-bit pseudorandom approximation to a normal distribution in the [low, high) range */
FOUNDATION_API int32_t
random32_gaussian_range(int32_t low, int32_t high);

/*! Generate floating point normal distribution random number in the [low, high) range.
\param low Lower limit of range
\param high Upper limit of range
\return Floating point value with an approximated normal distribution in the [low, high) range */
FOUNDATION_API real
random_gaussian_range(real low, real high);

/*! Generate 32 bit triangular distribution random number in the [low, high) range.
\param low Lower limit of range
\param high Upper limit of range
\return 32-bit triangular distribution in the [low, high) range. */
FOUNDATION_API int32_t
random32_triangle_range(int32_t low, int32_t high);

/*! Generate floating point triangular distribution random number in the [low, high) range.
\param low Lower limit of range
\param high Upper limit of range
\return Floating point value with a triangular distribution in the [low, high) range */
FOUNDATION_API real
random_triangle_range(real low, real high);

/*! Generate a weighted random number in the [0,limit) range, with probabilities
equal to the relative weights (probability = weight/total_weight_sum). The weights
array must have "limit" number of elements.
\param limit Upper limit of range
\param weights Array of weights, must have at least limit number of elements
\return 32-bit weighted pseudorandom number in [0,limit) range */
FOUNDATION_API uint32_t
random32_weighted(uint32_t limit, const real* weights);

/*! Free thread memory used by pseudorandom number generator. Will be called automatically
on thread exit for foundation threads. */
FOUNDATION_API void
random_thread_finalize(void);
