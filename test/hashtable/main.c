/* main.c  -  Foundation hashtable test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_hashtable_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation hashtable tests";
	app.short_name = "test_hashtable";
	app.config_dir = "test_hashtable";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_hashtable_memory_system( void )
{
	return memory_system_malloc();
}


static int test_hashtable_initialize( void )
{
	return 0;
}


static void test_hashtable_shutdown( void )
{
}


typedef struct
{
	hashtable32_t*       table;
	uint32_t             key_offset;
	uint32_t             key_num;
} producer32_arg_t;


typedef struct
{
	hashtable64_t*       table;
	uint64_t             key_offset;
	uint64_t             key_num;
} producer64_arg_t;


static void* producer32_thread( object_t thread, void* arg )
{
	producer32_arg_t* parg = arg;
	hashtable32_t* table = parg->table;
	uint32_t key_offset = parg->key_offset;
	uint32_t key;
	FOUNDATION_UNUSED( thread );

	for( key = 1; key < parg->key_num; ++key )
		hashtable32_set( table, key + key_offset, key + key_offset );

	thread_yield();

	for( key = 1; key < parg->key_num; ++key )
		hashtable32_erase( table, key + key_offset );

	thread_yield();

	for( key = 1; key < parg->key_num; ++key )
		hashtable32_set( table, key + key_offset, 1 + ( ( key + key_offset ) % 17 ) );

	return 0;
}


static void* producer64_thread( object_t thread, void* arg )
{
	producer64_arg_t* parg = arg;
	hashtable64_t* table = parg->table;
	uint64_t key_offset = parg->key_offset;
	uint64_t key;
	FOUNDATION_UNUSED( thread );

	for( key = 1; key < parg->key_num; ++key )
		hashtable64_set( table, key + key_offset, key + key_offset );

	thread_yield();

	for( key = 1; key < parg->key_num; ++key )
		hashtable64_erase( table, key + key_offset );

	thread_yield();

	for( key = 1; key < parg->key_num; ++key )
		hashtable64_set( table, key + key_offset, 1 + ( ( key + key_offset ) % 17 ) );

	return 0;
}


DECLARE_TEST( hashtable, 32bit_basic )
{
	hashtable32_t* table = hashtable32_allocate( 1024 );

	EXPECT_EQ( hashtable32_size( table ), 0 );

	hashtable32_set( table, 1, 1 );
	EXPECT_EQ( hashtable32_get( table, 1 ), 1 );

	hashtable32_erase( table, 1 );
	EXPECT_EQ( hashtable32_get( table, 1 ), 0 );

	hashtable32_set( table, 1, 2 );
	EXPECT_EQ( hashtable32_get( table, 1 ), 2 );

	hashtable32_set( table, 1, 3 );
	EXPECT_EQ( hashtable32_get( table, 1 ), 3 );

	hashtable32_set( table, 2, 1 );
	EXPECT_EQ( hashtable32_get( table, 2 ), 1 );

	hashtable32_erase( table, 1 );
	EXPECT_EQ( hashtable32_get( table, 1 ), 0 );
	EXPECT_EQ( hashtable32_get( table, 2 ), 1 );

	hashtable32_erase( table, 2 );
	EXPECT_EQ( hashtable32_get( table, 2 ), 0 );

	hashtable32_deallocate( table );

	return 0;
}


DECLARE_TEST( hashtable, 32bit_threaded )
{
	object_t thread[32];
	producer32_arg_t args[32];
	int i, j;
	int num_threads = 32;

	hashtable32_t* table = hashtable32_allocate( 32 * 16789 + 65536 );

	EXPECT_EQ( hashtable32_size( table ), 0 );

	num_threads = math_clamp( system_hardware_threads() * 2, 4, 32 );
	for( i = 0; i < num_threads; ++i )
	{
		args[i].table = table;
		args[i].key_offset = 1 + ( i * 16789 );
		args[i].key_num = 65535;

		thread[i] = thread_create( producer32_thread, "table_producer", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], args + i );
	}

	test_wait_for_threads_startup( thread, num_threads );

	for( i = 0; i < num_threads; ++i )
	{
		thread_terminate( thread[i] );
		thread_destroy( thread[i] );
	}

	test_wait_for_threads_exit( thread, num_threads );

	for( i = 0; i < num_threads; ++i )
	{
		for( j = 1; j < 65535; ++j )
		{
			uint32_t key = ( 1 + ( i * 16789 ) ) + j;
			EXPECT_EQ( hashtable32_get( table, key ), 1 + ( key % 17 ) );
		}
	}

	hashtable32_deallocate( table );

	return 0;
}


DECLARE_TEST( hashtable, 64bit_basic )
{
	hashtable64_t* table = hashtable64_allocate( 1024 );

	EXPECT_EQ( hashtable64_size( table ), 0 );

	hashtable64_set( table, 1, 1 );
	EXPECT_EQ( hashtable64_get( table, 1 ), 1 );

	hashtable64_erase( table, 1 );
	EXPECT_EQ( hashtable64_get( table, 1 ), 0 );

	hashtable64_set( table, 1, 2 );
	EXPECT_EQ( hashtable64_get( table, 1 ), 2 );

	hashtable64_set( table, 1, 3 );
	EXPECT_EQ( hashtable64_get( table, 1 ), 3 );

	hashtable64_set( table, 2, 1 );
	EXPECT_EQ( hashtable64_get( table, 2 ), 1 );

	hashtable64_erase( table, 1 );
	EXPECT_EQ( hashtable64_get( table, 1 ), 0 );
	EXPECT_EQ( hashtable64_get( table, 2 ), 1 );

	hashtable64_erase( table, 2 );
	EXPECT_EQ( hashtable64_get( table, 2 ), 0 );

	hashtable64_deallocate( table );

	return 0;
}


DECLARE_TEST( hashtable, 64bit_threaded )
{
	object_t thread[32];
	producer64_arg_t args[32];
	int i, j;
	int num_threads = 0;

	hashtable64_t* table = hashtable64_allocate( 32 * 16789 + 65536 );

	EXPECT_EQ( hashtable64_size( table ), 0 );

	num_threads = math_clamp( system_hardware_threads() * 2, 4, 32 );
	for( i = 0; i < num_threads; ++i )
	{
		args[i].table = table;
		args[i].key_offset = 1 + ( i * 16789 );
		args[i].key_num = 65535;

		thread[i] = thread_create( producer64_thread, "table_producer", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], args + i );
	}

	test_wait_for_threads_startup( thread, num_threads );

	for( i = 0; i < num_threads; ++i )
	{
		thread_terminate( thread[i] );
		thread_destroy( thread[i] );
	}

	test_wait_for_threads_exit( thread, num_threads );

	for( i = 0; i < num_threads; ++i )
	{
		for( j = 1; j < 65535; ++j )
		{
			uint32_t key = ( 1 + ( i * 16789 ) ) + j;
			EXPECT_EQ( hashtable64_get( table, key ), 1 + ( key % 17 ) );
		}
	}

	hashtable64_deallocate( table );

	return 0;
}


static void test_hashtable_declare( void )
{
	ADD_TEST( hashtable, 32bit_basic );
	ADD_TEST( hashtable, 32bit_threaded );
	ADD_TEST( hashtable, 64bit_basic );
	ADD_TEST( hashtable, 64bit_threaded );
}


test_suite_t test_hashtable_suite = {
	test_hashtable_application,
	test_hashtable_memory_system,
	test_hashtable_declare,
	test_hashtable_initialize,
	test_hashtable_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_PNACL

int test_hashtable_run( void );
int test_hashtable_run( void )
{
	test_suite = test_hashtable_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_hashtable_suite;
}

#endif
