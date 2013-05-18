/* main.c  -  Foundation hash test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation hash tests";
	app.short_name = "test_hash";
	app.config_dir = "test_hash";
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


DECLARE_TEST( hash, known )
{
	EXPECT_EQ( hash( "engine", string_length( "engine" ) ), 0x39c8cc157cfd24f8ULL );
	EXPECT_EQ( hash( "enable_remote_debugger", string_length( "enable_remote_debugger" ) ), 0xb760826929ca10a3ULL );
	EXPECT_EQ( hash( "enable_profiler", string_length( "enable_profiler" ) ), 0xaa75bf69e488ba1cULL );
	EXPECT_EQ( hash( "cache_directory", string_length( "cache_directory" ) ), 0x3e7b4931a3841da8ULL );
	EXPECT_EQ( hash( "server_address", string_length( "server_address" ) ), 0x64fcf494cf8072f5ULL );
	EXPECT_EQ( hash( "server_port", string_length( "server_port" ) ), 0xdd32e17d082c2959ULL );
	return 0;
}


DECLARE_TEST( hash, stability )
{
	//TODO: Implement a proper test instead of this crap
	int i, j, k, len;
	hash_t lhash, rhash, rhashref;

	for( i = 0; i < 128; ++i )
	{
		uint32_t lhs[129], rhs[129];
		len = i + 1;

		for( k = 0; k < len; ++k )
			lhs[k] = random32();

		lhash = hash( lhs, len * sizeof( uint32_t ) );
		EXPECT_NE( lhash, 0U );

		for( j = 0; j < 64000; ++j )
		{
			for( k = 0; k < len; ++k )
				rhs[k] = random32();

			rhashref = hash( rhs, len * sizeof( uint32_t ) );
			rhash = hash( rhs, len * sizeof( uint32_t ) );

			EXPECT_EQ( rhashref, rhash );
			if( memcmp( lhs, rhs, len * sizeof( uint32_t ) ) )
				EXPECT_NE( lhash, rhash );
			EXPECT_NE( rhash, 0U );
		}
	}

	for( i = 4; i < 128; ++i )
	{
		char lhs[130], rhs[130];
		len = i + 1;

		lhs[0] = 'f'; lhs[1] = 'n'; lhs[2] = 'd'; lhs[3] = '_';
		rhs[0] = 'f'; rhs[1] = 'n'; rhs[2] = 'd'; rhs[3] = '_';

		for( k = 4; k < len; ++k )
			lhs[k] = random32_range( 32, 128 );
		lhs[len] = 0;

		lhash = hash( lhs, len );
		EXPECT_NE( lhash, 0U );

		for( j = 0; j < 128000; ++j )
		{
			for( k = 4; k < len; ++k )
				rhs[k] = random32_range( 32, 128 );
			rhs[len] = 0;

			rhashref = hash( rhs, len );
			rhash = hash( rhs, len );

			EXPECT_EQ( rhashref, rhash );
			if( !string_equal_substr( lhs, rhs, len ) )
				EXPECT_NE( lhash, rhash );
			EXPECT_NE( rhash, 0U );
		}
	}
	return 0;
}


void test_declare( void )
{
	ADD_TEST( hash, known );
	ADD_TEST( hash, stability );
}
