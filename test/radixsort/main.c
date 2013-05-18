/* main.c  -  Foundation radixsort test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation radixsort tests";
	app.short_name = "test_radixsort";
	app.config_dir = "test_radixsort";
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


DECLARE_TEST( radixsort, allocation )
{
	radixsort_t* sort_none = radixsort_allocate( RADIXSORT_INT32, 0 );
	radixsort_t* sort_small = radixsort_allocate( RADIXSORT_INT32, 128 );
	radixsort_t* sort_large = radixsort_allocate( RADIXSORT_INT32, (radixsort_index_t)( ( 1ULL << (uint64_t)( sizeof( radixsort_index_t ) * 8 ) ) - 1 ) );

	EXPECT_NE( sort_none, 0 );
	EXPECT_NE( sort_small, 0 );
	EXPECT_NE( sort_large, 0 );

	radixsort_deallocate( sort_none );
	radixsort_deallocate( sort_small );
	radixsort_deallocate( sort_large );

	return 0;
}


DECLARE_TEST( radixsort, sort_int32 )
{
	int bits = 0;
	int max_bits = 20;
	int num_bits = sizeof( radixsort_index_t ) * 8;
	int32_t* arr_int;
	uint32_t* arr_uint;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	if( num_bits > max_bits )
		num_bits = max_bits;

	sort_int = radixsort_allocate( RADIXSORT_INT32, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );
	sort_uint = radixsort_allocate( RADIXSORT_UINT32, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );

	for( bits = 1; bits <= num_bits; ++bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_int;
		const radixsort_index_t* RESTRICT sindex_uint;

		arr_int = memory_allocate( sizeof( int32_t ) * num, 0, MEMORY_PERSISTENT );
		arr_uint = memory_allocate( sizeof( uint32_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort( sort_int, arr_int, num );
		sindex_uint = radixsort( sort_uint, arr_uint, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_int[0], 0 );
			EXPECT_EQ( sindex_uint[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_int[sval], sindex_int[ival] );
				EXPECT_NE( sindex_uint[sval], sindex_uint[ival] );
			}
			EXPECT_LE( arr_int[ sindex_int[ival-1] ], arr_int[ sindex_int[ival] ] );
			EXPECT_LE( arr_uint[ sindex_uint[ival-1] ], arr_uint[ sindex_uint[ival] ] );
		}

		memory_deallocate( arr_int );
		memory_deallocate( arr_uint );
	}

	for( ; bits >= 1; --bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_int;
		const radixsort_index_t* RESTRICT sindex_uint;

		arr_int = memory_allocate( sizeof( int32_t ) * num, 0, MEMORY_PERSISTENT );
		arr_uint = memory_allocate( sizeof( uint32_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort( sort_int, arr_int, num );
		sindex_uint = radixsort( sort_uint, arr_uint, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_int[0], 0 );
			EXPECT_EQ( sindex_uint[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_int[sval], sindex_int[ival] );
				EXPECT_NE( sindex_uint[sval], sindex_uint[ival] );
			}
			EXPECT_LE( arr_int[ sindex_int[ival-1] ], arr_int[ sindex_int[ival] ] );
			EXPECT_LE( arr_uint[ sindex_uint[ival-1] ], arr_uint[ sindex_uint[ival] ] );
		}

		memory_deallocate( arr_int );
		memory_deallocate( arr_uint );
	}

	radixsort_deallocate( sort_int );
	radixsort_deallocate( sort_uint );

	return 0;
}


DECLARE_TEST( radixsort, sort_int64 )
{
	int bits = 0;
	int max_bits = 20;
	int num_bits = sizeof( radixsort_index_t ) * 8;
	int64_t* arr_int;
	uint64_t* arr_uint;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	if( num_bits > max_bits )
		num_bits = max_bits;

	sort_int = radixsort_allocate( RADIXSORT_INT64, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );
	sort_uint = radixsort_allocate( RADIXSORT_UINT64, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );

	for( bits = 1; bits <= num_bits; ++bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_int;
		const radixsort_index_t* RESTRICT sindex_uint;

		arr_int = memory_allocate( sizeof( int64_t ) * num, 0, MEMORY_PERSISTENT );
		arr_uint = memory_allocate( sizeof( uint64_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort( sort_int, arr_int, num );
		sindex_uint = radixsort( sort_uint, arr_uint, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_int[0], 0 );
			EXPECT_EQ( sindex_uint[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_int[sval], sindex_int[ival] );
				EXPECT_NE( sindex_uint[sval], sindex_uint[ival] );
			}
			EXPECT_LE( arr_int[ sindex_int[ival-1] ], arr_int[ sindex_int[ival] ] );
			EXPECT_LE( arr_uint[ sindex_uint[ival-1] ], arr_uint[ sindex_uint[ival] ] );
		}

		memory_deallocate( arr_int );
		memory_deallocate( arr_uint );
	}

	for( ; bits >= 1; --bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_int;
		const radixsort_index_t* RESTRICT sindex_uint;

		arr_int = memory_allocate( sizeof( int64_t ) * num, 0, MEMORY_PERSISTENT );
		arr_uint = memory_allocate( sizeof( uint64_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort( sort_int, arr_int, num );
		sindex_uint = radixsort( sort_uint, arr_uint, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_int[0], 0 );
			EXPECT_EQ( sindex_uint[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_int[sval], sindex_int[ival] );
				EXPECT_NE( sindex_uint[sval], sindex_uint[ival] );
			}
			EXPECT_LE( arr_int[ sindex_int[ival-1] ], arr_int[ sindex_int[ival] ] );
			EXPECT_LE( arr_uint[ sindex_uint[ival-1] ], arr_uint[ sindex_uint[ival] ] );
		}

		memory_deallocate( arr_int );
		memory_deallocate( arr_uint );
	}

	radixsort_deallocate( sort_int );
	radixsort_deallocate( sort_uint );

	return 0;
}


DECLARE_TEST( radixsort, sort_real )
{
	int bits = 0;
	int max_bits = 20;
	int num_bits = sizeof( radixsort_index_t ) * 8;
	float32_t* arr_32;
	float64_t* arr_64;
	radixsort_t* sort_32;
	radixsort_t* sort_64;
	real low_range = -(real)( 1 << 30 );
	real high_range = (real)( 1 << 30 );

	if( num_bits > max_bits )
		num_bits = max_bits;

	sort_32 = radixsort_allocate( RADIXSORT_FLOAT32, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );
	sort_64 = radixsort_allocate( RADIXSORT_FLOAT64, (radixsort_index_t)( ( 1ULL << (uint64_t)num_bits ) - 1 ) );

	for( bits = 1; bits <= num_bits; ++bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_32;
		const radixsort_index_t* RESTRICT sindex_64;

		arr_32 = memory_allocate( sizeof( float32_t ) * num, 0, MEMORY_PERSISTENT );
		arr_64 = memory_allocate( sizeof( float64_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_32[ival] = random_range( low_range, high_range );
			arr_64[ival] = random_range( low_range, high_range );
		}

		sindex_32 = radixsort( sort_32, arr_32, num );
		sindex_64 = radixsort( sort_64, arr_64, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_32[0], 0 );
			EXPECT_EQ( sindex_64[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_32[sval], sindex_32[ival] );
				EXPECT_NE( sindex_64[sval], sindex_64[ival] );
			}
			EXPECT_LE( arr_32[ sindex_32[ival-1] ], arr_32[ sindex_32[ival] ] );
			EXPECT_LE( arr_64[ sindex_64[ival-1] ], arr_64[ sindex_64[ival] ] );
		}

		memory_deallocate( arr_32 );
		memory_deallocate( arr_64 );
	}

	for( ; bits >= 1; --bits )
	{
		radixsort_index_t ival, sval;
		radixsort_index_t num = (radixsort_index_t)( ( 1ULL << (uint64_t)bits ) - 1 );
		const radixsort_index_t* RESTRICT sindex_32;
		const radixsort_index_t* RESTRICT sindex_64;

		arr_32 = memory_allocate( sizeof( float32_t ) * num, 0, MEMORY_PERSISTENT );
		arr_64 = memory_allocate( sizeof( float64_t ) * num, 0, MEMORY_PERSISTENT );

		for( ival = 0; ival < num; ++ival )
		{
			arr_32[ival] = random_range( low_range, high_range );
			arr_64[ival] = random_range( low_range, high_range );
		}

		sindex_32 = radixsort( sort_32, arr_32, num );
		sindex_64 = radixsort( sort_64, arr_64, num );

		if( num == 1 )
		{
			EXPECT_EQ( sindex_32[0], 0 );
			EXPECT_EQ( sindex_64[0], 0 );
		}
		else for( ival = 1; ival < num; ++ival )
		{
			for( sval = 0; sval < ival; ++sval )
			{
				EXPECT_NE( sindex_32[sval], sindex_32[ival] );
				EXPECT_NE( sindex_64[sval], sindex_64[ival] );
			}
			EXPECT_LE( arr_32[ sindex_32[ival-1] ], arr_32[ sindex_32[ival] ] );
			EXPECT_LE( arr_64[ sindex_64[ival-1] ], arr_64[ sindex_64[ival] ] );
		}

		memory_deallocate( arr_32 );
		memory_deallocate( arr_64 );
	}

	radixsort_deallocate( sort_32 );
	radixsort_deallocate( sort_64 );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( radixsort, allocation );
	ADD_TEST( radixsort, sort_int32 );
	ADD_TEST( radixsort, sort_int64 );
	ADD_TEST( radixsort, sort_real );
}
