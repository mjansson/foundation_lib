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


static application_t _global_app = {0};


application_t test_app_application( void )
{
	_global_app.name = "Foundation application tests";
	_global_app.short_name = "test_app";
	_global_app.config_dir = "test_app";
	_global_app.version = version_make( 1, 2, 3, 4, 5 );
	_global_app.flags = APPLICATION_UTILITY;
	return _global_app;
}


memory_system_t test_app_memory_system( void )
{
	return memory_system_malloc();
}


int test_app_initialize( void )
{
	return 0;
}


void test_app_shutdown( void )
{
}


DECLARE_TEST( app, environment )
{
	EXPECT_STREQ( environment_application()->name, _global_app.name );
	EXPECT_STREQ( environment_application()->short_name, _global_app.short_name );
	EXPECT_STREQ( environment_application()->config_dir, _global_app.config_dir );
	EXPECT_TRUE( uint128_equal( environment_application()->version.version, _global_app.version.version ) );
	return 0;
}


void test_app_declare( void )
{
	ADD_TEST( app, environment );
}


test_suite_t test_app_suite = {
	test_app_application,
	test_app_memory_system,
	test_app_declare,
	test_app_initialize,
	test_app_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID

int test_app_run( void )
{
	test_suite = test_app_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_app_suite;
}

#endif
