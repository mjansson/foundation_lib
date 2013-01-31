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


application_t test_application( void )
{
	application_t app;
	app.name = "Foundation random tests";
	app.short_name = "test_random";
	app.config_dir = "test_random";
	return app;
}


static unsigned int _test_hist[64];
static unsigned int _test_bits[64];
static unsigned int _test_slice32 = ( 1U << 31U ) / 32U;
static uint64_t     _test_slice64 = ( 1ULL << 63ULL ) / 64ULL;

TEST( random, distribution32 )
{
#if FOUNDATION_PLATFORM_MOBILE
	int num_passes = 64000;
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

	for( int j = 0; j < 32; ++j )
	{
		if( _test_bits[j] < min_num )
			min_num = _test_bits[j];
		if( _test_bits[j] > max_num )
			max_num = _test_bits[j];
	}
	diff = (real)( max_num - min_num ) / ( (real)min_num + ( (real)( max_num - min_num ) / 2.0 ) );

	for( j = 0; j < 32; ++j )
		EXPECT_GT( _test_bits[j], 0U );
	EXPECT_LT( diff, 0.01 );// << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	//log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 32; ++j )
	{
		if( hist[j] < minNum )
			minNum = hist[j];
		if( hist[j] > maxNum )
			maxNum = hist[j];
	}
	diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 32; ++j )
		EXPECT_GT( hist[j], 0U );
	EXPECT_LT( diff, 0.06 ) << "Histograms: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );
	
	//Verify range distribution
	for( int j = 0; j < 128; ++j )
	{
		memset( bits, 0, sizeof( unsigned int ) * 32 );
		for( int i = 0; i < numPasses; ++i )
		{
			unsigned int num = random32_range( ( j + 1 ) * 32, j * 32 );
			EXPECT_GE( num, j * 32U );
			EXPECT_LT( num, ( j + 1 ) * 32U );
			++bits[ num % 32 ];
		}
	
		//Verify distribution...
		maxNum = 0, minNum = 0xFFFFFFFF;
		for( int j = 0; j < 32; ++j )
		{
			if( bits[j] < minNum )
				minNum = bits[j];
			if( bits[j] > maxNum )
				maxNum = bits[j];
		}
		real diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

		for( int j = 0; j < 32; ++j )
			EXPECT_GT( bits[j], 0U );
		EXPECT_LT( diff, 0.06 ) << "Range distribution: min " << minNum << " : max " << maxNum << " : diff " << diff;
	}

	END_TEST_BLOCK()
}


TEST( RandomTest, Distribution64 )
{
	BEGIN_TEST_BLOCK()

#if NEO_PLATFORM_IOS || NEO_PLATFORM_ANDROID
	int numPasses = 64000;
#else
	int numPasses = 512000;
#endif
	memset( bits, 0, sizeof( unsigned int ) * 64 );
	memset( hist, 0, sizeof( unsigned int ) * 64 );
	for( int i = 0; i < numPasses; ++i )
	{
		uint64_t num = random64();
		for( uint64_t j = 0; j < 64; ++j )
		{
			if( num & ( 1ULL << j ) )
				++bits[j];
			if( ( num >= ( slice64 * j ) ) && ( num < ( slice64 * ( j + 1 ) ) ) )
				++hist[j];
		}
	}

	/*debug_logf( "Bit distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%2d: %d", j, bits[j] );*/
	/*debug_logf( "Value distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%u-%u: %u", ( slice * j ), ( slice * ( j + 1 ) ), hist[j] );*/

	unsigned int maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 64; ++j )
	{
		if( bits[j] < minNum )
			minNum = bits[j];
		if( bits[j] > maxNum )
			maxNum = bits[j];
	}
	real diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 64; ++j )
		EXPECT_GT( bits[j], 0U );
	EXPECT_LT( diff, 0.01 ) << "Bits: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Bits: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );

	maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 64; ++j )
	{
		if( hist[j] < minNum )
			minNum = hist[j];
		if( hist[j] > maxNum )
			maxNum = hist[j];
	}
	diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 64; ++j )
		EXPECT_GT( hist[j], 0U );
	EXPECT_LT( diff, 0.1 ) << "Histograms: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );
	
	//Verify range distribution
	for( int j = 0; j < 128; ++j )
	{
		memset( bits, 0, sizeof( unsigned int ) * 64 );
		for( int i = 0; i < numPasses; ++i )
		{
			unsigned int num = random32_range( ( j + 1 ) * 64, j * 64 );
			EXPECT_GE( num, j * 64u );
			EXPECT_LT( num, ( j + 1 ) * 64U );
			++bits[ num % 64 ];
		}
	
		//Verify distribution...
		maxNum = 0, minNum = 0xFFFFFFFF;
		for( int j = 0; j < 64; ++j )
		{
			if( bits[j] < minNum )
				minNum = bits[j];
			if( bits[j] > maxNum )
				maxNum = bits[j];
		}
		real diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

		for( int j = 0; j < 64; ++j )
			EXPECT_GT( bits[j], 0U );
		EXPECT_LT( diff, 0.1 ) << "Range distribution: min " << minNum << " : max " << maxNum << " : diff " << diff;
	}
	
	END_TEST_BLOCK()
}


TEST( RandomTest, DistributionReal )
{
	BEGIN_TEST_BLOCK()

	memset( hist, 0, sizeof( unsigned int ) * 64 );
	for( int i = 0; i < 512000; ++i )
	{
		real num = random_normalized();
		EXPECT_GE( num, 0 );
		EXPECT_LT( num, 1 );
		int idx = neo_floor( num * 64.0 );
		++hist[ neo_clamp( idx, 0, 63 ) ];
	}

	/*debug_logf( "Value distribution:" );
	for( int j = 0; j < 54; ++j )
		debug_logf( "%lf-%lf: %llu", ( (1.0/64.0) * j ), ( (1.0/64.0) * ( j + 1 ) ), hist[j] );*/

	unsigned int maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 64; ++j )
	{
		if( hist[j] < minNum )
			minNum = hist[j];
		if( hist[j] > maxNum )
			maxNum = hist[j];
	}
	real diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 64; ++j )
		EXPECT_GT( hist[j], 0U );
	EXPECT_LT( diff, 0.1 ) << "Histograms: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );
	
	END_TEST_BLOCK()
}


void* random_thread( object_t thread, void* arg )
{
#if NEO_PLATFORM_IOS || NEO_PLATFORM_ANDROID
	int numPasses = 64000;
#else
	int numPasses = 512000;
#endif
	for( int i = 0; i < numPasses; ++i )
	{
		unsigned int num = random32();
		for( int j = 0; j < 32; ++j )
		{
			if( num & ( 1 << j ) )
				atomic_incr32( (int*)bits + j );
			if( ( num >= ( slice32 * j ) ) && ( num < ( slice32 * ( j + 1 ) ) ) )
				atomic_incr32( (int*)hist + j );
		}
	}
	return 0;
}


TEST( RandomTest, Threads )
{
	//This test actually allocates memory (thread random state buffers)
	//BEGIN_TEST_BLOCK()

	memset( bits, 0, sizeof( unsigned int ) * 32 );
	memset( hist, 0, sizeof( unsigned int ) * 32 );

	//Launch 32 threads
	object_t thread[32];
	for( int i = 0; i < 32; ++i )
	{
		thread[i] = thread_create( random_thread, "random", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], 0 );
	}

	thread_sleep( 500 );

	bool running = true;
	while( running )
	{
		running = false;
		for( int i = 0; i < 32; ++i )
		{
			if( thread_is_running( thread[i] ) )
			{
				running = true;
				thread_sleep( 10 );
				break;
			}
		}
	}

	for( int i = 0; i < 32; ++i )
		thread_destroy( thread[i] );
	
	debug_logf( "All threads finished" );

	/*debug_logf( "Bit distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%2d: %d", j, bits[j] );
	debug_logf( "Value distribution:" );
	for( int j = 0; j < 32; ++j )
		debug_logf( "%u-%u: %u", ( slice * j ), ( slice * ( j + 1 ) ), hist[j] );*/

	unsigned int maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 31; ++j )
	{
		if( bits[j] < minNum )
			minNum = bits[j];
		if( bits[j] > maxNum )
			maxNum = bits[j];
	}
	real diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 32; ++j )
		EXPECT_GT( bits[j], 0U );
	EXPECT_LT( diff, 0.01 ) << "Bits: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Bits: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );

	maxNum = 0, minNum = 0xFFFFFFFF;
	for( int j = 0; j < 31; ++j )
	{
		if( hist[j] < minNum )
			minNum = hist[j];
		if( hist[j] > maxNum )
			maxNum = hist[j];
	}
	diff = (real)( maxNum - minNum ) / (real)( minNum + ( ( maxNum - minNum ) / 2 ) );

	for( int j = 0; j < 32; ++j )
		EXPECT_GT( hist[j], 0U );
	EXPECT_LT( diff, 0.1 ) << "Histograms: min " << minNum << " : max " << maxNum << " : diff " << diff;

	//debug_logf( "Histograms: min %u : max %u : diff %.5lf", minNum, maxNum, (double)diff );
	
	//END_TEST_BLOCK()
}



DECLARE_TEST( random, random )
{
	return 0;
}


void test_declare( void )
{
	ADD_TEST( random, random );
}
