/* main.c  -  Foundation app test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t _global_app;


static application_t test_app_application( void )
{
	_global_app.name = string_const( STRING_CONST( "Foundation library test suite" ) );
	_global_app.short_name = string_const( STRING_CONST( "test_all" ) );
	_global_app.config_dir = string_const( STRING_CONST( "test_all" ) );
	_global_app.version = foundation_version();
	_global_app.flags = APPLICATION_UTILITY;
	_global_app.dump_callback = test_crash_handler;
	return _global_app;
}


static memory_system_t test_app_memory_system( void )
{
	return memory_system_malloc();
}


static foundation_config_t test_app_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
}


static int test_app_initialize( void )
{
	return 0;
}


static void test_app_finalize( void )
{
}


DECLARE_TEST( app, environment )
{
	EXPECT_STREQ( environment_application()->name.str, environment_application()->name.length, _global_app.name.str, _global_app.name.length );
	EXPECT_STREQ( environment_application()->short_name.str, environment_application()->short_name.length, _global_app.short_name.str, _global_app.short_name.length );
	EXPECT_STREQ( environment_application()->config_dir.str, environment_application()->config_dir.length, _global_app.config_dir.str, _global_app.config_dir.length );
	EXPECT_TRUE( uint128_equal( environment_application()->version.version, _global_app.version.version ) );
	EXPECT_EQ( environment_application()->flags, APPLICATION_UTILITY );
	EXPECT_EQ( environment_application()->dump_callback, test_crash_handler );
	return 0;
}


static void test_app_declare( void )
{
	ADD_TEST( app, environment );
}


static test_suite_t test_app_suite = {
	test_app_application,
	test_app_memory_system,
	test_app_config,
	test_app_declare,
	test_app_initialize,
	test_app_finalize
};


#if BUILD_MONOLITHIC

int test_app_run( void );
int test_app_run( void )
{
	test_suite = test_app_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_app_suite;
}

#endif
