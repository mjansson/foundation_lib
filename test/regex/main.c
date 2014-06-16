/* main.c  -  Foundation regex test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_regex_application( void )
{
	application_t app = {0};
	app.name = "Foundation regex tests";
	app.short_name = "test_regex";
	app.config_dir = "test_regex";
	app.flags = APPLICATION_UTILITY;
	return app;
}


static memory_system_t test_regex_memory_system( void )
{
	return memory_system_malloc();
}


static int test_regex_initialize( void )
{
	return 0;
}


static void test_regex_shutdown( void )
{
}


DECLARE_TEST( regex, simple )
{
	regex_t* regex = regex_compile( "^(TEST REGEX)$" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST_REGEX", 0, 0, 0 ) );
	
	regex_free( regex );

	regex = regex_compile( "(TEST REGEX)" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST_REGEX", 0, 0, 0 ) );
	
	regex_free( regex );
	
	return 0;
}


static void test_regex_declare( void )
{
	ADD_TEST( regex, simple );
}


test_suite_t test_regex_suite = {
	test_regex_application,
	test_regex_memory_system,
	test_regex_declare,
	test_regex_initialize,
	test_regex_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS

int test_regex_run( void );
int test_regex_run( void )
{
	test_suite = test_regex_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_regex_suite;
}

#endif
