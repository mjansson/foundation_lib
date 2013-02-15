/* random.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 * 
 * https://github.com/rampantpixels/foundation_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#pragma once

/*! \file random.h
    Pseudorandom number generator */

#include <foundation/platform.h>
#include <foundation/types.h>


//! \return                 Pseudorandom number in [0,2^32) range
FOUNDATION_API uint32_t     random32( void );

//! \return                 Pseudorandom number in [low,high) range
FOUNDATION_API uint32_t     random32_range( uint32_t low, uint32_t high );

//! \return                 Pseudorandom number in [0,2^64) range
FOUNDATION_API uint64_t     random64( void );

//! \return                 Pseudorandom number in [low,high) range
FOUNDATION_API uint64_t     random64_range( uint64_t low, uint64_t high );

//! \return                 Floating point pseudorandom number with maximum of 54bits of precision in [0,1) range
FOUNDATION_API real         random_normalized( void );

//! \return                 Floating point pseudorandom number in [low,high) range
FOUNDATION_API real         random_range( real low, real high );

/*! Creates an approximation to a normal distribution in the range of [low, high)  */
FOUNDATION_API int32_t      random32_gaussian_range( int32_t low, int32_t high );

/*! Generates a floating point value with an approximated normal distribution in the range of [low, high) */
FOUNDATION_API real         random_gaussian_range( real low, real high );

/*! Creates a triangle distribution in the range of [low, high). */
FOUNDATION_API int32_t      random32_triangle_range( int32_t low, int32_t high );

/*! Generates a floating point value with a triangular distribution in the range of [low, high) */
FOUNDATION_API real         random_triangle_range( real low, real high );

/*! Generate a weighted pseudorandom number in the [0,limit) range, with probabilities
    equal to the relative weights (probability = weight/total_weight_sum). The weights
    array must have "limit" number of elements. */
FOUNDATION_API uint32_t     random32_weighted( uint32_t limit, const real* weights );

//! Free thread memory used by pseudorandom number generator
FOUNDATION_API void         random_thread_deallocate( void );

