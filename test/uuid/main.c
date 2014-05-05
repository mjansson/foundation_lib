/* main.c  -  Foundation uuid test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_uuid_application( void )
{
	application_t app = {0};
	app.name = "Foundation uuid tests";
	app.short_name = "test_uuid";
	app.config_dir = "test_uuid";
	app.flags = APPLICATION_UTILITY;
	return app;
}


static memory_system_t test_uuid_memory_system( void )
{
	return memory_system_malloc();
}


static int test_uuid_initialize( void )
{
	return 0;
}


static void test_uuid_shutdown( void )
{
}


DECLARE_TEST( uuid, generate )
{
	int iloop;
	uuid_t uuid, uuid_ref;
	char name_str[] = "com.rampantpixels.foundation.uuid.000000";

	uuid = uuid_null();
	uuid_ref = uuid_null();

	EXPECT_TRUE( uuid_is_null( uuid ) );
	EXPECT_TRUE( uuid_is_null( uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid_ref ) );

	//Random based
	uuid = uuid_generate_random();
	uuid_ref = uuid_null();
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_TRUE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_generate_random();
	uuid_ref = uuid_generate_random();

	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_ref;
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	for( iloop = 0; iloop < 64000; ++iloop )
	{
		uuid_ref = uuid;
		uuid = uuid_generate_random();
	
		EXPECT_FALSE( uuid_is_null( uuid ) );
		EXPECT_FALSE( uuid_is_null( uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );
	}
	
	//Time based
	uuid = uuid_generate_time();
	uuid_ref = uuid_null();
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_TRUE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_generate_time();
	uuid_ref = uuid_generate_time();

	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_ref;
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	for( iloop = 0; iloop < 64000; ++iloop )
	{
		uuid_ref = uuid;
		uuid = uuid_generate_time();
	
		EXPECT_FALSE( uuid_is_null( uuid ) );
		EXPECT_FALSE( uuid_is_null( uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );
	}
	
	//Name based
	uuid = uuid_generate_name( UUID_DNS, "com.rampantpixels.foundation.uuid" );
	uuid_ref = uuid_null();
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_TRUE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_generate_name( UUID_DNS, "com.rampantpixels.foundation.uuid.1" );
	uuid_ref = uuid_generate_name( UUID_DNS, "com.rampantpixels.foundation.uuid.2" );

	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	uuid = uuid_generate_name( UUID_DNS, "com.rampantpixels.foundation.uuid.2" );
	
	EXPECT_FALSE( uuid_is_null( uuid ) );
	EXPECT_FALSE( uuid_is_null( uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid_ref ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid, uuid ) );
	EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );

	for( iloop = 0; iloop < 10000; ++iloop )
	{
		string_format_buffer( name_str, 40, "com.rampantpixels.foundation.uuid.%05u", iloop );
		
		uuid_ref = uuid;
		uuid = uuid_generate_name( UUID_DNS, name_str );
	
		EXPECT_FALSE( uuid_is_null( uuid ) );
		EXPECT_FALSE( uuid_is_null( uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid, uuid_ref ) );
		EXPECT_FALSE( uuid_equal( uuid_ref, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid, uuid ) );
		EXPECT_TRUE( uuid_equal( uuid_ref, uuid_ref ) );
	}
	
	return 0;
}


static uuid_t uuid_thread_store[32][8192];

static void* uuid_thread_time( object_t thread, void* arg )
{
	int i;
	int ithread = (int)(uintptr_t)arg;

	for( i = 0; i < 8192; ++i )
		uuid_thread_store[ithread][i] = uuid_generate_time();
	
	return 0;
}


DECLARE_TEST( uuid, threaded )
{
	object_t thread[32];
	int ith, i, jth, j;
	int num_threads = math_clamp( system_hardware_threads() * 2, 4, 32 );

	for( ith = 0; ith < num_threads; ++ith )
	{
		thread[ith] = thread_create( uuid_thread_time, "uuid_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], (void*)(uintptr_t)ith );
	}

	test_wait_for_threads_startup( thread, num_threads );
	
	for( ith = 0; ith < num_threads; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
	}

	test_wait_for_threads_exit( thread, num_threads );

	for( ith = 0; ith < num_threads; ++ith )
	{
		for( i = 0; i < 8192; ++i )
		{
			for( jth = ith + 1; jth < num_threads; ++jth )
			{
				for( j = 0; j < 8192; ++j )
				{
					EXPECT_FALSE( uuid_equal( uuid_thread_store[ith][i], uuid_thread_store[jth][j] ) );
				}
			}
			for( j = i + 1; j < 8192; ++j )
			{
				EXPECT_FALSE( uuid_equal( uuid_thread_store[ith][i], uuid_thread_store[ith][j] ) );
			}
		}
	}
	
	return 0;
}


static void test_uuid_declare( void )
{
	ADD_TEST( uuid, generate );
	ADD_TEST( uuid, threaded );
}


test_suite_t test_uuid_suite = {
	test_uuid_application,
	test_uuid_memory_system,
	test_uuid_declare,
	test_uuid_initialize,
	test_uuid_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS

int test_uuid_run( void );
int test_uuid_run( void )
{
	test_suite = test_uuid_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_uuid_suite;
}

#endif
