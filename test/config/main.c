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


static hash_t write_section;
static hash_t write_key_0;
static hash_t write_key_1;
static hash_t write_key_2;
static hash_t write_key_3;
static hash_t write_key_4;
static hash_t write_key_5;
static hash_t write_key_6;
static hash_t write_key_7;

static string_const_t string_mapper( hash_t hash )
{
	if( hash == write_section ) return string_const( STRING_CONST( "write_test" ) );
	if( hash == write_key_0 ) return string_const( STRING_CONST( "key_0" ) );
	if( hash == write_key_1 ) return string_const( STRING_CONST( "key_1" ) );
	if( hash == write_key_2 ) return string_const( STRING_CONST( "key_2" ) );
	if( hash == write_key_3 ) return string_const( STRING_CONST( "key_3" ) );
	if( hash == write_key_4 ) return string_const( STRING_CONST( "key_4" ) );
	if( hash == write_key_5 ) return string_const( STRING_CONST( "key_5" ) );
	if( hash == write_key_6 ) return string_const( STRING_CONST( "key_6" ) );
	if( hash == write_key_7 ) return string_const( STRING_CONST( "key_7" ) );
	return string_null();
}


static application_t test_config_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation config tests" ) );
	app.short_name = string_const( STRING_CONST( "test_config" ) );
	app.config_dir = string_const( STRING_CONST( "test_config" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_config_memory_system( void )
{
	return memory_system_malloc();
}


static int test_config_initialize( void )
{
	return 0;
}


static foundation_config_t test_config_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
}


static void test_config_shutdown( void )
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
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( invalid_section, invalid_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "3735941133" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "3735941133", 10 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-268562445" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-268562445", 10 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "0" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( STRING_ARGS( string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ) ) ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( STRING_ARGS( string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ) ) ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "0" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "stringvalue" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "stringvalue" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "1234" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "1234" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "-1234.1" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-1234.1" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "true" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "false" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "stringvalue" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "stringvalue" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "1234" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "1234" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "-1234.1" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-1234.1" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "true" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( test_section, test_key, STRING_CONST( "false" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	return 0;
}


DECLARE_TEST( config, variables )
{
	string_const_t refstr;

	hash_t invalid_section = hash( STRING_CONST( "__section" ) );
	hash_t invalid_key = hash( STRING_CONST( "__key" ) );

	hash_t test_section = hash( STRING_CONST( "__test_expand" ) );
	hash_t test_key = hash( STRING_CONST( "__test_var" ) );

	hash_t expand_section = hash( STRING_CONST( "__expand_section" ) );
	hash_t expand_key = hash( STRING_CONST( "__expand_key" ) );

	EXPECT_FALSE( config_bool( invalid_section, invalid_key ) );
	EXPECT_EQ( config_int( invalid_section, invalid_key ), 0 );
	EXPECT_REALZERO( config_real( invalid_section, invalid_key ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( invalid_section, invalid_key ), HASH_EMPTY_STRING );

	config_set_string( test_section, test_key, STRING_CONST( "$(__expand_section:__expand_key)" ) );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "3735941133" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "3735941133", 10 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-268562445" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-268562445", 10 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "0" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	refstr = string_from_real_static( REAL_C(1234.5678), 4, 0, '0' ); EXPECT_EQ( config_hash( test_section, test_key ), hash( STRING_ARGS( refstr ) ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	refstr = string_from_real_static( REAL_C(-1234.5678), 4, 0, '0' ); EXPECT_EQ( config_hash( test_section, test_key ), hash( STRING_ARGS( refstr ) ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

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
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "0" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "0", 1 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "stringvalue" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "stringvalue" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "1234" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "1234" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "-1234.1" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-1234.1" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "true" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string( expand_section, expand_key, STRING_CONST( "false" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "stringvalue" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "stringvalue" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "stringvalue", 11 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "1234" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1234.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "1234" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "1234", 4 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "-1234.1" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), -1234 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(-1234.1) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "-1234.1" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), hash( "-1234.1", 7 ) );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "true" ) );
	EXPECT_TRUE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(1.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_TRUE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	config_set_string_constant( expand_section, expand_key, STRING_CONST( "false" ) );
	EXPECT_FALSE( config_bool( test_section, test_key ) );
	EXPECT_FALSE( config_bool( test_section, invalid_key ) );
	EXPECT_FALSE( config_bool( invalid_section, test_key ) );
	EXPECT_EQ( config_int( test_section, test_key ), 0 );
	EXPECT_EQ( config_int( test_section, invalid_key ), 0 );
	EXPECT_EQ( config_int( invalid_section, test_key ), 0 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( test_section, invalid_key ), REAL_C(0.0) );
	EXPECT_REALEQ( config_real( invalid_section, test_key ), REAL_C(0.0) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, invalid_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( invalid_section, test_key ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_hash( test_section, test_key ), HASH_FALSE );
	EXPECT_EQ( config_hash( test_section, invalid_key ), HASH_EMPTY_STRING );
	EXPECT_EQ( config_hash( invalid_section, test_key ), HASH_EMPTY_STRING );

	return 0;
}


DECLARE_TEST( config, numbers )
{
	hash_t test_section = hash( "__test_numbers", 14 );
	hash_t test_key = hash( "__test_var", 10 );

	config_set_string( test_section, test_key, STRING_CONST( "1M" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1024 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 1024.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2M" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "0.1000m" ) );
	EXPECT_EQ( config_int( test_section, test_key ), (uint64_t)( 0.1 * 1024.0 * 1024.0 ) );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 0.1 * 1024.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2.0M" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2.5m" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 * 1024 + 512 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 * 1024.0 + 512.0 * 1024.0 ) );

	config_set_string( test_section, test_key, STRING_CONST( "2.5.M" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C( 2.5 ) );

	config_set_string( test_section, test_key, STRING_CONST( "1k" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C( 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2K" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "0.1000k" ) );
	EXPECT_EQ( config_int( test_section, test_key ), (uint64_t)( 0.1 * 1024.0 ) );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 0.1 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2.0K" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 ) );

	config_set_string_constant( test_section, test_key, STRING_CONST( "2.500k" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 * 1024 + 512 );
	EXPECT_REALEQ( config_real( test_section, test_key ), (real)( 2.0 * 1024.0 + 512.0 ) );

	config_set_string( test_section, test_key, STRING_CONST( "2.5.K" ) );
	EXPECT_EQ( config_int( test_section, test_key ), 2 );
	EXPECT_REALEQ( config_real( test_section, test_key ), REAL_C( 2.5 ) );

	return 0;
}


DECLARE_TEST( config, environment )
{
	hash_t test_section = hash( "__test_env", 10 );
	hash_t test_key = hash( "__test_var", 10 );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:executable_name)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_executable_name() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:executable_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_executable_directory() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:executable_path)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_executable_path() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:initial_working_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_initial_working_directory() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:current_working_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_current_working_directory() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:home_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_home_directory() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:temporary_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_temporary_directory() );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:variable[HOME])" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_variable( STRING_CONST( "HOME" ) ) );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:variable[PATH])" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), environment_variable( STRING_CONST( "PATH" ) ) );

	config_set_string( test_section, test_key, STRING_CONST( "$(environment:nonexisting)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );

	config_set_string( test_section, test_key, STRING_CONST( "$(invalid_section_name:nonexisting)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( test_section, test_key ), string_const( STRING_CONST( "" ) ) );

	config_set_string( HASH_ENVIRONMENT, test_key, STRING_CONST( "$(home_directory)" ) );
	EXPECT_CONSTSTRINGEQ( config_string( HASH_ENVIRONMENT, test_key ), environment_home_directory() );

	return 0;
}


DECLARE_TEST( config, commandline )
{
	const string_const_t cmdline[] = {
		string_const( STRING_CONST( "--foo:bar=1234" ) ),
		string_const( STRING_CONST( "--not=valid" ) ),
		string_const( STRING_CONST( "---one:dash=toomany" ) ),
		string_const( STRING_CONST( "--first:set=1" ) ),
		string_const( STRING_CONST( "--first:set=" ) ),
		string_const( STRING_CONST( "--this:is=valid" ) ),
		string_const( STRING_CONST( "--a:boolean=true" ) ),
		string_const( STRING_CONST( "--a:nother=false" ) ),
		string_const( STRING_CONST( "--a:real=10.05" ) ),
		string_const( STRING_CONST( "--a:notnumber=1.0.1" ) ),
		string_const( STRING_CONST( "--a:number=101" ) ),
		string_const( STRING_CONST( "--a:quoted=\"foobar\"" ) ),
		string_const( STRING_CONST( "--this:is=notparsed" ) )
	};

    config_parse_commandline( cmdline, 12 ); // Should skip last "--this:is=notparsed"

	EXPECT_CONSTSTRINGEQ( config_string( hash( "foo", 3 ), hash( "bar", 3 ) ), string_const( STRING_CONST( "1234" ) ) );
	EXPECT_EQ( config_int( hash( "foo", 3 ), hash( "bar", 3 ) ), 1234 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "foo", 3 ), hash( "not", 3 ) ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_int( hash( "foo", 3 ), hash( "not", 3 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "not", 3 ), hash( "valid", 5 ) ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_int( hash( "not", 3 ), hash( "valid", 5 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "one", 3 ), hash( "dash", 4 ) ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_int( hash( "one", 3 ), hash( "dash", 4 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "first", 5 ), hash( "set", 3 ) ), string_const( STRING_CONST( "" ) ) );
	EXPECT_EQ( config_int( hash( "first", 5 ), hash( "set", 3 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "this", 4 ), hash( "is", 2 ) ), string_const( STRING_CONST( "valid" ) ) );
	EXPECT_EQ( config_int( hash( "this", 4 ), hash( "is", 2 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "boolean", 7 ) ), string_const( STRING_CONST( "true" ) ) );
	EXPECT_EQ( config_int( hash( "a", 1 ), hash( "boolean", 7 ) ), 1 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "nother", 6 ) ), string_const( STRING_CONST( "false" ) ) );
	EXPECT_EQ( config_int( hash( "a", 1 ), hash( "nother", 6 ) ), 0 );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "real", 4 ) ), string_const( STRING_CONST( "10.05" ) ) );
	EXPECT_REALEQ( config_real( hash( "a", 1 ), hash( "real", 4 ) ), REAL_C( 10.05 ) );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "notnumber", 9 ) ), string_const( STRING_CONST( "1.0.1" ) ) );
	EXPECT_EQ( config_int( hash( "a", 1 ), hash( "notnumber", 9 ) ), 1 );
	EXPECT_REALEQ( config_real( hash( "a", 1 ), hash( "notnumber", 9 ) ), REAL_C( 1.0 ) );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "number", 6 ) ), string_const( STRING_CONST( "101" ) ) );
	EXPECT_EQ( config_int( hash( "a", 1 ), hash( "number", 6 ) ), 101 );
	EXPECT_REALEQ( config_real( hash( "a", 1 ), hash( "number", 6 ) ), REAL_C( 101.0 ) );

	EXPECT_CONSTSTRINGEQ( config_string( hash( "a", 1 ), hash( "quoted", 6 ) ), string_const( STRING_CONST( "foobar" ) ) );
	EXPECT_EQ( config_int( hash( "a", 1 ), hash( "quoted", 6 ) ), 0 );

	return 0;
}


DECLARE_TEST( config, readwrite )
{
	stream_t* stream = fs_temporary_file();

	write_section = hash( "write_test", 10 );
	write_key_0 = hash( "key_0", 5 );
	write_key_1 = hash( "key_1", 5 );
	write_key_2 = hash( "key_2", 5 );
	write_key_3 = hash( "key_3", 5 );
	write_key_4 = hash( "key_4", 5 );
	write_key_5 = hash( "key_5", 5 );
	write_key_6 = hash( "key_6", 5 );
	write_key_7 = hash( "key_7", 5 );

	config_set_string( write_section, write_key_0, STRING_CONST( "foobar" ) );
	config_set_string_constant( write_section, write_key_1, STRING_CONST( "another string" ) );
	config_set_int( write_section, write_key_2, 1234 );
	config_set_real( write_section, write_key_3, REAL_C( 12.34 ) );
	config_set_bool( write_section, write_key_4, true );
	config_set_bool( write_section, write_key_5, false );
	config_set_string( write_section, write_key_6, STRING_CONST( "$(environment:initial_working_directory)" ) );
	config_set_string_constant( write_section, write_key_7, STRING_CONST( "98765" ) );

	config_write( stream, write_section, string_mapper );
	/*stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	do
	{
		char* line = stream_read_line( stream, '\n' );
		log_infof( HASH_TEST, "%s", line );
		string_deallocate( line );
	} while( !stream_eos( stream ) );*/

	config_set_string( write_section, write_key_0, STRING_CONST( "asdf" ) );
	config_set_string_constant( write_section, write_key_1, STRING_CONST( "qwerty" ) );
	config_set_int( write_section, write_key_2, 54321 );
	config_set_real( write_section, write_key_3, REAL_C( 32.78 ) );
	config_set_bool( write_section, write_key_4, false );
	config_set_bool( write_section, write_key_5, true );
	config_set_string( write_section, write_key_6, STRING_CONST( "notavariable" ) );
	config_set_string_constant( write_section, write_key_7, STRING_CONST( "12345" ) );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	config_parse( stream, write_section, false );

	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_0 ), string_const( STRING_CONST( "asdf" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_1 ), string_const( STRING_CONST( "qwerty" ) ) );
	EXPECT_EQ( config_int( write_section, write_key_2 ), 54321 );
	EXPECT_REALEQ( config_real( write_section, write_key_3 ), REAL_C( 32.78 ) );
	EXPECT_EQ( config_bool( write_section, write_key_4 ), false );
	EXPECT_EQ( config_bool( write_section, write_key_5 ), true );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_6 ), string_const( STRING_CONST( "notavariable" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_7 ), string_const( STRING_CONST( "12345" ) ) );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	config_parse( stream, write_section, true );

	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_0 ), string_const( STRING_CONST( "foobar" ) ) );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_1 ), string_const( STRING_CONST( "another string" ) ) );
	EXPECT_EQ( config_int( write_section, write_key_2 ), 1234 );
	EXPECT_REALEQ( config_real( write_section, write_key_3 ), REAL_C( 12.34 ) );
	EXPECT_EQ( config_bool( write_section, write_key_4 ), true );
	EXPECT_EQ( config_bool( write_section, write_key_5 ), false );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_6 ), environment_initial_working_directory() );
	EXPECT_CONSTSTRINGEQ( config_string( write_section, write_key_7 ), string_const( STRING_CONST( "98765" ) ) );

	stream_deallocate( stream );

	return 0;
}


static void test_config_declare( void )
{
	ADD_TEST( config, builtin );
	ADD_TEST( config, getset );
	ADD_TEST( config, variables );
	ADD_TEST( config, numbers );
	ADD_TEST( config, environment );
	ADD_TEST( config, commandline );
	ADD_TEST( config, readwrite );
}


static test_suite_t test_config_suite = {
	test_config_application,
	test_config_memory_system,
	test_config_config,
	test_config_declare,
	test_config_initialize,
	test_config_shutdown
};


#if BUILD_MONOLITHIC

int test_config_run( void );
int test_config_run( void )
{
	test_suite = test_config_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_config_suite;
}

#endif
