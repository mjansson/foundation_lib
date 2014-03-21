/* mathcore.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file math.h
    Core math functionality, providing single entry points to common math
    functions across platforms and floating point notations used (32 or 64 bit real numbers).

    Increment/decrement and wrap functions from http://cellperformance.beyond3d.com/articles/2006/07/increment-and-decrement-wrapping-values.html */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/assert.h>

#if FOUNDATION_COMPILER_INTEL
#  include <mathimf.h>
#  undef I
#else
#  include <math.h>
#endif


/*! \def REAL_EPSILON
    Epsilon value. This represents a small number close to zero that can be used for comparisons or thresholds. Roughly equals 100 floating point units at 1.0 */

/*! \def REAL_MAX
    Maximum finite number representable in the current real number format */

/*! \def REAL_MIN
    Minimum finite number representable in the current real number format */

/*! \def REAL_ZERO
    Constant zero (0.0) */

/*! \def REAL_ONE
    Constant one (1.0) */

/*! \def REAL_TWO
    Constant two (2.0) */

/*! \def REAL_THREE
    Constant three (3.0) */

/*! \def REAL_FOUR
    Constant four (4.0) */

/*! \def REAL_HALF
    Constant half (0.5) */

/*! \def REAL_QUARTER
    Constant quarter (0.25) */

/*! \def REAL_PI
    Constant pi (3.141592...) */

/*! \def REAL_HALFPI
    Constant half pi (1.570796...) */

/*! \def REAL_TWOPI
    Constant two pi (6.283185...) */

/*! \def REAL_SQRT2
    Constant square root of two (1.414213...) */

/*! \def REAL_SQRT3
    Constant square root of three (1.732050...) */

/*! \def REAL_E
    Constant number e (2.718281...) */

/*! \def REAL_LOGN2
    Constant natural logarithm of two (0.693147...) */

/*! \def REAL_LOGN10
    Constant natural logarithm of ten (2.302585...) */

/*! \fn math_sin
    Sine function
    \param x                               Argument
    \return                                Sine of argument */

/*! \fn math_cos
    Cosine function
    \param x                               Argument
    \return                                Cosine of argument */

/*! \fn math_tan
    Tangent function
    \param x                               Argument
    \return                                Tangent of argument */

/*! \fn math_asin
    ArcSine function
    \param x                               Argument
    \return                                Arcsine of argument */

/*! \fn math_acos
    Arccosine function
    \param x                               Argument
    \return                                Arccosine of argument */

/*! \fn math_atan
    Arctangent function
    \param x                               Argument
    \return                                Arctangent of argument */

/*! \fn math_atan2
    Calculate the angle in radians between the positive x-axis of a plane and the point given by the coordinates (x, y) on it. The angle is positive for counter-clockwise angles (upper half-plane, y > 0), and negative for clockwise angles (lower half-plane, y < 0).
    \param x                               X coordinate
    \param y                               Y coordinate
    \return                                Angle in radians */

/*! \fn math_sqrt
    Square root function
    \param x                               Argument
    \return                                Square root of argument */

/*! \fn math_rsqrt
    Inverse squareroot (1/sqrt) function
    \param x                               Argument
    \return                                Inverse square root of argument */

/*! \fn math_abs
    Absolute function
    \param x                               Argument
    \return                                Absulute of argument */

/*! \fn math_mod
    Floating point modulo function
    \param x                               Value
    \param y                               Base
    \return                                x modulo y */

/*! \fn math_exp
    Natural exponential function
    \param x                               Argument
    \return                                e^x */

/*! \fn math_pow
    Power (exponential) function
    \param x                               Base
    \param y                               Exponent
    \return                                x^y */

/*! \fn math_logn
    Natural logarithm function
    \param x                               Argument
    \return                                Natural logarithm of argument */

/*! \fn math_log2
    Binary logarithm (base 2) function
    \param x                               Argument
    \return                                Binary logarithm of argument */

/*! \fn math_floor
    Floor (largest previous integer) function
    \param x                               Argument
    \return                                Largest integer not greater than x */

/*! \fn math_ceil
    Ceiling (smallest following integer) function
    \param x                               Argument
    \return                                Smallest integer greater than x */

/*! \fn math_floor64
    Floor (largest previous 64 bit integer) function
    \param x                               Argument
    \return                                Largest 64 bit integer not greater than x */

/*! \fn math_ceil
    Ceiling (smallest following 64 bit integer) function
    \param x                               Argument
    \return                                Smallest 64 bit integer greater than x */

/*! \fn math_round
    Round to nearest floating point function
    \param x                               Argument
    \return                                Argument rounded to nearest integer */

/*! \fn math_trunc
    Truncation (integral part) function
    \param x                               Argument
    \return                                Integral part of argument */

/*! \fn math_align_poweroftwo
    Calculate smallest greater power-of-two. Argument MUST be > 1 or results are undefined
    \param x                               Argument (MUST be >1)
    \return                                Smallest greater power-of-two (or x if it is a power-of-two) */

/*! \fn math_is_poweroftwo
    Query if power-of-two
    \param x                               Argument
    \return                                true if x is a power-of-two, false if not */

/*! \fn math_align_up
    Calculate smallest greater multiple of the given base
    \param x                               Argument
    \param alignment                       Alignment
    \return                                Smallest greater multiple of the given alignment */

/*! \fn math_smoothstep
    Scalar interpolation function with zero first-order derivative at endpoints. smoothstep(t) = 3t^2 - 2t^3
    \param t                               Argument in [0..1]
    \return                                Smoothed value */

/*! \fn math_smootherstep
    Scalar interpolation function with zero first-order and second-order derivative at endpoints. smoothstep(t) = 6t^5 - 15t^4 + 10t^3
    \param t                               Argument in [0..1]
    \return                                Smoothed value */

/*! \fn math_lerp
    Linear interpolation function
    \param t                               Interpolation factor in [0..1] range
    \param x                               Start value
    \param y                               End value
    \return                                Linear interpolation, x + t * (y-x) */

/*! \fn math_unlerp
    Inverse linear interpolation function
    \param v                               Current value
    \param x                               Start value
    \param y                               End value
    \return                                Linear interpolation factor, t = (v-x) / (y-x) */

/*! \fn math_linear_remap
    Remap a value from source interval to destination interval
    \param x                               Current value
    \param xmin                            Start of source interval
    \param xmax                            End of source interval
    \param ymin                            Start of destination interval
    \param ymax                            End of destination interval
    \return                                Linear remapped value v = ymin + ((x-xmin) / (xmax-xmin)) * (ymax-ymin) */

/*! \def math_clamp
    Clamp value to interval. Note that as a macro it can evaluate arguments multiple times!
    \param x                               Value to clamp
    \param minval                          Start of interval
    \param maxval                          End of interval
    \return                                Value x clamped to [minval,maxval] interval */

/*! \fn math_realeq
    Compare two floats with epsilon tolerance expressed as number of adjacent float values
    \param rval                            First float
    \param lval                            Second float
    \param ulps                            Number of float values in tolerance
    \return                                True if float values are in tolerance range of eachother, false if not */

/*! \fn math_realeqns
    Compare two floats with epsilon tolerance expressed as number of adjacent float values. This
    method (unlike math_realeq) is not sign safe, comparing -x to x for small values of x will yield incorrect results. Most notably will -0 and +0 not be equal.
    \param rval                            First float
    \param lval                            Second float
    \param ulps                            Number of float values in tolerance
    \return                                True if float values are in tolerance range of eachother, false if not */

/*! \fn math_realzero
    Test if float is near zero (within epsilon distance)
    \param val                             Float
    \return                                True if float is within epsilon distance of zero */

/*! \fn math_realone
    Test if float is near one (within epsilon distance)
    \param val                             Float
    \return                                True if float is within epsilon distance of one */

/*! \fn math_realdec
    Decrease a floating point value the given number of units
    \param val                             Float
    \param units                           Units
    \return                                Resulting float */

/*! \fn math_realinc
    Increase a floating point value the given number of units
    \param val                             Float
    \param units                           Units
    \return                                Resulting float */

/*! \fn math_realisnan
    Query if value is NaN
    +NAN float: 0x7fc00000
    -NAN float: 0x7fc00000
    +NAN double: 0x7ff8000000000000
    -NAN double: 0x7ff8000000000000
    \param val                             Value
    \return                                true if NaN, false if not */

/*! \fn math_realisinf
    Queryif value is infinite
    +INF float: 0x7f800000
    -INF float: 0x7f800000
    +INF double: 0x7ff0000000000000
    -INF double: 0x7ff0000000000000
    \param val                             Value
    \return                                true if infinite, false if not */

/*! \fn math_realisuninitialized
    Query if value is a compiler specific uninitialized value identifier
    \param val                             Value
    \return                                true if value is an uninitialized value, false if not */

/*! \fn math_realisfinite
    Query if value is finite (not NaN, inf or uninitialized)
    \param val                             Value
    \return                                true if finite, false if not */

/*! \fn math_realisdenormalized
    Query if value is denormalized
    \param val                             Value
    \return                                true if value is denormalized, false if not */

/*! \fn math_realundenormalize
    Un-denormalize a value
    \param val                             Value
    \return                                0 if value is denormalized, value if not */

/*! \def FOUNDATION_ASSERT_FINITE
    Assert that a value is finite
    \param value                           Value */

/*! \fn math_inc_wrap_uint8
    Increment and wrap unsigned 8-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_uint16
    Increment and wrap unsigned 16-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_uint32
    Increment and wrap unsigned 32-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_uint64
    Increment and wrap unsigned 64-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_int8
    Increment and wrap signed 8-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_int16
    Increment and wrap signed 16-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_int32
    Increment and wrap signed 32-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_inc_wrap_int64
    Increment and wrap signed 64-bit value inside given range
    \param val                             Value to increment
    \param min                             Start of range
    \param max                             End of range
    \return                                Incremented and wrapped value inside range */

/*! \fn math_dec_wrap_uint8
    Decrement and wrap unsigned 8-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_uint16
    Decrement and wrap unsigned 16-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_uint32
    Decrement and wrap unsigned 32-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_uint64
    Decrement and wrap unsigned 64-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_int8
    Decrement and wrap signed 8-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_int16
    Decrement and wrap signed 16-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_int32
    Decrement and wrap signed 32-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */

/*! \fn math_dec_wrap_int64
    Decrement and wrap signed 64-bit value inside given range
    \param val                             Value to decrement
    \param min                             Start of range
    \param max                             End of range
    \return                                Decremented and wrapped value inside range */


#if FOUNDATION_PLATFORM_REALSIZE == 64

#define REAL_EPSILON                       0.00000000000002

#define REAL_MAX                           DBL_MAX
#define REAL_MIN                           DBL_MIN

#else

#define REAL_EPSILON                       0.00001f

#define REAL_MAX                           FLT_MAX
#define REAL_MIN                           FLT_MIN

#endif

#define REAL_ZERO                          REAL_C( 0.0 )
#define REAL_ONE                           REAL_C( 1.0 )
#define REAL_TWO                           REAL_C( 2.0 )
#define REAL_THREE                         REAL_C( 3.0 )
#define REAL_FOUR                          REAL_C( 4.0 )
#define REAL_HALF                          REAL_C( 0.5 )
#define REAL_QUARTER                       REAL_C( 0.25 )

#define REAL_PI                            REAL_C( 3.1415926535897932384626433832795 )
#define REAL_HALFPI                        REAL_C( 1.5707963267948966192313216916398 )
#define REAL_TWOPI                         REAL_C( 6.2831853071795864769252867665590 )
#define REAL_SQRT2                         REAL_C( 1.4142135623730950488016887242097 )
#define REAL_SQRT3                         REAL_C( 1.7320508075688772935274463415059 )
#define REAL_E                             REAL_C( 2.7182818284590452353602874713527 )
#define REAL_LOGN2                         REAL_C( 0.6931471805599453094172321214582 )
#define REAL_LOGN10                        REAL_C( 2.3025850929940456840179914546844 )

static FORCEINLINE CONSTCALL real          math_sin( real x );
static FORCEINLINE CONSTCALL real          math_cos( real x );
static FORCEINLINE CONSTCALL real          math_tan( real x );
static FORCEINLINE CONSTCALL real          math_asin( real x );
static FORCEINLINE CONSTCALL real          math_acos( real x );
static FORCEINLINE CONSTCALL real          math_atan( real x );
static FORCEINLINE CONSTCALL real          math_atan2( real x, real y );
static FORCEINLINE CONSTCALL real          math_sqrt( real x );
static FORCEINLINE CONSTCALL real          math_rsqrt( real x );
static FORCEINLINE CONSTCALL real          math_abs( real x );
static FORCEINLINE CONSTCALL real          math_mod( real x, real y );
static FORCEINLINE CONSTCALL real          math_exp( real x );
static FORCEINLINE CONSTCALL real          math_pow( real x, real y );
static FORCEINLINE CONSTCALL real          math_logn( real x );
static FORCEINLINE CONSTCALL real          math_log2( real x );

static FORCEINLINE CONSTCALL int           math_floor( real x );
static FORCEINLINE CONSTCALL int           math_ceil( real x );
static FORCEINLINE CONSTCALL int           math_round( real x );
static FORCEINLINE CONSTCALL int           math_trunc( real x );

static FORCEINLINE CONSTCALL int64_t       math_floor64( real x );
static FORCEINLINE CONSTCALL int64_t       math_ceil64( real x );

static FORCEINLINE CONSTCALL unsigned int  math_align_poweroftwo( unsigned int x );
static FORCEINLINE CONSTCALL bool          math_is_poweroftwo( unsigned int x );

static FORCEINLINE CONSTCALL unsigned int  math_align_up( unsigned int x, unsigned int alignment );

static FORCEINLINE CONSTCALL real          math_smoothstep( real t );
static FORCEINLINE CONSTCALL real          math_smootherstep( real t );

static FORCEINLINE CONSTCALL real          math_lerp( real t, real x, real y );
static FORCEINLINE CONSTCALL real          math_unlerp( real v, real x, real y );
static FORCEINLINE CONSTCALL real          math_linear_remap( real x, real xmin, real xmax, real ymin, real ymax );

#define                                    math_clamp( x, minval, maxval ) ( (x) < (minval) ? (minval) : ( (x) > (maxval) ? (maxval) : (x) ) )

static FORCEINLINE CONSTCALL bool          math_realeq( real, real, int ulps );
static FORCEINLINE CONSTCALL bool          math_realeqns( real, real, int ulps );
static FORCEINLINE CONSTCALL bool          math_realzero( real val );
static FORCEINLINE CONSTCALL bool          math_realone( real val );
static FORCEINLINE CONSTCALL real          math_realdec( real val, int units );
static FORCEINLINE CONSTCALL real          math_realinc( real val, int units );

static FORCEINLINE CONSTCALL bool          math_realisnan( real val );
static FORCEINLINE CONSTCALL bool          math_realisinf( real val );
static FORCEINLINE CONSTCALL bool          math_realisuninitialized( real val );
static FORCEINLINE CONSTCALL bool          math_realisfinite( real val );
static FORCEINLINE CONSTCALL bool          math_realisdenormalized( real val );
static FORCEINLINE CONSTCALL real          math_realundenormalize( real val );


#if BUILD_ENABLE_ASSERT
#  define FOUNDATION_ASSERT_FINITE( value ) \
	/*lint -save -e717 */ do { FOUNDATION_ASSERT_MSG( math_realisfinite( (value), "Non-finite float value" ); } while(0) /*lint -restore */
#else
#  define FOUNDATION_ASSERT_FINITE( value ) /*lint -save -e717 */ do { (void)sizeof( value ); } while(0) /*lint -restore */
#endif

// inc/dec with wrap, from http://cellperformance.beyond3d.com/articles/2006/07/increment-and-decrement-wrapping-values.html

#define FOUNDATION_DECLARE_INCREMENT_AND_WRAP( suffix, type, signed_type, bit_mask ) \
static FORCEINLINE CONSTCALL type math_inc_wrap_##suffix( const type val, const type min, const type max ); \
static FORCEINLINE CONSTCALL type math_inc_wrap_##suffix( const type val, const type min, const type max ) { \
	const type increased    = val + 1; \
	const type max_diff     = max - val; \
	const type max_diff_nz  = (type)( ( (signed_type)max_diff | -(signed_type)max_diff ) >> bit_mask ); \
	const type max_diff_eqz = ~max_diff_nz; \
	const type result       = ( increased & max_diff_nz ) | ( min & max_diff_eqz ); \
	return result; }
 
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint8,  uint8_t,  int8_t,  7  )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint16, uint16_t, int16_t, 15 )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint32, uint32_t, int32_t, 31 )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint64, uint64_t, int64_t, 63ULL )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int8,   int8_t,   int8_t,  7  )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int16,  int16_t,  int16_t, 15 )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int32,  int32_t,  int32_t, 31 )
FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int64,  int64_t,  int64_t, 63ULL )
#undef FOUNDATION_DECLARE_INCREMENT_AND_WRAP
 
#define FOUNDATION_DECLARE_DECREMENT_AND_WRAP( suffix, type, signed_type, bit_mask ) \
static FORCEINLINE CONSTCALL type math_dec_wrap_##suffix( const type val, const type min, const type max ); \
static FORCEINLINE CONSTCALL type math_dec_wrap_##suffix( const type val, const type min, const type max ) { \
	const type decreased    = val - 1; \
	const type min_diff     = min - val; \
	const type min_diff_nz  = (type)( ( (signed_type)min_diff | -(signed_type)min_diff ) >> bit_mask ); \
	const type min_diff_eqz = ~min_diff_nz; \
	const type result       = ( decreased & min_diff_nz ) | ( max & min_diff_eqz ); \
	return result; }
 
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint8,  uint8_t,  int8_t,  7  )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint16, uint16_t, int16_t, 15 )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint32, uint32_t, int32_t, 31 )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint64, uint64_t, int64_t, 63ULL )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int8,  int8_t,   int8_t,  7  )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int16, int16_t,  int16_t, 15 )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int32, int32_t,  int32_t, 31 )
FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int64, int64_t,  int64_t, 63ULL )
#undef FOUNDATION_DECLARE_DECREMENT_AND_WRAP


// IMPLEMENTATIONS	

static FORCEINLINE CONSTCALL unsigned int math_align_poweroftwo( unsigned int x )
{
	FOUNDATION_ASSERT( x > 1 );

#if FOUNDATION_COMPILER_INTEL && ( FOUNDATION_ARCH_X86 || FOUNDATION_ARCH_X86_64 )
	--x;
	__asm__( "bsrl %1,%0"
		:"=r" (x)
		:"rm" (x) );
	return ( 1 << ( x + 1 ) );
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


static FORCEINLINE CONSTCALL bool math_is_poweroftwo( unsigned int x )
{
	return ( x & ( x - 1 ) ) == 0;
}


static FORCEINLINE CONSTCALL unsigned int math_align_up( unsigned int x, unsigned int alignment )
{
	unsigned int aligned = x;
	unsigned int remain = x % alignment;
	if( remain )
		aligned += alignment - remain;
	return aligned;
}


static FORCEINLINE CONSTCALL real math_smoothstep( real t ) { return ( REAL_C(3.0) - REAL_C(2.0)*t ) * ( t * t ); }
static FORCEINLINE CONSTCALL real math_smootherstep( real t ) { return ( t * t * t ) * ( t * ( t * REAL_C(6.0) - REAL_C(15.0) ) + REAL_C(10.0) ); }

static FORCEINLINE CONSTCALL real math_lerp( real t, real x, real y ) { return ( x + ( t * ( y - x ) ) ); }
static FORCEINLINE CONSTCALL real math_unlerp( real v, real x, real y ) { return ( ( v - x ) / ( y - x ) ); }
static FORCEINLINE CONSTCALL real math_linear_remap( real x, real xmin, real xmax, real ymin, real ymax ) { return math_lerp( math_unlerp( x, xmin, xmax ), ymin, ymax ); }

static FORCEINLINE CONSTCALL real math_log2( real x ) { return math_logn( x ) * REAL_C( 1.4426950408889634073599246810019 ); }

#if FOUNDATION_COMPILER_MSVC

#if FOUNDATION_PLATFORM_REALSIZE == 64

static FORCEINLINE real     math_sin( real x ) { return sin( x ); }
static FORCEINLINE real     math_cos( real x ) { return cos( x ); }
static FORCEINLINE real     math_tan( real x ) { return tan( x ); }
static FORCEINLINE real     math_asin( real x ) { return asin( x ); }
static FORCEINLINE real     math_acos( real x ) { return acos( x ); }
static FORCEINLINE real     math_atan( real x ) { return atan( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return atan2( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return sqrt( x ); }
#  if FOUNDATION_COMPILER_MSVC
static FORCEINLINE real     math_rsqrt( real x ) { return REAL_C( 1.0 ) / sqrtf( x ); }
#  else
static FORCEINLINE real     math_rsqrt( real x ) { return invsqrt( x ); }
#  endif
static FORCEINLINE real     math_abs( real x ) { return fabs( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return fmod( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return exp( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return pow( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return log( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)floor( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)ceil( x ); }
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)x; }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)ceil( x ); }

#elif FOUNDATION_ARCH_X86

_CRTIMP double  __cdecl ceil(_In_ double _X);

static FORCEINLINE real     math_sin( real x ) { return (real)sin( x ); }
static FORCEINLINE real     math_cos( real x ) { return (real)cos( x ); }
static FORCEINLINE real     math_tan( real x ) { return (real)tan( x ); }
static FORCEINLINE real     math_asin( real x ) { return (real)asin( x ); }
static FORCEINLINE real     math_acos( real x ) { return (real)acos( x ); }
static FORCEINLINE real     math_atan( real x ) { return (real)atan( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return (real)atan2( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return (real)sqrt( x ); }
static FORCEINLINE real     math_rsqrt( real x ) { return REAL_C( 1.0 ) / (real)sqrt( x ); }
static FORCEINLINE real     math_abs( real x ) { return (real)fabs( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return (real)fmod( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return (real)exp( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return (real)pow( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return (real)log( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)floor( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)ceil( x ); }
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)x; }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)ceil( x ); }

#else

static FORCEINLINE real     math_sin( real x ) { return sinf( x ); }
static FORCEINLINE real     math_cos( real x ) { return cosf( x ); }
static FORCEINLINE real     math_tan( real x ) { return tanf( x ); }
static FORCEINLINE real     math_asin( real x ) { return asinf( x ); }
static FORCEINLINE real     math_acos( real x ) { return acosf( x ); }
static FORCEINLINE real     math_atan( real x ) { return atanf( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return atan2f( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return sqrtf( x ); }
static FORCEINLINE real     math_rsqrt( real x ) { return REAL_C( 1.0 ) / sqrtf( x ); }
static FORCEINLINE real     math_abs( real x ) { return (real)fabs( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return fmodf( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return expf( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return powf( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return logf( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)floorf( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)ceilf( x ); }
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)x; }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)ceil( x ); }

#endif

#elif FOUNDATION_COMPILER_INTEL

#if FOUNDATION_PLATFORM_REALSIZE == 64

static FORCEINLINE real     math_sin( real x ) { return sin( x ); }
static FORCEINLINE real     math_cos( real x ) { return cos( x ); }
static FORCEINLINE real     math_tan( real x ) { return tan( x ); }
static FORCEINLINE real     math_asin( real x ) { return asin( x ); }
static FORCEINLINE real     math_acos( real x ) { return acos( x ); }
static FORCEINLINE real     math_atan( real x ) { return atan( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return atan2( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return sqrt( x ); }
static FORCEINLINE real     math_rsqrt( real x ) { return invsqrt( x ); }
static FORCEINLINE real     math_abs( real x ) { return fabs( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return fmod( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return exp( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return pow( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return log( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)floor( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)ceil( x ); }
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)x; }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)ceil( x ); }

#else

static FORCEINLINE real     math_sin( real x ) { return sinf( x ); }
static FORCEINLINE real     math_cos( real x ) { return cosf( x ); }
static FORCEINLINE real     math_tan( real x ) { return tanf( x ); }
static FORCEINLINE real     math_asin( real x ) { return asinf( x ); }
static FORCEINLINE real     math_acos( real x ) { return acosf( x ); }
static FORCEINLINE real     math_atan( real x ) { return atanf( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return atan2f( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return sqrtf( x ); }
static FORCEINLINE real     math_rsqrt( real x ) { return invsqrtf( x ); }
static FORCEINLINE real     math_abs( real x ) { return fabsf( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return fmodf( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return expf( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return powf( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return logf( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)floorf( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)ceilf( x ); }
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)x; }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)ceil( x ); }

#endif

#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG

static FORCEINLINE real     math_sin( real x ) { return __builtin_sin( x ); }
static FORCEINLINE real     math_cos( real x ) { return __builtin_cos( x ); }
static FORCEINLINE real     math_tan( real x ) { return __builtin_tan( x ); }
static FORCEINLINE real     math_asin( real x ) { return __builtin_asin( x ); }
static FORCEINLINE real     math_acos( real x ) { return __builtin_acos( x ); }
static FORCEINLINE real     math_atan( real x ) { return __builtin_atan( x ); }
static FORCEINLINE real     math_atan2( real x, real y ) { return __builtin_atan2( x, y ); }
static FORCEINLINE real     math_sqrt( real x ) { return __builtin_sqrt( x ); }
static FORCEINLINE real     math_rsqrt( real x ) { return REAL_C( 1.0 ) / __builtin_sqrt( x ); }
static FORCEINLINE real     math_abs( real x ) { return __builtin_fabs( x ); }
static FORCEINLINE real     math_mod( real x, real y ) { return __builtin_fmod( x, y ); }
static FORCEINLINE real     math_exp( real x ) { return __builtin_exp( x ); }
static FORCEINLINE real     math_pow( real x, real y ) { return __builtin_pow( x, y ); }
static FORCEINLINE real     math_logn( real x ) { return __builtin_log( x ); }

#if FOUNDATION_PLATFORM_REALSIZE == 64

static FORCEINLINE int      math_floor( real x ) { return (int)__builtin_floor( x ); }
static FORCEINLINE int      math_ceil( real x ) { return (int)__builtin_ceil( x ); }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)__builtin_floor( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)__builtin_ceil( x ); }
#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_WINDOWS
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5 ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)( x ); }
#else
static FORCEINLINE int      math_round( real x ) { return (int)__builtin_round( x ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)__builtin_trunc( x ); }
#endif

#else

static FORCEINLINE int      math_ceil( real x ) { return (int)__builtin_ceilf( x ); }
static FORCEINLINE int      math_floor( real x ) { return (int)__builtin_floorf( x ); }
static FORCEINLINE int64_t  math_ceil64( real x ) { return (int64_t)__builtin_ceil( x ); }
static FORCEINLINE int64_t  math_floor64( real x ) { return (int64_t)__builtin_floor( x ); }
#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_WINDOWS
static FORCEINLINE int      math_round( real x ) { return (int)( x + 0.5f ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)( x ); }
#else
static FORCEINLINE int      math_round( real x ) { return (int)__builtin_roundf( x ); }
static FORCEINLINE int      math_trunc( real x ) { return (int)__builtin_truncf( x ); }
#endif

#endif

#else
#  error Unknown compiler
#endif


#if FOUNDATION_PLATFORM_REALSIZE == 64


typedef union { int64_t ival; float64_t rval; } __real_convert;


static FORCEINLINE CONSTCALL bool math_realisnan( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( ( (const uint64_t)conv.ival & 0x7F80000000000000ULL ) >> 55ULL ) == 0xff ) & ( ( (const uint64_t)conv.ival & 0xFFFFFFFFFFFFFULL ) != 0 );
}


static FORCEINLINE CONSTCALL bool math_realisinf( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( ( (const uint64_t)conv.ival & 0x7F80000000000000ULL ) >> 55ULL ) == 0xff ) & ( ( (const uint64_t)conv.ival & 0xFFFFFFFFFFFFFULL ) == 0 );
}


static FORCEINLINE CONSTCALL bool math_realisuninitialized( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( (const uint64_t)conv.ival == 0xCDCDCDCDCDCDCDCDULL ) | ( (const uint64_t)conv.ival == 0xFEEEFEEEFEEEFEEEULL );
}


static FORCEINLINE CONSTCALL bool math_realisfinite( real val )
{
	return !( math_realisnan( val ) || math_realisinf( val ) || math_realisuninitialized( val ) );
}


static FORCEINLINE CONSTCALL bool math_realisdenormalized( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( (const uint64_t)conv.ival & 0x7F80000000000000ULL ) == 0 );
}


static FORCEINLINE CONSTCALL real math_realundenormalize( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	if( ( (const uint64_t)conv.ival & 0x7F80000000000000ULL ) == 0 )
		return 0;
	return val;
}


static FORCEINLINE CONSTCALL bool math_realeq( real a, real b, int32_t ulps )
{
	__real_convert ca;
	__real_convert cb;
	int64_t ai, bi, diff;
	ca.rval = a;
	cb.rval = b;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x8000000000000000LL - ai;

	bi = cb.ival;
	if( bi < 0 )
		bi = 0x8000000000000000LL - bi;

	diff = ( ai - bi );
	if( ( diff <= ulps ) && ( diff >= -ulps ) )
		return true;

	return false;
}


static FORCEINLINE CONSTCALL bool math_realeqns( real a, real b, int32_t ulps )
{
	__real_convert ca;
	__real_convert cb;
	int64_t diff;
	
	ca.rval = a;
	cb.rval = b;

	diff = ( ca.ival - cb.ival );
	if( ( diff <= ulps ) && ( diff >= -ulps ) )
		return true;
	return false;
}


static FORCEINLINE CONSTCALL bool math_realzero( real val )
{
#if 0
	__real_convert ca;
	int64_t ai;

	ca.rval = val;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x8000000000000000LL - ai;

	if( ( ai <= 100 ) && ( ai >= -100 ) )
		return true;

	return false;
#else

	return math_abs( val ) < REAL_EPSILON;
	
#endif
}


static FORCEINLINE CONSTCALL bool math_realone( real val )
{
#if 0 //BUILD_DEBUG
	__real_convert ca;
	int64_t ai, diff;
	ca.rval = val;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x8000000000000000LL - ai;

	diff = ( ai - 0x3ff0000000000000LL );
	if( ( diff <= 100 ) && ( diff >= -100 ) )
		return true;

	return false;
#else
	return math_abs( val - REAL_ONE ) < REAL_EPSILON;
#endif
}


static FORCEINLINE CONSTCALL real math_realdec( real val, int units )
{
	__real_convert ca; ca.rval = val;
	
	ca.ival -= ( ca.ival < 0 ? -units : units );
	
	return ca.rval;
}


static FORCEINLINE CONSTCALL real math_realinc( real val, int units )
{
	__real_convert ca; ca.rval = val;
	
	ca.ival += ( ca.ival < 0 ? -units : units );
	
	return ca.rval;
}

#else


typedef union { int32_t ival; float32_t rval; } __real_convert;


static FORCEINLINE CONSTCALL bool math_realisnan( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( ( (const uint32_t)conv.ival & 0x7F800000 ) >> 23 ) == 0xFF ) & ( ( (const uint32_t)conv.ival & 0x7FFFFF ) != 0 );
}


static FORCEINLINE CONSTCALL bool math_realisinf( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( ( (const uint32_t)conv.ival & 0x7F800000) >> 23 ) == 0xFF ) & ( ( (const uint32_t)conv.ival & 0x7FFFFF ) == 0 );
}


static FORCEINLINE CONSTCALL bool math_realisuninitialized( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	//Some common debugger uninitialized filler values
	return ( (const uint32_t)conv.ival == 0xFEEEFEEEU ) || ( (const uint32_t)conv.ival == 0xCDCDCDCDU );
}


static FORCEINLINE CONSTCALL bool math_realisfinite( real val )
{
	return !( math_realisnan( val ) || math_realisinf( val ) || math_realisuninitialized( val ) );
}


static FORCEINLINE CONSTCALL bool math_realisdenormalized( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	return ( ( (const uint32_t)conv.ival & 0x7F800000ULL ) == 0 );
}


static FORCEINLINE CONSTCALL real math_realundenormalize( real val )
{
#if !defined( __cplusplus ) && !FOUNDATION_COMPILER_MSVC
	const __real_convert conv = { .rval=val };
#else
	__real_convert conv; conv.rval = val;
#endif
	if( ( (const uint32_t)conv.ival & 0x7F800000 ) == 0 )
		return 0;
	return val;
}


static FORCEINLINE CONSTCALL bool math_realeq( real a, real b, int ulps )
{
	__real_convert ca;
	__real_convert cb;
	int32_t ai, bi, diff;
	ca.rval = a;
	cb.rval = b;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x80000000L - ai;

	bi = cb.ival;
	if( bi < 0 )
		bi = 0x80000000L - bi;

	diff = ( ai - bi );
	if( ( diff <= ulps ) && ( diff >= -ulps ) )
		return true;

	return false;
}


static FORCEINLINE CONSTCALL bool math_realeqns( real a, real b, int ulps )
{
	__real_convert ca;
	__real_convert cb;
	int32_t diff;
	ca.rval = a;
	cb.rval = b;

	diff = ( ca.ival - cb.ival );
	if( ( diff <= ulps ) && ( diff >= -ulps ) )
		return true;
	return false;
}


static FORCEINLINE CONSTCALL bool math_realzero( real val )
{
#if 0
	__real_convert ca;
	int32_t ai;
	ca.rval = val;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x80000000 - ai;

	if( ( ai <= 100 ) && ( ai >= -100 ) )
		return true;

	return false;
#else
	return math_abs( val ) < REAL_EPSILON;
#endif
}


static FORCEINLINE CONSTCALL bool math_realone( real val )
{
#if 0
	__real_convert ca;
	int32_t ai, diff;
	ca.rval = val;

	ai = ca.ival;
	if( ai < 0 )
		ai = 0x80000000 - ai;

	diff = ( ai - 0x3f800000 );
	if( ( diff <= 100 ) && ( diff >= -100 ) )
		return true;

	return false;
#else
	return math_abs( val - 1.0f ) < REAL_EPSILON;
#endif
}


static FORCEINLINE CONSTCALL real math_realdec( real val, int units )
{
	__real_convert ca; ca.rval = val;
	
	ca.ival -= ( ca.ival < 0 ? -units : units );
	
	return ca.rval;
}


static FORCEINLINE CONSTCALL real math_realinc( real val, int units )
{
	__real_convert ca; ca.rval = val;
	
	ca.ival += ( ca.ival < 0 ? -units : units );
	
	return ca.rval;
}

#endif
