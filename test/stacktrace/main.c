/* main.c  -  Foundation stacktrace test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation stacktrace tests";
	app.short_name = "test_stacktrace";
	app.config_dir = "test_stacktrace";
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


DECLARE_TEST( stacktrace, capture )
{
	#define TEST_DEPTH 64
	void* trace[TEST_DEPTH];
	unsigned int num_frames = 0;

	num_frames = stacktrace_capture( trace, TEST_DEPTH, 0 );
	EXPECT_GT( num_frames, 3 );
	
	return 0;
}


DECLARE_TEST( stacktrace, resolve )
{
	#define TEST_DEPTH 64
	void* trace[TEST_DEPTH];
	unsigned int num_frames = 0;
	char* resolved = 0;

	num_frames = stacktrace_capture( trace, TEST_DEPTH, 0 );
	EXPECT_GT( num_frames, 3 );

	resolved = stacktrace_resolve( trace, num_frames, 0 );
	EXPECT_NE( resolved, 0 );

	log_infof( "Resolved stack trace:\n%s", resolved );
	
	EXPECT_NE( string_find_string( resolved, "stacktraceresolve_fn", 0 ), STRING_NPOS );
	EXPECT_NE( string_find_string( resolved, "test_run", 0 ), STRING_NPOS );
	EXPECT_NE( string_find_string( resolved, "main_run", 0 ), STRING_NPOS );
	
	memory_deallocate( resolved );
	
	return 0;
}


void test_declare( void )
{
	ADD_TEST( stacktrace, capture );
	ADD_TEST( stacktrace, resolve );
}
