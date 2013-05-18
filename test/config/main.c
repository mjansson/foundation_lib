/* main.c  -  Foundation config test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation config tests";
	app.short_name = "test_config";
	app.config_dir = "test_config";
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


DECLARE_TEST( config, builtin )
{
	//No built-ins yet
	return 0;
}


DECLARE_TEST( config, getset )
{
	hash_t invalid_section = hash( "__section", 9 );
	hash_t invalid_key = hash( "__key", 5 );

	hash_t test_section = hash( "__test_config", 13 );
	hash_t test_key = hash( "__test_key", 10 );

	EXPECT_FALSE( config_bool( invalid_section, invalid_key ) );
	EXPECT_EQ( config_int( invalid_section, invalid_key ), 0 );
	EXPECT_REALZERO( config_real( invalid_section, invalid_key ) );
	EXPECT_STREQ( config_string( invalid_section, invalid_key ), "" );
	EXPECT_EQ( config_string_hash( invalid_section, invalid_key ), HASH_EMPTY_STRING );

	config_set_bool( test_section, test_key, true );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_bool( test_section, test_key, false );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( test_section, test_key, 0xdeadf00d );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0xdeadf00d );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)0xdeadf00d );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "3735941133" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "3735941133", 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( test_section, test_key, -(int64_t)0x1001f00d );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -(int64_t)0x1001f00d );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), -(real)0x1001f00d );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-268562445" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-268562445", 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( test_section, test_key, 0 );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "0" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( test_section, test_key, REAL_C(1234.5678) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.5678) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ) );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ), 9 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( test_section, test_key, REAL_C(-1234.5678) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.5678) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ) );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ), 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( test_section, test_key, REAL_C(0.0) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "0" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "stringvalue" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "stringvalue" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "1234" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "1234" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "-1234.1" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-1234.1" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "true" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "false" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "stringvalue" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "stringvalue" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "1234" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "1234" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "-1234.1" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-1234.1" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "true" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, "false" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	return 0;
}


DECLARE_TEST( config, variables )
{
	hash_t invalid_section = hash( "__section", 9 );
	hash_t invalid_key = hash( "__key", 5 );

	hash_t test_section = hash( "__test_expand", 13 );
	hash_t test_key = hash( "__test_var", 10 );

	hash_t expand_section = hash( "__expand_section", 16 );
	hash_t expand_key = hash( "__expand_key", 12 );

	EXPECT_FALSE( config_bool( invalid_section, invalid_key ) );
	EXPECT_EQ( config_int( invalid_section, invalid_key ), 0 );
	EXPECT_REALZERO( config_real( invalid_section, invalid_key ) );
	EXPECT_STREQ( config_string( invalid_section, invalid_key ), "" );
	EXPECT_EQ( config_string_hash( invalid_section, invalid_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, "$(__expand_section:__expand_key)" );

	config_set_bool( expand_section, expand_key, true );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_bool( expand_section, expand_key, false );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( expand_section, expand_key, 0xdeadf00d );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0xdeadf00d );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)0xdeadf00d );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "3735941133" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "3735941133", 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( expand_section, expand_key, -(int64_t)0x1001f00d );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -(int64_t)0x1001f00d );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), -(real)0x1001f00d );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-268562445" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-268562445", 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_int( expand_section, expand_key, 0 );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "0" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( expand_section, expand_key, REAL_C(1234.5678) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.5678) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ) );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ), 9 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( expand_section, expand_key, REAL_C(-1234.5678) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.5678) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ) );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ), 10 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_real( expand_section, expand_key, REAL_C(0.0) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "0" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "stringvalue" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "stringvalue" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "1234" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "1234" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "-1234.1" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-1234.1" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "true" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, "false" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "stringvalue" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "stringvalue" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "1234" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "1234" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "-1234.1" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "-1234.1" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "true" );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "true" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, "false" );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_STREQ( config_string( test_section, test_key ), "false" );
	EXPECT_STREQ( config_string( test_section, invalid_key ), "" );
	EXPECT_STREQ( config_string( invalid_section, test_key ), "" );
	EXPECT_EQ( config_string_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_string_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_string_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( config, builtin );
	ADD_TEST( config, getset );
	ADD_TEST( config, variables );
}
