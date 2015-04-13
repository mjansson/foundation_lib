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


//Must be >30000 since we assume that in forced fail test
#define TEST_PROFILE_BUFFER_SIZE  512000

static const uint64_t       _test_profile_buffer_size = TEST_PROFILE_BUFFER_SIZE;
static char*                _test_profile_buffer;
static uint64_t             _test_profile_offset;
static atomic32_t           _test_profile_output_counter;


static void test_profile_output( void* buffer, uint64_t size )
{
	FOUNDATION_UNUSED( buffer );
	FOUNDATION_UNUSED( size );
	atomic_incr32( &_test_profile_output_counter );
}


static application_t test_profile_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation profile tests";
	app.short_name = "test_profile";
	app.config_dir = "test_profile";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_profile_memory_system( void )
{
	return memory_system_malloc();
}


static int test_profile_initialize( void )
{
	profile_set_output( test_profile_output );

	_test_profile_buffer = memory_allocate( 0, TEST_PROFILE_BUFFER_SIZE, 0, MEMORY_PERSISTENT );

	FOUNDATION_UNUSED( _test_profile_buffer_size );

	return 0;
}


static void test_profile_shutdown( void )
{
	profile_set_output( 0 );
	memory_deallocate( _test_profile_buffer );
}


DECLARE_TEST( profile, initialize )
{
	error_t err = error();

	_test_profile_offset = 0;
	atomic_store32( &_test_profile_output_counter, 0 );

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( true );

	profile_log( "testing" );

	thread_sleep( 1000 );

	profile_enable( false );
	profile_shutdown();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( atomic_load32( &_test_profile_output_counter ), 0 );
#else
	EXPECT_EQ( atomic_load32( &_test_profile_output_counter ), 0 );
#endif

	err = error();
	EXPECT_EQ( err, ERROR_NONE );

	return 0;
}


DECLARE_TEST( profile, output )
{
	error_t err = error();

	_test_profile_offset = 0;
	atomic_store32( &_test_profile_output_counter, 0 );

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( true );

	profile_log( "testing" );

	thread_sleep( 1000 );

	profile_enable( false );
	profile_shutdown();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( atomic_load32( &_test_profile_output_counter ), 0 );
	//TODO: Implement parsing output results
#else
	EXPECT_EQ( atomic_load32( &_test_profile_output_counter ), 0 );
#endif

	err = error();
	EXPECT_EQ( err, ERROR_NONE );

	_test_profile_offset = 0;
	atomic_store32( &_test_profile_output_counter, 0 );

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_enable( false );

	profile_log( "testing again" );

	thread_sleep( 1000 );

	profile_enable( false );
	profile_shutdown();

	EXPECT_EQ( atomic_load32( &_test_profile_output_counter ), 0 );

	err = error();
	EXPECT_EQ( err, ERROR_NONE );

	return 0;
}


static void* _profile_fail_thread( object_t thread, void* arg )
{
	FOUNDATION_UNUSED( arg );
	thread_sleep( 10 );

	while( !thread_should_terminate( thread ) )
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
	atomic_store32( &_test_profile_output_counter, 0 );

	profile_initialize( "test_profile", _test_profile_buffer, 30000/*_test_profile_buffer_size*/ );
	profile_enable( true );
	profile_set_output_wait( 1 );

	log_info( HASH_TEST, "This test will intentionally run out of memory in profiling system" );
	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( _profile_fail_thread, "profile_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], 0 );
	}

	test_wait_for_threads_startup( thread, 32 );

	for( frame = 0; frame < 1000; ++frame )
	{
		thread_sleep( 16 );
		profile_end_frame( frame );
	}

	for( ith = 0; ith < 32; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
		thread_yield();
	}

	test_wait_for_threads_exit( thread, 32 );

	thread_sleep( 1000 );

	profile_enable( false );
	profile_shutdown();

	err = error();

#if BUILD_ENABLE_PROFILE
	EXPECT_GT( atomic_load32( &_test_profile_output_counter ), 0 );
	//TODO: Implement parsing output results
#else
	EXPECT_EQ( atomic_load32( &_test_profile_output_counter ), 0 );
#endif
	EXPECT_EQ( err, ERROR_NONE );

	return 0;
}


static stream_t* _profile_stream;
static atomic64_t _profile_generated_blocks;


static void _profile_file_writer( void* buffer, uint64_t size )
{
	if( _profile_stream )
		stream_write( _profile_stream, buffer, size );
}


static void* _profile_stream_thread( object_t thread, void* arg )
{
	FOUNDATION_UNUSED( arg );
	thread_yield();

	while( !thread_should_terminate( thread ) )
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

				thread_sleep( 2 );

				profile_unlock( "Trylock" );

				profile_log( "End sub" );
			}
			profile_end_block();

			profile_trylock( "Trylock" );
			thread_sleep( 1 );

			profile_lock( "Trylock" );
			thread_sleep( 4 );

			profile_unlock( "Trylock" );
		}
		profile_end_block();

		thread_sleep( 4 );

		atomic_add64( &_profile_generated_blocks, 12 );
	}

	return 0;
}


DECLARE_TEST( profile, stream )
{
	object_t thread[32];
	int ith;
	int frame;
	char* filename;

	error();

	filename = path_merge( environment_temporary_directory(), "test.profile" );
	log_infof( HASH_TEST, "Output to profile file: %s", filename );
	fs_make_directory( environment_temporary_directory() );
	_profile_stream = fs_open_file( filename, STREAM_OUT | STREAM_BINARY );
	string_deallocate( filename );

	profile_initialize( "test_profile", _test_profile_buffer, _test_profile_buffer_size );
	profile_set_output( _profile_file_writer );
	profile_set_output_wait( 10 );
	profile_enable( true );

	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( _profile_stream_thread, "profile_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], 0 );
	}

	test_wait_for_threads_startup( thread, 32 );

	for( frame = 0; frame < 1000; ++frame )
	{
		thread_sleep( 16 );
		profile_log( "This is a really long profile log line that should break into multiple profile blocks automatically without causing any issues whatsoever if everything works as expected which it should or the code needs to be fixed" );
		profile_end_frame( frame++ );
		if( ( frame % 30 ) == 0 )
		{
			profile_enable( false );
			thread_sleep( 10 );
			profile_enable( true );
		}
	}

	for( ith = 0; ith < 32; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
		thread_yield();
	}

	test_wait_for_threads_exit( thread, 32 );

	profile_end_frame( frame++ );
	profile_set_output_wait( 100 );

	thread_sleep( 1000 );

	profile_enable( false );
	profile_shutdown();

	error();

	stream_deallocate( _profile_stream );

	//TODO: Validate that output is sane
	log_debugf( HASH_TEST, "Generated %lld blocks", atomic_load64( &_profile_generated_blocks ) );

	return 0;
}


static void test_profile_declare( void )
{
	ADD_TEST( profile, initialize );
	ADD_TEST( profile, output );
	ADD_TEST( profile, thread );
	ADD_TEST( profile, stream );
}


test_suite_t test_profile_suite = {
	test_profile_application,
	test_profile_memory_system,
	test_profile_declare,
	test_profile_initialize,
	test_profile_shutdown
};


#if BUILD_MONOLITHIC

int test_profile_run( void );
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
