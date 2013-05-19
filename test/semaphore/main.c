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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation semaphore tests";
	app.short_name = "test_semaphore";
	app.config_dir = "test_semaphore";
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

                   
DECLARE_TEST( semaphore, initialize )
{
	semaphore_t sem;

	semaphore_initialize( &sem, 0 );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_destroy( &sem );

	semaphore_initialize( &sem, 1 );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	semaphore_destroy( &sem );

	semaphore_initialize( &sem, 2 );
	EXPECT_TRUE( semaphore_wait( &sem ) );
	EXPECT_TRUE( semaphore_try_wait( &sem, 100 ) );
	EXPECT_FALSE( semaphore_try_wait( &sem, 100 ) );
	semaphore_destroy( &sem );
	
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
	EXPECT_LT( end - start, time_ticks_per_second() / 1000 );

	start = time_current();
	semaphore_try_wait( &sem, 500 );
	end = time_current();
	EXPECT_GE( end - start, time_ticks_per_second() / 2 );

	semaphore_destroy( &sem );

	return 0;
}


typedef struct
{
	semaphore_t   read;
	semaphore_t   write;
	int           loopcount;
	int           counter;
} semaphore_test_t;

void* semaphore_waiter( object_t thread, void* arg )
{
	semaphore_test_t* sem = arg;
	int loop;

	for( loop = 0; loop < sem->loopcount; ++loop )
	{
		thread_yield();
		semaphore_wait( &sem->read );
		++sem->counter;
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
	test.counter = 0;
	
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
		thread_destroy( thread[ith] );
		thread_yield();
	}

	test_wait_for_threads_exit( thread, 32 );

	EXPECT_EQ( test.counter, test.loopcount * 32 );
	EXPECT_EQ( failed_waits, 0 );

	semaphore_destroy( &test.read );
	semaphore_destroy( &test.write );
	
	return 0;
}


void test_declare( void )
{
	ADD_TEST( semaphore, initialize );
	ADD_TEST( semaphore, postwait );
	ADD_TEST( semaphore, threaded );
}
