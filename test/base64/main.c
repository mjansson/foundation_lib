/* main.c  -  Foundation base64 test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation base64 tests";
	app.short_name = "test_base64";
	app.config_dir = "test_base64";
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


DECLARE_TEST( base64, encode_decode )
{
	char test_string[2048];
	char test_data[1024];
	char verify_data[1024];
	unsigned int written;
	char prev_value;
	char guard_value;

	for( written = 0; written < 1024; written += 8 )
		*(uint64_t*)( test_data + written ) = random64();

	//Test encode/decode zero/one blocks
	{
		written = base64_encode( test_data, test_string, 1, 1 );
		EXPECT_EQ( written, 1 );
		EXPECT_EQ( test_string[0], 0 );

		guard_value = test_data[0];
		written = base64_decode( test_string, test_data, 0, 1 );
		EXPECT_EQ( written, 0 );
		EXPECT_EQ( guard_value, test_data[0] );

		guard_value = test_data[0];
		written = base64_decode( test_string, test_data, 1, 1 );
		EXPECT_EQ( written, 0 );
		EXPECT_EQ( guard_value, test_data[0] );

		written = base64_encode( test_data, test_string, 1, 0 );
		EXPECT_EQ( written, 5 );
		EXPECT_EQ( test_string[2], '=' );
		EXPECT_EQ( test_string[3], '=' );
		EXPECT_EQ( test_string[4], 0 );

		prev_value = test_data[0];
		guard_value = test_data[1];
		test_data[0] = ~prev_value;
		written = base64_decode( test_string, test_data, 4, 1 );
		EXPECT_EQ( written, 1 );
		EXPECT_EQ( prev_value, test_data[0] );
		EXPECT_EQ( guard_value, test_data[1] );

		prev_value = test_data[0];
		guard_value = test_data[1];
		test_data[0] = ~prev_value;
		written = base64_decode( test_string, test_data, 0, 0 );
		EXPECT_EQ( written, 1 );
		EXPECT_EQ( prev_value, test_data[0] );
		EXPECT_EQ( guard_value, test_data[1] );
	}

	//Test encode/decode larger blocks
	{
		written = base64_encode( test_data, test_string, 32, 8 );
		EXPECT_EQ( written, 5 );
		EXPECT_EQ( test_string[4], 0 );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 32, 9 );
		EXPECT_EQ( written, 9 );
		EXPECT_EQ( test_string[8], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 32, 10 );
		EXPECT_EQ( written, 9 );
		EXPECT_EQ( test_string[8], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 31, 45 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 32, 45 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 33, 45 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 32, 64 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 31, 0 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 44, 0 );
		EXPECT_EQ( written, 31 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 32, 0 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 44, 0 );
		EXPECT_EQ( written, 32 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		prev_value = test_string[0];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 33, 0 );
		EXPECT_EQ( written, 45 );
		EXPECT_EQ( test_string[44], 0 );
		EXPECT_EQ( test_string[0], prev_value );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 44, 0 );
		EXPECT_EQ( written, 33 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 44, 33 );
		EXPECT_EQ( written, 33 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 44, 7 );
		EXPECT_EQ( written, 7 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 8, 33 );
		EXPECT_EQ( written, 6 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 0, 33 );
		EXPECT_EQ( written, 33 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 0, 0 );
		EXPECT_EQ( written, 33 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		prev_value = test_string[5];
		test_string[0] = ~test_string[0];
		written = base64_encode( test_data, test_string, 1024, 0 );
		EXPECT_EQ( written, 1369 );
		EXPECT_EQ( test_string[1368], 0 );
		EXPECT_EQ( test_string[5], prev_value );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 1369, 0 );
		EXPECT_EQ( written, 1024 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 1368, 1024 );
		EXPECT_EQ( written, 1024 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 982, 519 );
		EXPECT_EQ( written, 519 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 378, 985 );
		EXPECT_EQ( written, 283 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 0, 1024 );
		EXPECT_EQ( written, 1024 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );

		memset( verify_data, 0, 1024 );
		written = base64_decode( test_string, verify_data, 0, 0 );
		EXPECT_EQ( written, 1024 );
		EXPECT_EQ( memcmp( test_data, verify_data, written ), 0 );
	}

	return 0;
}


void test_declare( void )
{
	ADD_TEST( base64, encode_decode );
}
