/* main.c  -  Foundation profile test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#define TEST_PROFILE_BUFFER_SIZE  65535

static const uint64_t       _test_profile_buffer_size = TEST_PROFILE_BUFFER_SIZE;
static char                 _test_profile_buffer[TEST_PROFILE_BUFFER_SIZE];
static uint64_t             _test_profile_offset = 0;

static volatile int32_t     _test_profile_output_counter = 0;


void test_profile_output( void* buffer, uint64_t size )
{
	
	atomic_incr32( &_test_profile_output_counter );
}


application_t test_profile_application( void )
{
	application_t app = {0};
	app.name = "Foundation profile tests";
	app.short_name = "test_profile";
	app.config_dir = "test_profile";
	app.flags = APPLICATION_UTILITY;
	return app;
}


memory_system_t test_profile_memory_system( void )
{
	return memory_system_malloc();
}


int test_profile_initialize( void )
{
	profile_output( test_profile_output );
	return 0;
}


void test_profile_shutdown( void )
{
}


DECLARE_TEST( profile, initialize )
{
	_test_profile_offset = 0;
	_test_profile_output_counter = 0;
	
	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( 1 );

	profile_log( "testing" );

	thread_sleep( 1000 );
	
	profile_enable( 0 );
	profile_shutdown();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( _test_profile_output_counter, 0 );
#else
	EXPECT_EQ( _test_profile_output_counter, 0 );
#endif
	
	return 0;
}


DECLARE_TEST( profile, output )
{
	_test_profile_offset = 0;
	_test_profile_output_counter = 0;

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( 1 );

	profile_log( "testing" );

	thread_sleep( 1000 );
	
	profile_enable( 0 );
	profile_shutdown();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( _test_profile_output_counter, 0 );

	//Parse output results
	
	
#else
	EXPECT_EQ( _test_profile_output_counter, 0 );
#endif

	_test_profile_offset = 0;
	_test_profile_output_counter = 0;

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( 0 );
	
	profile_log( "testing again" );

	thread_sleep( 1000 );
	
	profile_enable( 0 );
	profile_shutdown();

	EXPECT_EQ( _test_profile_output_counter, 0 );
	
	return 0;
}


void test_profile_declare( void )
{
	ADD_TEST( profile, initialize );
	ADD_TEST( profile, output );
}


test_suite_t test_profile_suite = {
	test_profile_application,
	test_profile_memory_system,
	test_profile_declare,
	test_profile_initialize,
	test_profile_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID

int test_profile_run( void )
{
	test_suite = test_profile_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_profile_suite;
}

#endif
