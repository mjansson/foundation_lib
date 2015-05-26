/* main.c  -  Foundation atomic test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_atomic_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation atomic tests" ) );
	app.short_name = string_const( STRING_CONST( "test_atomic" ) );
	app.config_dir = string_const( STRING_CONST( "test_atomic" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_atomic_memory_system( void )
{
	return memory_system_malloc();
}


static foundation_config_t test_atomic_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
}


static int test_atomic_initialize( void )
{
	return 0;
}


static void test_atomic_shutdown( void )
{
}


static atomic32_t  val_32;
static atomic64_t  val_64;
static atomicptr_t val_ptr;


static void* inc_thread( object_t thread, void* arg )
{
	int loop = 0;
	int icount = 0;
	FOUNDATION_UNUSED( arg );
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 256; ++icount )
		{
			atomic_incr32( &val_32 );
			atomic_incr64( &val_64 );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


static void* dec_thread( object_t thread, void* arg )
{
	int loop = 0;
	int icount = 0;
	FOUNDATION_UNUSED( arg );
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 256; ++icount )
		{
			atomic_decr32( &val_32 );
			atomic_decr64( &val_64 );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


static void* add_thread( object_t thread, void* arg )
{
	int loop = 0;
	int32_t icount = 0;
	FOUNDATION_UNUSED( arg );
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 128; ++icount )
		{
			atomic_add32( &val_32, icount % 2 ? -icount : icount );
			atomic_exchange_and_add64( &val_64, icount % 2 ? -icount : icount );
		}
		for( icount = 0; icount < 128; ++icount )
		{
			atomic_exchange_and_add32( &val_32, icount % 2 ? icount : -icount );
			atomic_add64( &val_64, icount % 2 ? icount : -icount );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


typedef struct
{
	int32_t val_32;
	int64_t val_64;
	void*   val_ptr;
} cas_value_t;

static void* cas_thread( object_t thread, void* arg )
{
	unsigned int loop = 0;
	cas_value_t val = *(cas_value_t*)arg;

	thread_sleep( 10 );

	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		while( !atomic_cas32( &val_32, val.val_32, 0 ) )
			thread_yield();
		while( !atomic_cas32( &val_32, 0, val.val_32 ) )
			thread_yield();
		while( !atomic_cas64( &val_64, val.val_64, 0 ) )
			thread_yield();
		while( !atomic_cas64( &val_64, 0, val.val_64 ) )
			thread_yield();
		while( !atomic_cas_ptr( &val_ptr, val.val_ptr, 0 ) )
			thread_yield();
		while( !atomic_cas_ptr( &val_ptr, 0, val.val_ptr ) )
			thread_yield();

		++loop;
		thread_yield();
	}
	return 0;
}


DECLARE_TEST( atomic, incdec )
{
	size_t num_threads = math_clamp( system_hardware_threads() * 4, 4, 32 );
	size_t ithread;
	object_t threads[32];

	for( ithread = 0; ithread < num_threads; ++ithread )
		threads[ithread] = thread_create( ithread % 2 ? dec_thread : inc_thread, ithread % 2 ? "dec" : "inc", 3, THREAD_PRIORITY_NORMAL, 0 );
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], 0 );

	test_wait_for_threads_startup( threads, num_threads );

	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] ); //No terminate, wait to self-exit

	test_wait_for_threads_exit( threads, num_threads );

	EXPECT_EQ( atomic_load32( &val_32 ), 0 );
	EXPECT_EQ( atomic_load64( &val_64 ), 0 );

	return 0;
}


DECLARE_TEST( atomic, add )
{
	size_t num_threads = math_clamp( system_hardware_threads() * 4, 4, 32 );
	size_t ithread;
	object_t threads[32];

	for( ithread = 0; ithread < num_threads; ++ithread )
		threads[ithread] = thread_create( add_thread, STRING_CONST( "add" ), THREAD_PRIORITY_NORMAL, 0 );
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], 0 );

	test_wait_for_threads_startup( threads, num_threads );

	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] ); //No terminate, wait to self-exit

	test_wait_for_threads_exit( threads, num_threads );

	EXPECT_EQ( atomic_load32( &val_32 ), 0 );
	EXPECT_EQ( atomic_load64( &val_64 ), 0 );

	return 0;
}


DECLARE_TEST( atomic, cas )
{
	size_t num_threads = math_clamp( system_hardware_threads() * 4, 4, 32 );
	size_t ithread;
	object_t threads[32];
	cas_value_t cas_values[32];

	for( ithread = 0; ithread < num_threads; ++ithread )
	{
		threads[ithread] = thread_create( cas_thread, STRING_CONST( "cas" ), THREAD_PRIORITY_NORMAL, 0 );
		cas_values[ithread].val_32 = (int32_t)ithread;
		cas_values[ithread].val_64 = (int64_t)ithread;
		cas_values[ithread].val_ptr = (void*)(uintptr_t)ithread;
	}
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], &cas_values[ithread] );

	test_wait_for_threads_startup( threads, num_threads );

	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] ); //No terminate, wait to self-exit

	test_wait_for_threads_exit( threads, num_threads );

	EXPECT_EQ( atomic_load32( &val_32 ), 0 );
	EXPECT_EQ( atomic_load64( &val_64 ), 0 );
	EXPECT_EQ( atomic_loadptr( &val_ptr ), 0 );

	return 0;
}


static void test_atomic_declare( void )
{
	ADD_TEST( atomic, incdec );
	ADD_TEST( atomic, add );
	ADD_TEST( atomic, cas );
}


static test_suite_t test_atomic_suite = {
	test_atomic_application,
	test_atomic_memory_system,
	test_atomic_config,
	test_atomic_declare,
	test_atomic_initialize,
	test_atomic_shutdown
};


#if BUILD_MONOLITHIC

int test_atomic_run( void );
int test_atomic_run( void )
{
	test_suite = test_atomic_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_atomic_suite;
}

#endif


