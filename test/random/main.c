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

#include <foundation.h>
#include <test/test.h>


static unsigned int _test_hist[64];
static unsigned int _test_bits[64];
static unsigned int _test_slice32 = ( 1U << 31U ) / 32U;
static uint64_t     _test_slice64 = ( 1ULL << 63ULL ) / 64ULL;


application_t test_application( void )
{
	application_t app;
	app.name = "Foundation random tests";
	app.short_name = "test_random";
	app.config_dir = "test_random";
	return app;
}


DECLARE_TEST( random, distribution32 )
{
#if FOUNDATION_PLATFORM_MOBILE
	int num_passes = 64000;
#elif FOUNDATION_BUILD_DEBUG
	int num_passes = 256000;
#else
	int num_passes = 512000;
#endif
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
			if( ( num >= ( _test_slice32 * j ) ) && ( num < ( _test_slice32 * ( j + 1 ) ) ) )
				++_test_hist[j];
		}
	}

	/*log_debugf( "Bit distribution:" );
	for( int j = 0; j < 32; ++j )
		log_debugf( "%2d: %d", j, _test_bits[j] );*/
	/*log_debugf( "Value distribution:" );
	for( int j = 0; j < 32; ++j )
		log_debugf( "%u-%u: %u", ( _test_slice32 * j ), ( _test_slice32 * ( j + 1 ) ), _test_hist[j] );*/

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
	EXPECT_LT( diff, 0.01 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

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
	EXPECT_LT( diff, 0.06 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );
	
	//Verify range distribution
	for( j = 0; j < 128; ++j )
	{
		memset( _test_bits, 0, sizeof( unsigned int ) * 32 );
		for( i = 0; i < num_passes; ++i )
		{
			unsigned int num = random32_range( ( j + 1 ) * 32, j * 32 );
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
		EXPECT_LT( diff, 0.06 );// << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;

		//log_debugf( "Range distribution: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );
	}

	return 0;
}


DECLARE_TEST( random, distribution64 )
{
#if FOUNDATION_PLATFORM_MOBILE
	int num_passes = 64000;
#elif FOUNDATION_BUILD_DEBUG
	int num_passes = 256000;
#else
	int num_passes = 512000;
#endif
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
			if( ( num >= ( _test_slice64 * j ) ) && ( num < ( _test_slice64 * ( j + 1 ) ) ) )
				++_test_hist[j];
		}
	}

	/*debug_logf( "Bit distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%2d: %d", j, _test_bits[j] );*/
	/*debug_logf( "Value distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%u-%u: %u", ( _test_slice64 * j ), ( _test_slice64 * ( j + 1 ) ), _test_hist[j] );*/

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
	EXPECT_LT( diff, 0.01 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//debug_logf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

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
	EXPECT_LT( diff, 0.1 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );
	
	//Verify range distribution
	for( j = 0; j < 128; ++j )
	{
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
		EXPECT_LT( diff, 0.1 );// << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;
	}

	return 0;
}


DECLARE_TEST( random, distribution_real )
{
	int i, idx;
	real diff, num;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;

	memset( _test_hist, 0, sizeof( unsigned int ) * 64 );
	for( i = 0; i < 512000; ++i )
	{
		num = random_normalized();
		EXPECT_GE( num, 0 );
		EXPECT_LT( num, 1 );
		idx = math_floor( num * REAL_C(64.0) );
		++_test_hist[ ( idx < 0 ? 0 : ( idx > 63 ? 63 : idx ) ) ];
	}

	/*debug_logf( "Value distribution:" );
	for( int j = 0; j < 54; ++j )
		debug_logf( "%lf-%lf: %llu", ( (1.0/64.0) * j ), ( (1.0/64.0) * ( j + 1 ) ), _test_hist[j] );*/

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
	EXPECT_LT( diff, 0.1 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


void* random_thread( object_t thread, void* arg )
{
#if FOUNDATION_PLATFORM_MOBILE
	int num_passes = 64000;
#else
	int num_passes = 512000;
#endif
	int i, j;
	unsigned int num;

	for( i = 0; i < num_passes; ++i )
	{
		num = random32();
		for( j = 0; j < 32; ++j )
		{
			if( num & ( 1 << j ) )
				atomic_incr32( (int*)_test_bits + j );
			if( ( num >= ( _test_slice32 * j ) ) && ( num < ( _test_slice32 * ( j + 1 ) ) ) )
				atomic_incr32( (int*)_test_hist + j );
		}
	}
	return 0;
}


DECLARE_TEST( random, threads )
{
	//Launch 32 threads
	object_t thread[32];
	bool running = true;
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

	thread_sleep( 500 );

	while( running )
	{
		running = false;
		for( i = 0; i < 32; ++i )
		{
			if( thread_is_running( thread[i] ) )
			{
				running = true;
				thread_sleep( 10 );
				break;
			}
		}
	}

	for( i = 0; i < 32; ++i )
		thread_destroy( thread[i] );
	
	/*debug_logf( "Bit distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%2d: %d", j, _test_bits[j] );
	debug_logf( "Value distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%u-%u: %u", ( _test_slice32 * j ), ( _test_slice32 * ( j + 1 ) ), _test_hist[j] );*/

	for( j = 0; j < 31; ++j )
	{
		if( _test_bits[j] < min_num )
			min_num = _test_bits[j];
		if( _test_bits[j] > max_num )
			max_num = _test_bits[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / REAL_C(2.0) ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_bits[j], 0U );
	EXPECT_LT( diff, 0.01 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//debug_logf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

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
	EXPECT_LT( diff, 0.1 );// << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( random, distribution32 );
	ADD_TEST( random, distribution64 );
	ADD_TEST( random, distribution_real );
	ADD_TEST( random, threads );
}
