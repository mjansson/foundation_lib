/* main.c  -  Foundation objectmap test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_objectmap_application( void )
{
	application_t app = {0};
	app.name = "Foundation objectmap tests";
	app.short_name = "test_objectmap";
	app.config_dir = "test_objectmap";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_objectmap_memory_system( void )
{
	return memory_system_malloc();
}


static int test_objectmap_initialize( void )
{
	return 0;
}


static void test_objectmap_shutdown( void )
{
}


DECLARE_TEST( objectmap, initialize )
{
	objectmap_t* map;

	map = objectmap_allocate( 129 );
	EXPECT_EQ( objectmap_size( map ), 129 );
	EXPECT_EQ( objectmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, 1 ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), 0 );

	objectmap_deallocate( map );
	
	return 0;
}


DECLARE_TEST( objectmap, store )
{
	objectmap_t* map;
	object_base_t first = {0};
	object_base_t second = {0};
	
	map = objectmap_allocate( 129 );

	EXPECT_EQ( objectmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, 1 ), 0 );

	first.id = objectmap_reserve( map );
	EXPECT_EQ( objectmap_lookup( map, first.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );

	second.id = objectmap_reserve( map );
	EXPECT_EQ( objectmap_lookup( map, first.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, second.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), 0 );

	objectmap_set( map, first.id, &first );
	EXPECT_EQ( objectmap_lookup( map, first.id ), &first );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), &first );
	EXPECT_EQ( objectmap_lookup( map, second.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), 0 );
	
	objectmap_set( map, second.id, &second );
	EXPECT_EQ( objectmap_lookup( map, first.id ), &first );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), &first );
	EXPECT_EQ( objectmap_lookup( map, second.id ), &second );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), &second );	

	objectmap_free( map, first.id );
	EXPECT_EQ( objectmap_lookup( map, first.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, second.id ), &second );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), &second );	

	objectmap_free( map, first.id );
	EXPECT_EQ( objectmap_lookup( map, first.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, second.id ), &second );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), &second );	

	objectmap_free( map, second.id );
	EXPECT_EQ( objectmap_lookup( map, first.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 0 ), 0 );
	EXPECT_EQ( objectmap_lookup( map, second.id ), 0 );
	EXPECT_EQ( objectmap_raw_lookup( map, 1 ), 0 );	
	
	objectmap_deallocate( map );

	return 0;
}


static void* objectmap_thread( object_t thread, void* arg )
{
	objectmap_t* map;
	object_base_t* objects;
	int obj;
	int loop;

	map = arg;
	objects = memory_allocate( 0, sizeof( object_base_t ) * 512, 0, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED );
	
	thread_sleep( 10 );
	
	for( loop = 0; loop < 32; ++loop )
	{
		thread_yield();
	
		for( obj = 0; obj < 512; ++obj )
		{
			objects[obj].id = objectmap_reserve( map );
			EXPECT_NE( objects[obj].id, 0 );
			EXPECT_EQ( objectmap_lookup( map, objects[obj].id ), 0 );
			objectmap_set( map, objects[obj].id, objects + obj );
			EXPECT_EQ( objectmap_lookup( map, objects[obj].id ), objects + obj );
		}

		thread_yield();
	
		for( obj = 0; obj < 512; ++obj )
		{
			EXPECT_EQ( objectmap_lookup( map, objects[obj].id ), objects + obj );
			objectmap_free( map, objects[obj].id );
			EXPECT_EQ( objectmap_lookup( map, objects[obj].id ), 0 );
		}
	}
	
	memory_deallocate( objects );

	return 0;
}


DECLARE_TEST( objectmap, thread )
{
	objectmap_t* map;
	object_t thread[32];
	int ith;
	int num_threads = math_clamp( system_hardware_threads() * 4, 4, 32 );

	map = objectmap_allocate( 32000 );

	for( ith = 0; ith < num_threads; ++ith )
	{
		thread[ith] = thread_create( objectmap_thread, "objectmap_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], map );
	}

	test_wait_for_threads_startup( thread, num_threads );

	for( ith = 0; ith < num_threads; ++ith )
	{
		thread_terminate( thread[ith] );
		thread_destroy( thread[ith] );
		thread_yield();
	}
	
	test_wait_for_threads_exit( thread, num_threads );

	objectmap_deallocate( map );

	return 0;
}


static void test_objectmap_declare( void )
{
	ADD_TEST( objectmap, initialize );
	ADD_TEST( objectmap, store );
	ADD_TEST( objectmap, thread );
}


test_suite_t test_objectmap_suite = {
	test_objectmap_application,
	test_objectmap_memory_system,
	test_objectmap_declare,
	test_objectmap_initialize,
	test_objectmap_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS

int test_objectmap_run( void );
int test_objectmap_run( void )
{
	test_suite = test_objectmap_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_objectmap_suite;
}

#endif
