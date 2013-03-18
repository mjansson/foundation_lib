/* main.c  -  Foundation math test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>
#include <test/test.h>


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation math tests";
	app.short_name = "test_math";
	app.config_dir = "test_math";
	return app;
}


DECLARE_TEST( math, core )
{
	int i;
	real testreal, refreal;
	
	EXPECT_GT( REAL_EPSILON, REAL_ZERO );
	EXPECT_LT( REAL_EPSILON, REAL_ONE );
	EXPECT_GT( REAL_MAX, REAL_MIN );
	EXPECT_GT( REAL_MIN, REAL_ZERO );

	EXPECT_REALZERO( math_sin( REAL_ZERO ) );
	EXPECT_REALZERO( math_sin( REAL_PI ) );
	EXPECT_REALZERO( math_sin( REAL_TWOPI ) );
	EXPECT_REALONE(  math_sin( REAL_HALFPI ) );

	EXPECT_REALONE( math_cos( REAL_ZERO ) );
	EXPECT_REALONE( -math_cos( REAL_PI ) );
	EXPECT_REALONE( math_cos( REAL_TWOPI ) );
	EXPECT_REALZERO( math_cos( REAL_HALFPI ) );

	EXPECT_REALONE( math_tan( REAL_HALFPI * REAL_HALF ) );
	EXPECT_REALONE( -math_tan( REAL_HALFPI * REAL_HALF * REAL_THREE ) );

	EXPECT_REALZERO( math_asin( REAL_ZERO ) );
	EXPECT_REALONE( math_asin( REAL_HALFPI ) );
	EXPECT_REALZERO( math_asin( REAL_PI ) );

	EXPECT_REALZERO( math_acos( REAL_ONE ) );
	EXPECT_REALZERO( math_acos( REAL_HALFPI ) );
	EXPECT_REALONE( -math_acos( REAL_PI ) );

	EXPECT_REALZERO( math_atan( REAL_ZERO ) );
	EXPECT_REALEQ( math_atan( REAL_ONE ), REAL_HALFPI * REAL_HALF );
	
	EXPECT_REALZERO( math_atan2( REAL_ZERO, REAL_ONE ) );
	EXPECT_REALEQ( math_atan2( REAL_ONE, -REAL_ONE ), REAL_HALFPI * REAL_HALF * REAL_THREE );

	EXPECT_REALEQ( math_sqrt( 2 ), REAL_SQRT2 );
	EXPECT_REALEQ( math_sqrt( 3 ), REAL_SQRT3 );

	EXPECT_REALEQ( math_rsqrt( 2 ), REAL_ONE / REAL_SQRT2 );
	EXPECT_REALEQ( math_rsqrt( 3 ), REAL_ONE / REAL_SQRT3 );

	EXPECT_REALONE( math_abs( REAL_ONE ) );
	EXPECT_REALONE( math_abs( -REAL_ONE ) );
	EXPECT_REALZERO( math_abs( REAL_ZERO ) );
	EXPECT_REALEQ( math_abs( REAL_MAX ), REAL_MAX );
	EXPECT_REALEQ( math_abs( -REAL_MAX ), REAL_MAX );
	EXPECT_REALEQ( math_abs( REAL_MIN ), REAL_MIN );
	EXPECT_REALEQ( math_abs( -REAL_MIN ), REAL_MIN );

	EXPECT_REALZERO( math_mod( REAL_ZERO, REAL_ONE ) );
	EXPECT_REALZERO( math_mod( REAL_ONE, REAL_ONE ) );
	EXPECT_REALZERO( math_mod( REAL_MAX, REAL_ONE ) );
	EXPECT_REALONE( math_mod( REAL_THREE, REAL_TWO ) );
	EXPECT_REALONE( -math_mod( -REAL_THREE, -REAL_TWO ) );

	EXPECT_REALONE( math_exp( REAL_ZERO ) );
	EXPECT_REALEQ( math_exp( REAL_ONE ), REAL_E );

	EXPECT_REALONE( math_pow( REAL_ONE, REAL_ONE ) );
	EXPECT_REALONE( math_pow( REAL_ONE, REAL_ZERO ) );
	EXPECT_REALONE( math_pow( REAL_THREE, REAL_ZERO ) );
	EXPECT_REALEQ( math_pow( REAL_SQRT2, REAL_TWO ), REAL_TWO );

	EXPECT_REALEQ( math_logn( REAL_TWO ), REAL_LOGN2 );
	EXPECT_REALEQ( math_logn( REAL_C( 10.0 ) ), REAL_LOGN10 );

	EXPECT_REALONE( math_log2( REAL_TWO ) );
	EXPECT_REALEQ( math_log2( REAL_TWO * REAL_TWO ), REAL_TWO );
	EXPECT_REALEQ( math_log2( REAL_TWO * REAL_TWO * REAL_TWO * REAL_TWO ), REAL_C( 4.0 ) );

	EXPECT_EQ( math_floor( REAL_ZERO ), 0 );
	EXPECT_EQ( math_floor( REAL_C( 0.999 ) ), 0 );
	EXPECT_EQ( math_floor( REAL_C( -0.1 ) ), -1 );
	EXPECT_EQ( math_floor( REAL_C( 42.5 ) ), 42 );

	EXPECT_EQ( math_ceil( REAL_ZERO ), 0 );
	EXPECT_EQ( math_ceil( REAL_C( 0.999 ) ), 1 );
	EXPECT_EQ( math_ceil( REAL_C( -0.1 ) ), 0 );
	EXPECT_EQ( math_ceil( REAL_C( 42.5 ) ), 43 );
	EXPECT_EQ( math_ceil( REAL_C( 42.45 ) ), 43 );

	EXPECT_EQ( math_floor64( REAL_ZERO ), 0 );
	EXPECT_EQ( math_floor64( REAL_C( 0.999 ) ), 0 );
	EXPECT_EQ( math_floor64( REAL_C( -0.1 ) ), -1 );
	EXPECT_EQ( math_floor64( REAL_C( 42.5 ) ), 42 );

	EXPECT_EQ( math_ceil64( REAL_ZERO ), 0 );
	EXPECT_EQ( math_ceil64( REAL_C( 0.999 ) ), 1 );
	EXPECT_EQ( math_ceil64( REAL_C( -0.1 ) ), 0 );
	EXPECT_EQ( math_ceil64( REAL_C( 42.5 ) ), 43 );
	EXPECT_EQ( math_ceil64( REAL_C( 42.45 ) ), 43 );
	
	EXPECT_EQ( math_round( REAL_ZERO ), 0 );
	EXPECT_EQ( math_round( REAL_C( 0.999 ) ), 1 );
	EXPECT_EQ( math_round( REAL_C( -0.1 ) ), 0 );
	EXPECT_EQ( math_round( REAL_C( 42.5 ) ), 43 );
	EXPECT_EQ( math_round( REAL_C( 42.45 ) ), 42 );
	
	EXPECT_EQ( math_trunc( REAL_ZERO ), 0 );
	EXPECT_EQ( math_trunc( REAL_C( 0.999 ) ), 0 );
	EXPECT_EQ( math_trunc( REAL_C( -0.1 ) ), 0 );
	EXPECT_EQ( math_trunc( REAL_C( 42.5 ) ), 42 );

	EXPECT_EQ( math_align_poweroftwo( 2 ), 2 );
	EXPECT_EQ( math_align_poweroftwo( 3 ), 4 );
	EXPECT_EQ( math_align_poweroftwo( 4 ), 4 );
	EXPECT_EQ( math_align_poweroftwo( 33 ), 64 );
	EXPECT_EQ( math_align_poweroftwo( 134217729 ), 268435456 );

	for( i = 1; i < 31; ++i )
	{
		EXPECT_TRUE( math_is_poweroftwo( math_align_poweroftwo( ( 2 << i ) - 1 ) ) );
		EXPECT_TRUE( math_is_poweroftwo( math_align_poweroftwo( ( 2 << i )     ) ) );
		EXPECT_TRUE( math_is_poweroftwo( math_align_poweroftwo( ( 2 << i ) + 1 ) ) );

		EXPECT_FALSE( math_is_poweroftwo( ( 2 << i ) - 1 ) );
		EXPECT_TRUE( math_is_poweroftwo( ( 2 << i )     ) );
		EXPECT_FALSE( math_is_poweroftwo( ( 2 << i ) + 1 ) );
	}

	EXPECT_EQ( math_align_up( 1, 1 ), 1 );
	EXPECT_EQ( math_align_up( 1, 2 ), 2 );
	EXPECT_EQ( math_align_up( 17, 2 ), 18 );
	EXPECT_EQ( math_align_up( 43, 42 ), 84 );

	EXPECT_REALZERO( math_smoothstep( REAL_ZERO ) );
	EXPECT_REALONE( math_smoothstep( REAL_ONE ) );
	EXPECT_REALEQ( math_smoothstep( REAL_HALF ), REAL_HALF );
	
	EXPECT_REALZERO( math_smootherstep( REAL_ZERO ) );
	EXPECT_REALONE( math_smootherstep( REAL_ONE ) );
	EXPECT_REALEQ( math_smootherstep( REAL_HALF ), REAL_HALF );

	EXPECT_REALZERO( math_lerp( REAL_ZERO, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALZERO( math_lerp( REAL_ONE, REAL_ONE, REAL_ZERO ) );
	EXPECT_REALONE( math_lerp( REAL_ONE, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALONE( math_lerp( REAL_ZERO, REAL_ONE, REAL_ZERO ) );
	EXPECT_REALEQ( math_lerp( REAL_HALF, REAL_ZERO, REAL_ONE ), REAL_HALF );
	EXPECT_REALEQ( math_lerp( REAL_HALF, REAL_ZERO, REAL_ONE ), REAL_HALF );

	EXPECT_REALZERO( math_unlerp( REAL_ZERO, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALZERO( math_unlerp( REAL_ONE, REAL_ONE, REAL_ZERO ) );
	EXPECT_REALONE( math_unlerp( REAL_ONE, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALONE( math_unlerp( REAL_ZERO, REAL_ONE, REAL_ZERO ) );
	EXPECT_REALEQ( math_unlerp( REAL_HALF, REAL_ZERO, REAL_ONE ), REAL_HALF );
	EXPECT_REALEQ( math_unlerp( REAL_HALF, REAL_ZERO, REAL_ONE ), REAL_HALF );

	EXPECT_REALONE( math_linear_remap( REAL_C( 150.0 ), REAL_C( 100.0 ), REAL_C( 200.0 ), REAL_ZERO, REAL_TWO ) );

	EXPECT_REALZERO( math_clamp( -REAL_ONE, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALZERO( math_clamp( REAL_ZERO, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALEQ( math_clamp( REAL_HALF, REAL_ZERO, REAL_ONE ), REAL_HALF );
	EXPECT_REALONE( math_clamp( REAL_ONE, REAL_ZERO, REAL_ONE ) );
	EXPECT_REALONE( math_clamp( REAL_TWO, REAL_ZERO, REAL_ONE ) );
	
	testreal = REAL_C( 42.42 );
	refreal = testreal;

	EXPECT_EQ( testreal, refreal );
	EXPECT_TRUE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_TRUE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	testreal = math_realdec( testreal, 10 );
	EXPECT_NE( testreal, refreal );
	EXPECT_FALSE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_TRUE( math_realeq( testreal, refreal, 10 ) );
	EXPECT_TRUE( math_realeqns( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	testreal = math_realdec( testreal, 10 );
	EXPECT_NE( testreal, refreal );
	EXPECT_FALSE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeq( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	testreal = math_realinc( testreal, 20 );
	EXPECT_EQ( testreal, refreal );
	EXPECT_TRUE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_TRUE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	testreal = math_realinc( testreal, 10 );
	EXPECT_NE( testreal, refreal );
	EXPECT_FALSE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_TRUE( math_realeq( testreal, refreal, 10 ) );
	EXPECT_TRUE( math_realeqns( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	testreal = math_realinc( testreal, 10 );
	EXPECT_NE( testreal, refreal );
	EXPECT_FALSE( math_realeq( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 0 ) );
	EXPECT_FALSE( math_realeq( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realeqns( testreal, refreal, 10 ) );
	EXPECT_FALSE( math_realzero( testreal ) );
	EXPECT_FALSE( math_realone( testreal ) );
	EXPECT_FALSE( math_realisnan( testreal ) );
	EXPECT_FALSE( math_realisinf( testreal ) );
	EXPECT_FALSE( math_realisuninitialized( testreal ) );
	EXPECT_TRUE( math_realisfinite( testreal ) );

	EXPECT_TRUE( math_realisnan( math_sqrt( REAL_C( -1.0 ) ) ) );
	EXPECT_TRUE( math_realisinf( REAL_ONE / REAL_ZERO ) );

	testreal = REAL_ONE / REAL_MAX;
	EXPECT_REALNE( testreal, REAL_ZERO );
	EXPECT_REALZERO( math_realundenormalize( testreal ) );

#if 0
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint8,  uint8_t,  int8_t,  7  )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint16, uint16_t, int16_t, 15 )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint32, uint32_t, int32_t, 31 )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( uint64, uint64_t, int64_t, 63ULL )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int8,   int8_t,   int8_t,  7  )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int16,  int16_t,  int16_t, 15 )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int32,  int32_t,  int32_t, 31 )
	FOUNDATION_DECLARE_INCREMENT_AND_WRAP( int64,  int64_t,  int64_t, 63ULL )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint8,  uint8_t,  int8_t,  7  )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint16, uint16_t, int16_t, 15 )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint32, uint32_t, int32_t, 31 )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( uint64, uint64_t, int64_t, 63ULL )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int8,  int8_t,   int8_t,  7  )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int16, int16_t,  int16_t, 15 )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int32, int32_t,  int32_t, 31 )
	FOUNDATION_DECLARE_DECREMENT_AND_WRAP( int64, int64_t,  int64_t, 63ULL )
#endif	
	
	return 0;
}


void test_declare( void )
{
	ADD_TEST( math, core );
}
