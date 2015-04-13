/* main.c  -  Foundation semaphore test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_semaphore_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation semaphore tests";
	app.short_name = "test_semaphore";
	app.config_dir = "test_semaphore";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_semaphore_memory_system( void )
{
	return memory_system_malloc();
}


static int test_semaphore_initialize( void )
{
	return 0;
}


static void test_semaphore_shutdown( void )
{
}


DECLARE_TEST( semaphore, initialize )
{
	semaphore_t sem;

	semaphore_initialize( &sem, 0 );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_finalize( &sem );

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_PNACL
	semaphore_initialize_named( &sem, "foundation_test", 0 );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_finalize( &sem );
#endif

	semaphore_initialize( &sem, 1 );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	semaphore_post( &sem ); //Restored value
	semaphore_finalize( &sem );

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_PNACL
	semaphore_initialize_named( &sem, "foundation_test", 1 );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	semaphore_post( &sem ); //Restored value
	semaphore_finalize( &sem );
#endif

	semaphore_initialize( &sem, 2 );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_post( &sem );
	semaphore_post( &sem ); //Restored value
	semaphore_finalize( &sem );

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_PNACL
	semaphore_initialize_named( &sem, "foundation_test", 2 );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_post( &sem );
	semaphore_post( &sem ); //Restored value
	semaphore_finalize( &sem );
#endif

	return 0;
}


DECLARE_TEST( semaphore, postwait )
{
	semaphore_t sem;
	tick_t start, end;

	semaphore_initialize( &sem, 0 );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	semaphore_post( &sem );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	semaphore_post( &sem );
	semaphore_post( &sem );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	start = time_current();
	semaphore_try_wait( &sem, 0 );
	end = time_current();
	EXPECT_LE( end - start, time_ticks_per_second() / 1000 );

	start = time_current();
	semaphore_try_wait( &sem, 510 );
	end = time_current();
	EXPECT_GE( end - start, time_ticks_per_second() / 2 );

	semaphore_finalize( &sem );

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_PNACL
	semaphore_initialize_named( &sem, "foundation_test", 0 );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	semaphore_post( &sem );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	semaphore_post( &sem );
	semaphore_post( &sem );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );

	start = time_current();
	semaphore_try_wait( &sem, 0 );
	end = time_current();
	EXPECT_LE( end - start, time_ticks_per_second() / 1000 );

	start = time_current();
	semaphore_try_wait( &sem, 510 );
	end = time_current();
	EXPECT_GE( end - start, time_ticks_per_second() / 2 );

	semaphore_finalize( &sem );
#endif

	return 0;
}


typedef struct
{
	semaphore_t   read;
	semaphore_t   write;
	int           loopcount;
	atomic32_t    counter;
} semaphore_test_t;

static void* semaphore_waiter( object_t thread, void* arg )
{
	semaphore_test_t* sem = arg;
	int loop;
	FOUNDATION_UNUSED( thread );

	for( loop = 0; loop < sem->loopcount; ++loop )
	{
		thread_yield();
		semaphore_wait( &sem->read );
		atomic_incr32( &sem->counter );
		semaphore_post( &sem->write );
	}

	return 0;
}


DECLARE_TEST( semaphore, threaded )
{
	object_t thread[32];
	int ith;
	int failed_waits;

	semaphore_test_t test;
	semaphore_initialize( &test.read, 0 );
	semaphore_initialize( &test.write, 0 );
	test.loopcount = 128;
	atomic_store32( &test.counter, 0 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( semaphore_waiter, "semaphore_waiter", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], &test );
	}

	test_wait_for_threads_startup( thread, 32 );

	failed_waits = 0;
	for( ith = 0; ith < test.loopcount * 32; ++ith )
	{
		semaphore_post( &test.read );
		thread_yield();
		if( !semaphore_try_wait( &test.write, 200 ) )
		{
			failed_waits++;
			EXPECT_TRUE( semaphore_wait( &test.write ) );
		}
	}

	for( ith = 0; ith < 32; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
		thread_yield();
	}

	test_wait_for_threads_exit( thread, 32 );

	EXPECT_EQ( atomic_load32( &test.counter ), test.loopcount * 32 );
	EXPECT_EQ( failed_waits, 0 );

	semaphore_finalize( &test.read );
	semaphore_finalize( &test.write );

#if !FOUNDATION_PLATFORM_IOS && !FOUNDATION_PLATFORM_PNACL
	semaphore_initialize_named( &test.read, "foundation_test_read", 0 );
	semaphore_initialize_named( &test.write, "foundation_test_write", 0 );
	test.loopcount = 128;
	atomic_store32( &test.counter, 0 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( semaphore_waiter, "semaphore_waiter", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], &test );
	}

	test_wait_for_threads_startup( thread, 32 );

	failed_waits = 0;
	for( ith = 0; ith < test.loopcount * 32; ++ith )
	{
		semaphore_post( &test.read );
		thread_yield();
		if( !semaphore_try_wait( &test.write, 200 ) )
		{
			failed_waits++;
			EXPECT_TRUE( semaphore_wait( &test.write ) );
		}
	}

	EXPECT_EQ( atomic_load32( &test.counter ), test.loopcount * 32 );
	EXPECT_EQ( failed_waits, 0 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
		thread_yield();
	}

	test_wait_for_threads_exit( thread, 32 );

	semaphore_finalize( &test.read );
	semaphore_finalize( &test.write );
#endif

	return 0;
}


static void test_semaphore_declare( void )
{
	ADD_TEST( semaphore, initialize );
	ADD_TEST( semaphore, postwait );
	ADD_TEST( semaphore, threaded );
}


test_suite_t test_semaphore_suite = {
	test_semaphore_application,
	test_semaphore_memory_system,
	test_semaphore_declare,
	test_semaphore_initialize,
	test_semaphore_shutdown
};


#if BUILD_MONOLITHIC

int test_semaphore_run( void );
int test_semaphore_run( void )
{
	test_suite = test_semaphore_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_semaphore_suite;
}

#endif
