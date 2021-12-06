/* math.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*!

\file math.h
\brief Math functions

Core math functionality, providing single entry points to common math
functions across platforms and floating point notations used (32 or 64 bit real numbers).

Increment/decrement and wrap functions from
http://cellperformance.beyond3d.com/articles/2006/07/increment-and-decrement-wrapping-values.html */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/assert.h>

#if FOUNDATION_COMPILER_INTEL
#include <mathimf.h>
#undef I
#else
#include <math.h>
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wbad-function-cast"
#endif

#if FOUNDATION_SIZE_REAL == 4

/*! Epsilon value. This represents a small number close to zero that can be used
for comparisons or thresholds. Roughly equals 100 floating point units at 1.0 */
#define REAL_EPSILON FLT_EPSILON

/*! Maximum finite number representable in the current real number format */
#define REAL_MAX FLT_MAX

/*! Minimum finite number representable in the current real number format */
#define REAL_MIN FLT_MIN

#else

#define REAL_EPSILON DBL_EPSILON
#define REAL_MAX DBL_MAX
#define REAL_MIN DBL_MIN

#endif

/*! Constant pi (3.141592...) */
#define DBL_PI 3.1415926535897932384626433832795

/*! Constant half pi (1.570796...) */
#define DBL_HALFPI 1.5707963267948966192313216916398

/*! Constant two pi (6.283185...) */
#define DBL_TWOPI 6.2831853071795864769252867665590

/*! Constant square root of two (1.414213...) */
#define DBL_SQRT2 1.4142135623730950488016887242097

/*! Constant square root of three (1.732050...) */
#define DBL_SQRT3 1.7320508075688772935274463415059

/*! Constant number e (2.718281...) */
#define DBL_E 2.7182818284590452353602874713527

/*! Constant natural logarithm of two (0.693147...) */
#define DBL_LOGN2 0.6931471805599453094172321214582

/*! Constant natural logarithm of ten (2.302585...) */
#define DBL_LOGN10 2.3025850929940456840179914546844

/*! Constant pi (3.141592...) */
#define FLT_PI ((float)(DBL_PI))

/*! Constant half pi (1.570796...) */
#define FLT_HALFPI ((float)(DBL_HALFPI))

/*! Constant two pi (6.283185...) */
#define FLT_TWOPI ((float)(DBL_TWOPI))

/*! Constant square root of two (1.414213...) */
#define FLT_SQRT2 ((float)(DBL_SQRT2))

/*! Constant square root of three (1.732050...) */
#define FLT_SQRT3 ((float)(DBL_SQRT3))

/*! Constant number e (2.718281...) */
#define FLT_E ((float)(DBL_E))

/*! Constant natural logarithm of two (0.693147...) */
#define LFT_LOGN2 ((float)(DBL_LOGN2))

/*! Constant natural logarithm of ten (2.302585...) */
#define FLT_LOGN10 ((float)(DBL_LOGN10))

/*! Constant pi (3.141592...) */
#define REAL_PI REAL_C(DBL_PI)

/*! Constant half pi (1.570796...) */
#define REAL_HALFPI REAL_C(DBL_HALFPI)

/*! Constant two pi (6.283185...) */
#define REAL_TWOPI REAL_C(DBL_TWOPI)

/*! Constant square root of two (1.414213...) */
#define REAL_SQRT2 REAL_C(DBL_SQRT2)

/*! Constant square root of three (1.732050...) */
#define REAL_SQRT3 REAL_C(DBL_SQRT3)

/*! Constant number e (2.718281...) */
#define REAL_E REAL_C(DBL_E)

/*! Constant natural logarithm of two (0.693147...) */
#define REAL_LOGN2 REAL_C(DBL_LOGN2)

/*! Constant natural logarithm of ten (2.302585...) */
#define REAL_LOGN10 REAL_C(DBL_LOGN10)

/*! Sine function
\param x Argument
\return Sine of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x);

/*! Cosine function
\param x Argument
\return Cosine of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x);

/*! Tangent function
\param x Argument
\return Tangent of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x);

/*! Arcsine function
\param x Argument
\return Arcsine of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x);

/*! Arccosine function
\param x Argument
\return Arccosine of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x);

/*! Arctangent function
\param x Argument
\return Arctangent of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x);

/*! Calculate the angle in radians between the positive x-axis of a plane and the point given
by the coordinates (x, y) on it. The angle is positive for counter-clockwise angles (upper
half-plane, y > 0), and negative for clockwise angles (lower half-plane, y < 0).
\param x X coordinate
\param y Y coordinate
\return Angle in radians */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y);

/*! Square root function
\param x Argument
\return Square root of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x);

/*! Inverse square root (1/sqrt) function
\param x Argument
\return Inverse square root of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x);

/*! Absolute function
\param x Argument
\return Absulute of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x);

/*! Floating point modulo function
\param x Value
\param y Base
\return x modulo y */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y);

/*! Natural exponential function
\param x Argument
\return e^x */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x);

/*! Power (exponential) function
\param x Base
\param y Exponent
\return x^y */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y);

/*! Natural logarithm function
\param x Argument
\return Natural logarithm of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x);

/*! Binary logarithm (base 2) function
\param x Argument
\return Binary logarithm of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_log2(real x);

/*! Floor (largest previous integer) function
\param x Argument
\return Largest integer not greater than x */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x);

/*! Floor (largest previous 64 bit integer) function
\param x Argument
\return Largest 64 bit integer not greater than x */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x);

/*! Ceiling (smallest following integer) function
\param x Argument
\return Smallest integer greater than x */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x);

/*! Ceiling (smallest following 64 bit integer) function
\param x Argument
\return Smallest 64 bit integer greater than x */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x);

/*! Round to nearest floating point function
\param x Argument
\return Argument rounded to nearest integer */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x);

/*! Truncation (integral part) function
\param x Argument
\return Integral part of argument */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x);

/*! Calculate smallest greater power-of-two. Argument MUST be > 1 or results are undefined
\param x Argument (MUST be >1)
\return Smallest greater power-of-two (or x if it is a power-of-two) */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL unsigned int
math_align_poweroftwo(unsigned int x);

/*! Query if argument is power-of-two
\param x Argument
\return true if x is a power-of-two, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_is_poweroftwo(unsigned int x);

/*! Calculate smallest greater multiple of the given base
\param x Argument
\param alignment Alignment
\return Smallest greater multiple of the given alignment */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL unsigned int
math_align_up(unsigned int x, unsigned int alignment);

/*! Scalar interpolation function with zero first-order derivative at endpoints,
smoothstep(t) = 3t^2 - 2t^3
\param t Argument in [0..1]
\return Smoothed value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_smoothstep(real t);

/*! Scalar interpolation function with zero first-order and second-order derivative
at endpoints, smoothstep(t) = 6t^5 - 15t^4 + 10t^3
\param t Argument in [0..1]
\return Smoothed value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_smootherstep(real t);

/*! Linear interpolation function
\param t Interpolation factor in [0..1] range
\param x Start value
\param y End value
\return Linear interpolation, x + t * (y-x) */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_lerp(real t, real x, real y);

/*! Inverse linear interpolation function
\param v Current value
\param x Start value
\param y End value
\return Linear interpolation factor, t = (v-x) / (y-x) */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_unlerp(real v, real x, real y);

/*! Remap a value from source interval to destination interval
\param x Current value
\param xmin Start of source interval
\param xmax End of source interval
\param ymin Start of destination interval
\param ymax End of destination interval
\return Linear remapped value <code>v = ymin + ((x-xmin) / (xmax-xmin)) * (ymax-ymin)</code> */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_linear_remap(real x, real xmin, real xmax, real ymin, real ymax);

/*! Get maximum of two values. Note that as a macro it can evaluate arguments multiple times.
\param x First value
\param y Second value
\return Maximum of x and y */
#define math_max(x, y) ((x) < (y) ? (y) : (x))

/*! Get minimum of two values. Note that as a macro it can evaluate arguments multiple times.
\param x First value
\param y Second value
\return Minimum of x and y */
#define math_min(x, y) ((x) < (y) ? (x) : (y))

/*! Clamp value to interval. Note that as a macro it can evaluate arguments multiple times.
\param x Value to clamp
\param minval Start of interval
\param maxval End of interval
\return Value x clamped to [minval,maxval] interval */
#define math_clamp(x, minval, maxval) ((x) < (minval) ? (minval) : ((x) > (maxval) ? (maxval) : (x)))

/*! Compare two floats with epsilon tolerance expressed as number of adjacent float values.
\param rval First float
\param lval Second float
\param ulps Number of float values in tolerance
\return True if float values are in tolerance range of eachother, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_eq(real rval, real lval, int ulps);

/*! Compare two floats with epsilon tolerance expressed as number of adjacent float values.
This method (unlike #math_real_eq) is not sign safe, comparing -x to x for small values
of x will yield incorrect results. Most notably will -0 and +0 not be equal.
\param rval First float
\param lval Second float
\param ulps Number of float values in tolerance
\return True if float values are in tolerance range of eachother, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_eqns(real rval, real lval, int ulps);

/*! Test if value is zero. A value is interpreted as zero if it is either positive
or negative zero, or a denormalized value (i.e less than smallest normalized value)
\param val Float
\return True if float is within epsilon distance of zero */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_zero(real val);

/*! Test if value is within epsilon distance of zero
\param val Float
\return True if float is within epsilon distance of zero */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_epsilon_zero(real val);

/*! Test if float is near one (within epsilon distance)
\param val Float
\return True if float is within epsilon distance of one */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_one(real val);

/*! Decrease a floating point value the given number of units. Safe to traverse
the zero threshold and switch sign
\param val Float
\param units Units
\return Resulting float */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_real_dec(real val, int units);

/*! Increase a floating point value the given number of units
\param val Float
\param units Units
\return Resulting float */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_real_inc(real val, int units);

/*! Query if value is NaN (not-a-number, exponent all 1, fraction non-zero)
<pre>+NAN float: 0x7f8xxxxx
-NAN float: 0x7f8xxxxx
+NAN double: 0x7ffxxxxxxxxxxxxx
-NAN double: 0x7ffxxxxxxxxxxxxx</pre>
\param val Value
\return true if NaN, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_nan(real val);

/*! Query if value is infinite (exponent all 1, fraction all 0)
<pre>+inf float: 0x7f800000
-inf float: 0x7f800000
+inf double: 0x7ff0000000000000
-inf double: 0x7ff0000000000000</pre>
\param val Value
\return true if infinite, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_inf(real val);

/*! Query if value is a compiler specific uninitialized value identifier
\param val Value
\return true if value is an uninitialized value, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_uninitialized(real val);

/*! Query if value is finite (not NaN, inf or uninitialized)
\param val Value
\return true if finite, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_finite(real val);

/*! Query if value is denormalized (close to zero, exponent all 0)
\param val Value
\return true if value is denormalized, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_denormalized(real val);

/*! Un-denormalize a value by clamping denormals to zero
\param val Value
\return 0 if value is denormalized, value if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_real_undenormalize(real val);

#if BUILD_ENABLE_ASSERT
/*! Assert that a value is finite. Like all assert macros it will evaluate to a void
expression if asserts are disabled.
\param value Value */
#define FOUNDATION_ASSERT_FINITE(value)                                               \
	/*lint -save -e717 */ do {                                                        \
		FOUNDATION_ASSERT_MSG(math_real_isfinite((value)), "Non-finite float value"); \
	} while (0) /*lint -restore */
#else
#define FOUNDATION_ASSERT_FINITE(value) \
	/*lint -save -e717 */ do { (void)sizeof(value); } while (0) /*lint -restore */
#endif

/*!
\fn math_inc_wrap_uint8
\brief Increment and wrap
\details Increment and wrap unsigned 8-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_uint16
\brief Increment and wrap
\details Increment and wrap unsigned 16-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_uint32
\brief Increment and wrap
\details Increment and wrap unsigned 32-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_uint64
\brief Increment and wrap
\details Increment and wrap unsigned 64-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_int8
\brief Increment and wrap
\details Increment and wrap signed 8-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_int16
\brief Increment and wrap
\details Increment and wrap signed 16-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_int32
\brief Increment and wrap
\details Increment and wrap signed 32-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_inc_wrap_int64
\brief Increment and wrap
\details Increment and wrap signed 64-bit value inside given range
\param val Value to increment
\param min Start of range
\param max End of range
\return Incremented and wrapped value inside range

\fn math_dec_wrap_uint8
\brief Decrement and wrap
\details Decrement and wrap unsigned 8-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_uint16
\brief Decrement and wrap
\details Decrement and wrap unsigned 16-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_uint32
\brief Decrement and wrap
\details Decrement and wrap unsigned 32-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_uint64
\brief Decrement and wrap
\details Decrement and wrap unsigned 64-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_int8
\brief Decrement and wrap
\details Decrement and wrap signed 8-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_int16
\brief Decrement and wrap
\details Decrement and wrap signed 16-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_int32
\brief Decrement and wrap
\details Decrement and wrap signed 32-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range

\fn math_dec_wrap_int64
\brief Decrement and wrap
\details Decrement and wrap signed 64-bit value inside given range
\param val Value to decrement
\param min Start of range
\param max End of range
\return Decremented and wrapped value inside range
*/

#define FOUNDATION_DECLARE_INCREMENT_AND_WRAP(suffix, type, signed_type, bit_mask)                                 \
	static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL type math_inc_wrap_##suffix(const type val, const type min, \
	                                                                               const type max);                \
	static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL type math_inc_wrap_##suffix(const type val, const type min, \
	                                                                               const type max) {               \
		const type increased = val + 1;                                                                            \
		const type max_diff = max - val;                                                                           \
		const type max_diff_nz = (type)(((signed_type)max_diff | -(signed_type)max_diff) >> bit_mask);             \
		const type max_diff_eqz = ~max_diff_nz;                                                                    \
		const type result = (increased & max_diff_nz) | (min & max_diff_eqz);                                      \
		return result;                                                                                             \
	}

FOUNDATION_DECLARE_INCREMENT_AND_WRAP(uint8, uint8_t, int8_t, 7)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(uint16, uint16_t, int16_t, 15)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(uint32, uint32_t, int32_t, 31)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(uint64, uint64_t, int64_t, 63ULL)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(int8, int8_t, int8_t, 7)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(int16, int16_t, int16_t, 15)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(int32, int32_t, int32_t, 31)
FOUNDATION_DECLARE_INCREMENT_AND_WRAP(int64, int64_t, int64_t, 63ULL)
#undef FOUNDATION_DECLARE_INCREMENT_AND_WRAP

#define FOUNDATION_DECLARE_DECREMENT_AND_WRAP(suffix, type, signed_type, bit_mask)                                 \
	static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL type math_dec_wrap_##suffix(const type val, const type min, \
	                                                                               const type max);                \
	static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL type math_dec_wrap_##suffix(const type val, const type min, \
	                                                                               const type max) {               \
		const type decreased = val - 1;                                                                            \
		const type min_diff = min - val;                                                                           \
		const type min_diff_nz = (type)(((signed_type)min_diff | -(signed_type)min_diff) >> bit_mask);             \
		const type min_diff_eqz = ~min_diff_nz;                                                                    \
		const type result = (decreased & min_diff_nz) | (max & min_diff_eqz);                                      \
		return result;                                                                                             \
	}

FOUNDATION_DECLARE_DECREMENT_AND_WRAP(uint8, uint8_t, int8_t, 7)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(uint16, uint16_t, int16_t, 15)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(uint32, uint32_t, int32_t, 31)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(uint64, uint64_t, int64_t, 63ULL)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(int8, int8_t, int8_t, 7)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(int16, int16_t, int16_t, 15)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(int32, int32_t, int32_t, 31)
FOUNDATION_DECLARE_DECREMENT_AND_WRAP(int64, int64_t, int64_t, 63ULL)
#undef FOUNDATION_DECLARE_DECREMENT_AND_WRAP

// Implementation

#ifndef FOUNDATION_PLATFORM_DOXYGEN

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL unsigned int
math_align_poweroftwo(unsigned int x) {
	FOUNDATION_ASSERT(x > 1);

#if FOUNDATION_COMPILER_INTEL && (FOUNDATION_ARCH_X86 || FOUNDATION_ARCH_X86_64)
	--x;
	__asm__("bsrl %1,%0" : "=r"(x) : "rm"(x));
	return (1 << (x + 1));
#else
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return ++x;
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_is_poweroftwo(unsigned int x) {
	return (x & (x - 1)) == 0;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL unsigned int
math_align_up(unsigned int x, unsigned int alignment) {
	unsigned int aligned = x;
	unsigned int remain = x % alignment;
	if (remain)
		aligned += alignment - remain;
	return aligned;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_smoothstep(real t) {
	return (REAL_C(3.0) - REAL_C(2.0) * t) * (t * t);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_smoothstep_double(double t) {
	return (3.0 - 2.0 * t) * (t * t);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_smootherstep(real t) {
	return (t * t * t) * (t * (t * REAL_C(6.0) - REAL_C(15.0)) + REAL_C(10.0));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_smootherstep_double(double t) {
	return (t * t * t) * (t * (t * 6.0 - 15.0) + 10.0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_lerp(real t, real x, real y) {
	return (x + (t * (y - x)));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_lerp_double(double t, double x, double y) {
	return (x + (t * (y - x)));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_unlerp(real v, real x, real y) {
	return ((v - x) / (y - x));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_unlerp_double(double v, double x, double y) {
	return ((v - x) / (y - x));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_linear_remap(real x, real xmin, real xmax, real ymin, real ymax) {
	return math_lerp(math_unlerp(x, xmin, xmax), ymin, ymax);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_log2(real x) {
	return math_logn(x) * REAL_C(1.4426950408889634073599246810019);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_real_is_epsilon_zero(real val) {
	return (val > -REAL_EPSILON) && (val < REAL_EPSILON);
}

#if FOUNDATION_COMPILER_MSVC

#if FOUNDATION_SIZE_REAL == 8

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return sin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return cos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return tan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return asin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return acos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return atan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return atan2(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return sqrt(x);
}
#if FOUNDATION_COMPILER_MSVC
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return REAL_C(1.0) / sqrt(x);
}
#else
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return invsqrt(x);
}
#endif
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return fabs(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return fmod(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return exp(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return pow(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return log(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)ceil(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)x;
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)ceil(x);
}

#else

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return sinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return cosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return tanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return asinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return acosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return atanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return atan2f(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return sqrtf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return REAL_C(1.0) / sqrtf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return (real)fabs(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return fmodf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return expf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return powf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return logf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)floorf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)ceilf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5f);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)x;
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)ceil(x);
}

#endif

#elif FOUNDATION_COMPILER_INTEL

#if FOUNDATION_SIZE_REAL == 8

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return sin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return cos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return tan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return asin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return acos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return atan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return atan2(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return sqrt(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return invsqrt(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return fabs(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return fmod(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return exp(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return pow(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return log(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)ceil(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5f);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)x;
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)ceil(x);
}

#else

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return sinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return cosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return tanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return asinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return acosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return atanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return atan2f(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return sqrtf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return invsqrtf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return fabsf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return fmodf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return expf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return powf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return logf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)floorf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)ceilf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5f);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)x;
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)ceil(x);
}

#endif

#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG

#if FOUNDATION_SIZE_REAL == 8

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return __builtin_sin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return __builtin_cos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return __builtin_tan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return __builtin_asin(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return __builtin_acos(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return __builtin_atan(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return __builtin_atan2(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return __builtin_sqrt(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return (REAL_C(1.0) / __builtin_sqrt(x));
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return __builtin_fabs(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return __builtin_fmod(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return __builtin_exp(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return __builtin_pow(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return __builtin_log(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)__builtin_floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)__builtin_ceil(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)__builtin_floor(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)__builtin_ceil(x);
}
#if FOUNDATION_PLATFORM_APPLE
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)(x);
}
#else
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)__builtin_round(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)__builtin_trunc(x);
}
#endif

#else

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sin(real x) {
	return __builtin_sinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_cos(real x) {
	return __builtin_cosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_tan(real x) {
	return __builtin_tanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_asin(real x) {
	return __builtin_asinf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_acos(real x) {
	return __builtin_acosf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan(real x) {
	return __builtin_atanf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_atan2(real x, real y) {
	return __builtin_atan2f(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_sqrt(real x) {
	return __builtin_sqrtf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_rsqrt(real x) {
	return (REAL_C(1.0) / __builtin_sqrtf(x));
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_abs(real x) {
	return __builtin_fabsf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_mod(real x, real y) {
	return __builtin_fmodf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_exp(real x) {
	return __builtin_expf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_pow(real x, real y) {
	return __builtin_powf(x, y);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL real
math_logn(real x) {
	return __builtin_logf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_ceil(real x) {
	return (int)__builtin_ceilf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_floor(real x) {
	return (int)__builtin_floorf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_ceil64(real x) {
	return (int64_t)__builtin_ceilf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int64_t
math_floor64(real x) {
	return (int64_t)__builtin_floorf(x);
}
#if FOUNDATION_PLATFORM_APPLE
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)(x + 0.5f);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)(x);
}
#else
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_round(real x) {
	return (int)__builtin_roundf(x);
}
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL int
math_trunc(real x) {
	return (int)__builtin_truncf(x);
}
#endif

#endif

#else
#error Unknown compiler
#endif

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_nan(double val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float64_cast_t conv = {.fval = val};
#else
	float64_cast_t conv;
	conv.fval = val;
#endif
	return (((conv.uival & 0x7FF0000000000000ULL) >> 52ULL) == 0x7ff) & ((conv.uival & 0xFFFFFFFFFFFFFULL) != 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_inf(double val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float64_cast_t conv = {.fval = val};
#else
	float64_cast_t conv;
	conv.fval = val;
#endif
	return (((conv.uival & 0x7FF0000000000000ULL) >> 52ULL) == 0x7ff) & ((conv.uival & 0xFFFFFFFFFFFFFULL) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_uninitialized(double val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float64_cast_t conv = {.fval = val};
#else
	float64_cast_t conv;
	conv.fval = val;
#endif
	return (conv.uival == 0xCDCDCDCDCDCDCDCDULL) | (conv.uival == 0xFEEEFEEEFEEEFEEEULL);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_finite(double val) {
	return !(math_double_is_nan(val) || math_double_is_inf(val) || math_double_is_uninitialized(val));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_denormalized(double val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float64_cast_t conv = {.fval = val};
#else
	float64_cast_t conv;
	conv.fval = val;
#endif
	return conv.uival && (conv.uival != 0x8000000000000000ULL) && ((conv.uival & 0x7F80000000000000ULL) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_double_undenormalize(double val) {
	return math_double_is_denormalized(val) ? 0 : val;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_eq(double a, double b, int64_t ulps) {
	float64_cast_t ca;
	float64_cast_t cb;
	int64_t ai, bi, diff;
	ca.fval = a;
	cb.fval = b;

	ai = ca.ival;
	if (ai < 0)
		ai = (int64_t)0x8000000000000000LL - ai;

	bi = cb.ival;
	if (bi < 0)
		bi = (int64_t)0x8000000000000000LL - bi;

	diff = (ai - bi);
	if ((diff <= ulps) && (diff >= -ulps))
		return true;

	return false;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_eqns(double a, double b, int64_t ulps) {
	float64_cast_t ca;
	float64_cast_t cb;
	int64_t diff;

	ca.fval = a;
	cb.fval = b;

	diff = (ca.ival - cb.ival);
	if ((diff <= ulps) && (diff >= -ulps))
		return true;
	return false;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_zero(double val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float64_cast_t conv = {.fval = val};
#else
	float64_cast_t conv;
	conv.fval = val;
#endif
	return !conv.uival || (conv.uival == 0x8000000000000000ULL) || ((conv.uival & 0x7FF0000000000000ULL) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_double_is_one(double val) {
	return math_double_eq(val, 1.0, 100);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_double_dec(double val, int units) {
	float64_cast_t ca, cb;

	ca.fval = val;
	cb.ival = ca.ival - (ca.ival < 0 ? -units : units);
	if ((cb.ival < 0 && ca.ival >= 0) || (cb.ival > 0 && ca.ival < 0))
		cb.ival = (int64_t)0x8000000000000000LL - cb.ival;

	return cb.fval;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL double
math_double_inc(double val, int units) {
	float64_cast_t ca, cb;

	ca.fval = val;
	cb.ival = ca.ival + (ca.ival < 0 ? -units : units);
	if ((cb.ival < 0 && ca.ival >= 0) || (cb.ival > 0 && ca.ival < 0))
		cb.ival = (int64_t)0x8000000000000000LL - cb.ival;

	return cb.fval;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_nan(float val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float32_cast_t conv = {.fval = val};
#else
	float32_cast_t conv;
	conv.fval = val;
#endif
	return (((conv.uival & 0x7F800000) >> 23) == 0xFF) & ((conv.uival & 0x7FFFFF) != 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_inf(float val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float32_cast_t conv = {.fval = val};
#else
	float32_cast_t conv;
	conv.fval = val;
#endif
	return (((conv.uival & 0x7F800000) >> 23) == 0xFF) & ((conv.uival & 0x7FFFFF) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_uninitialized(float val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float32_cast_t conv = {.fval = val};
#else
	float32_cast_t conv;
	conv.fval = val;
#endif
	// Some common debugger uninitialized filler values
	return (conv.uival == 0xFEEEFEEEU) || (conv.uival == 0xCDCDCDCDU);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_finite(float val) {
	return !(math_float_is_nan(val) || math_float_is_inf(val) || math_float_is_uninitialized(val));
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_denormalized(float val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float32_cast_t conv = {.fval = val};
#else
	float32_cast_t conv;
	conv.fval = val;
#endif
	return conv.uival && (conv.uival != 0x80000000) && ((conv.uival & 0x7F800000ULL) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL float
math_float_undenormalize(float val) {
	return math_float_is_denormalized(val) ? 0 : val;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_eq(float a, float b, int ulps) {
	float32_cast_t ca;
	float32_cast_t cb;
	int32_t ai, bi, diff;
	ca.fval = a;
	cb.fval = b;

	ai = ca.ival;
	if (ai < 0)
		ai = (int32_t)0x80000000 - ai;

	bi = cb.ival;
	if (bi < 0)
		bi = (int32_t)0x80000000 - bi;

	diff = (ai - bi);
	if ((diff <= ulps) && (diff >= -ulps))
		return true;

	return false;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_eqns(float a, float b, int ulps) {
	float32_cast_t ca;
	float32_cast_t cb;
	int32_t diff;
	ca.fval = a;
	cb.fval = b;

	diff = (ca.ival - cb.ival);
	if ((diff <= ulps) && (diff >= -ulps))
		return true;
	return false;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_zero(float val) {
#if !defined(__cplusplus) && !FOUNDATION_COMPILER_MSVC
	const float32_cast_t conv = {.fval = val};
#else
	float32_cast_t conv;
	conv.fval = val;
#endif
	return !conv.uival || (conv.uival == 0x80000000) || ((conv.uival & 0x7F800000ULL) == 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
math_float_is_one(float val) {
	return math_float_eq(val, 1.0f, 100);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL float
math_float_dec(float val, int units) {
	float32_cast_t ca, cb;

	ca.fval = val;
	cb.ival = ca.ival - (ca.ival < 0 ? -units : units);
	if ((cb.ival < 0 && ca.ival >= 0) || (cb.ival > 0 && ca.ival < 0))
		cb.ival = (int32_t)0x80000000 - cb.ival;

	return cb.fval;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL float
math_float_inc(float val, int units) {
	float32_cast_t ca, cb;

	ca.fval = val;
	cb.ival = ca.ival + (ca.ival < 0 ? -units : units);
	if ((cb.ival < 0 && ca.ival >= 0) || (cb.ival > 0 && ca.ival < 0))
		cb.ival = (int32_t)0x80000000 - cb.ival;

	return cb.fval;
}

#endif

#if FOUNDATION_SIZE_REAL == 8
#define math_real_is_nan(x) math_double_is_nan(x)
#define math_real_is_inf(x) math_double_is_inf(x)
#define math_real_is_uninitialized(x) math_double_is_uninitialized(x)
#define math_real_is_finite(x) math_double_is_finite(x)
#define math_real_is_denormalized(x) math_double_is_denormalized(x)
#define math_real_undenormalize(x) math_double_undenormalize(x)
#define math_real_eq(x, y, ulps) math_double_eq(x, y, ulps)
#define math_real_eqns(x, y, ulps) math_double_eqns(x, y, ulps)
#define math_real_is_zero(x) math_double_is_zero(x)
#define math_real_is_one(x) math_double_is_one(x)
#define math_real_dec(x, units) math_double_dec(x, units)
#define math_real_inc(x, units) math_double_inc(x, units)
#else
#define math_real_is_nan(x) math_float_is_nan(x)
#define math_real_is_inf(x) math_float_is_inf(x)
#define math_real_is_uninitialized(x) math_float_is_uninitialized(x)
#define math_real_is_finite(x) math_float_is_finite(x)
#define math_real_is_denormalized(x) math_float_is_denormalized(x)
#define math_real_undenormalize(x) math_float_undenormalize(x)
#define math_real_eq(x, y, ulps) math_float_eq(x, y, ulps)
#define math_real_eqns(x, y, ulps) math_float_eqns(x, y, ulps)
#define math_real_is_zero(x) math_float_is_zero(x)
#define math_real_is_one(x) math_float_is_one(x)
#define math_real_dec(x, units) math_float_dec(x, units)
#define math_real_inc(x, units) math_float_inc(x, units)
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
