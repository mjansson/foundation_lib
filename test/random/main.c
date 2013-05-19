/* main.c  -  Foundation random tests  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static unsigned int _test_hist[64];
static unsigned int _test_bits[64];
static unsigned int _test_slice32 = 0x8000000U;//( 1U << 32U ) / 32U;
static uint64_t     _test_slice64 = 0x400000000000000ULL;//( 1ULL << 64ULL ) / 64ULL;


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation random tests";
	app.short_name = "test_random";
	app.config_dir = "test_random";
	app.flags = APPLICATION_UTILITY;
	return app;
}


int test_initialize( void )
{
	return 0;
}


void test_shutdown( void )
{
}


DECLARE_TEST( random, distribution32 )
{
	int num_passes = 512000 * 16;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;
	unsigned int num;
	int i, j;
	real diff;

	//Force allocations
	random32();

	memset( _test_bits, 0, sizeof( unsigned int ) * 32 );
	memset( _test_hist, 0, sizeof( unsigned int ) * 32 );
	for( i = 0; i < num_passes; ++i )
	{
		num = random32();
		for( j = 0; j < 32; ++j )
		{
			if( num & ( 1 << j ) )
				++_test_bits[j];
			if( ( num >= ( _test_slice32 * j ) ) && ( ( j == 31 ) || ( num < ( _test_slice32 * ( j + 1 ) ) ) ) )
				++_test_hist[j];
		}
	}

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 32; ++j )
		log_debugf( "%2u: %u", j, _test_bits[j] );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 32; ++j )
		log_debugf( "%08x-%08x: %u", ( _test_slice32 * j ), ( _test_slice32 * ( j + 1 ) ) - 1, _test_hist[j] );*/

	for( j = 0; j < 32; ++j )
	{
		if( _test_bits[j] < min_num )
			min_num = _test_bits[j];
		if( _test_bits[j] > max_num )
			max_num = _test_bits[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_bits[j], 0U );
	EXPECT_LT( diff, 0.004 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0, min_num = 0xFFFFFFFF;
	for( j = 0; j < 32; ++j )
	{
		if( _test_hist[j] < min_num )
			min_num = _test_hist[j];
		if( _test_hist[j] > max_num )
			max_num = _test_hist[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_hist[j], 0U );
	EXPECT_LT( diff, 0.02 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );
	
	//Verify range distribution
	memset( _test_bits, 0, sizeof( unsigned int ) * 32 );
	for( i = 0; i < num_passes; ++i )
	{
		num = random32_range( ( j + 1 ) * 32, j * 32 );
		EXPECT_GE( num, j * 32U );
		EXPECT_LT( num, ( j + 1 ) * 32U );
		++_test_bits[ num % 32 ];
	}
	
	//Verify distribution...
	max_num = 0, min_num = 0xFFFFFFFF;
	for( i = 0; i < 32; ++i )
	{
		if( _test_bits[i] < min_num )
			min_num = _test_bits[i];
		if( _test_bits[i] > max_num )
			max_num = _test_bits[i];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( i = 0; i < 32; ++i )
		EXPECT_GT( _test_bits[i], 0U );
	EXPECT_LT( diff, 0.02 );// << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Range distribution: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


DECLARE_TEST( random, distribution64 )
{
	int num_passes = 512000 * 16;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;
	int i;
	uint64_t j;
	real diff;
	
	memset( _test_bits, 0, sizeof( unsigned int ) * 64 );
	memset( _test_hist, 0, sizeof( unsigned int ) * 64 );
	for( i = 0; i < num_passes; ++i )
	{
		uint64_t num = random64();
		for( j = 0; j < 64; ++j )
		{
			if( num & ( 1ULL << j ) )
				++_test_bits[j];
			if( ( num >= ( _test_slice64 * j ) ) && ( ( j == 63 ) || ( num < ( _test_slice64 * ( j + 1 ) ) ) ) )
				++_test_hist[j];
		}
	}

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 64; ++j )
		log_debugf( "%2u: %u", (unsigned int)j, _test_bits[j] );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 64; ++j )
		log_debugf( "%016llx-%016llx: %u", ( _test_slice64 * j ), ( _test_slice64 * ( j + 1 ) ) - 1, _test_hist[j] );*/

	for( j = 0; j < 64; ++j )
	{
		if( _test_bits[j] < min_num )
			min_num = _test_bits[j];
		if( _test_bits[j] > max_num )
			max_num = _test_bits[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 64; ++j )
		EXPECT_GT( _test_bits[j], 0U );
	EXPECT_LT( diff, 0.004 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0, min_num = 0xFFFFFFFF;
	for( j = 0; j < 64; ++j )
	{
		if( _test_hist[j] < min_num )
			min_num = _test_hist[j];
		if( _test_hist[j] > max_num )
			max_num = _test_hist[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 64; ++j )
		EXPECT_GT( _test_hist[j], 0U );
	EXPECT_LT( diff, 0.02 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );
	
	//Verify range distribution
	memset( _test_bits, 0, sizeof( unsigned int ) * 64 );
	for( i = 0; i < num_passes; ++i )
	{
		uint64_t num = random64_range( ( j + 1 ) * 64, j * 64 );
		EXPECT_GE( num, j * 64U );
		EXPECT_LT( num, ( j + 1 ) * 64U );
		++_test_bits[ num % 64 ];
	}
	
	//Verify distribution...
	max_num = 0, min_num = 0xFFFFFFFF;
	for( i = 0; i < 64; ++i )
	{
		if( _test_bits[i] < min_num )
			min_num = _test_bits[i];
		if( _test_bits[i] > max_num )
			max_num = _test_bits[i];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( i = 0; i < 64; ++i )
		EXPECT_GT( _test_bits[i], 0U );
	EXPECT_LT( diff, 0.02 );// << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Range distribution: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


DECLARE_TEST( random, distribution_real )
{
	int num_passes = 512000 * 16;
	int i, idx;
	real diff, num;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;

	memset( _test_hist, 0, sizeof( unsigned int ) * 64 );
	for( i = 0; i < num_passes; ++i )
	{
		num = random_normalized();
		EXPECT_GE( num, 0 );
		EXPECT_LT( num, 1 );
		idx = math_floor( num * REAL_C(64.0) );
		++_test_hist[ ( idx < 0 ? 0 : ( idx > 63 ? 63 : idx ) ) ];
	}

	/*log_debugf( "Value distribution:" );
	for( i = 0; i < 64; ++i )
		log_debugf( "%lf-%lf: %u", ( (1.0/64.0) * (double)i ), ( (1.0/64.0) * (double)( i + 1 ) ) - 1, _test_hist[i] );*/

	for( i = 0; i < 64; ++i )
	{
		if( _test_hist[i] < min_num )
			min_num = _test_hist[i];
		if( _test_hist[i] > max_num )
			max_num = _test_hist[i];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( i = 0; i < 64; ++i )
		EXPECT_GT( _test_hist[i], 0U );
	EXPECT_LT( diff, 0.02 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


void* random_thread( object_t thread, void* arg )
{
	int num_passes = 512000 * 8;
	int i, j;
	unsigned int num;

	for( i = 0; i < num_passes; ++i )
	{
		num = random32();
		for( j = 0; j < 32; ++j )
		{
			if( num & ( 1 << j ) )
				atomic_incr32( (int*)_test_bits + j );
			if( ( num >= ( _test_slice32 * j ) ) && ( ( j == 31 ) || ( num < ( _test_slice32 * ( j + 1 ) ) ) ) )
				atomic_incr32( (int*)_test_hist + j );
		}
	}
	return 0;
}


DECLARE_TEST( random, threads )
{
	//Launch 32 threads
	object_t thread[32];
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;
	int i, j;
	real diff;
	memset( _test_bits, 0, sizeof( unsigned int ) * 32 );
	memset( _test_hist, 0, sizeof( unsigned int ) * 32 );

	for( i = 0; i < 32; ++i )
	{
		thread[i] = thread_create( random_thread, "random", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], 0 );
	}

	test_wait_for_threads_startup( thread, 32 );

	for( i = 0; i < 32; ++i )
		thread_destroy( thread[i] );
	
	test_wait_for_threads_exit( thread, 32 );

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 32; ++j )
		log_debugf( "%2d: %d", j, _test_bits[j] );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 32; ++j )
		log_debugf( "%08x-%08x: %u", ( _test_slice32 * j ), ( _test_slice32 * ( j + 1 ) ) - 1, _test_hist[j] );*/

	for( j = 0; j < 32; ++j )
	{
		if( _test_bits[j] < min_num )
			min_num = _test_bits[j];
		if( _test_bits[j] > max_num )
			max_num = _test_bits[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_bits[j], 0U );
	EXPECT_LT( diff, 0.004 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0, min_num = 0xFFFFFFFF;
	for( j = 0; j < 31; ++j )
	{
		if( _test_hist[j] < min_num )
			min_num = _test_hist[j];
		if( _test_hist[j] > max_num )
			max_num = _test_hist[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_hist[j], 0U );
	EXPECT_LT( diff, 0.02 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( random, distribution32 );
	ADD_TEST( random, distribution64 );
	ADD_TEST( random, distribution_real );
	ADD_TEST( random, threads );
}
