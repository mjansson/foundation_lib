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


#define TEST_PROFILE_BUFFER_SIZE  256000

static const uint64_t       _test_profile_buffer_size = TEST_PROFILE_BUFFER_SIZE;
static char*                _test_profile_buffer = 0;
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

	_test_profile_buffer = memory_allocate( TEST_PROFILE_BUFFER_SIZE, 0, MEMORY_PERSISTENT );
	
	return 0;
}


void test_profile_shutdown( void )
{
	memory_deallocate( _test_profile_buffer );
}


DECLARE_TEST( profile, initialize )
{
	error_t err = error();

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
	
	err = error();
	EXPECT_EQ( err, ERROR_NONE );

	return 0;
}


DECLARE_TEST( profile, output )
{
	error_t err = error();

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
	//TODO: Implement parsing output results	
#else
	EXPECT_EQ( _test_profile_output_counter, 0 );
#endif

	err = error();
	EXPECT_EQ( err, ERROR_NONE );
	
	_test_profile_offset = 0;
	_test_profile_output_counter = 0;

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( 0 );
	
	profile_log( "testing again" );

	thread_sleep( 1000 );
	
	profile_enable( 0 );
	profile_shutdown();

	EXPECT_EQ( _test_profile_output_counter, 0 );

	err = error();
	EXPECT_EQ( err, ERROR_NONE );
	
	return 0;
}


void* profile_thread( object_t thread, void* arg )
{
	int loop;

	thread_yield();
	
	for( loop = 0; loop < 1000; ++loop )
	{
		profile_log( "Thread message" );

		profile_begin_block( "Thread block" );
		{
			profile_update_block();

			profile_begin_block( "Thread subblock" );
			{
				profile_log( "Sub message" );

				profile_trylock( "Trylock" );

				profile_lock( "Trylock" );

				profile_wait( "Wait" );
				profile_signal( "Signal" );

				profile_unlock( "Trylock" );

				profile_log( "End sub" );

				thread_yield();
			}
			profile_end_block();

			thread_sleep( 1 );
		}
		profile_end_block();
	}
	
	return 0;
}


DECLARE_TEST( profile, thread )
{
	object_t thread[32];
	int ith;
	int frame;
	error_t err = error();

	_test_profile_offset = 0;
	_test_profile_output_counter = 0;

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( 1 );
	profile_output_wait( 1 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( profile_thread, "profile_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], 0 );
	}

	test_wait_for_threads_startup( thread, 32 );

	for( frame = 0; frame < 1000; ++frame )
	{
		thread_sleep( 16 );
		profile_end_frame( frame++ );
	}
	
	for( ith = 0; ith < 32; ++ith )
	{
		thread_destroy( thread[ith] );
		thread_yield();
	}
	
	test_wait_for_threads_exit( thread, 32 );

	thread_sleep( 1000 );
	
	profile_enable( 0 );
	profile_shutdown();

	err = error();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( _test_profile_output_counter, 0 );
	//TODO: Implement parsing output results	
#else
	EXPECT_EQ( _test_profile_output_counter, 0 );
#endif
	EXPECT_EQ( err, ERROR_NONE );

	return 0;
}


void test_profile_declare( void )
{
	ADD_TEST( profile, initialize );
	ADD_TEST( profile, output );
	ADD_TEST( profile, thread );
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
