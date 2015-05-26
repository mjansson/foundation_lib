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
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation objectmap tests" ) );
	app.short_name = string_const( STRING_CONST( "test_objectmap" ) );
	app.config_dir = string_const( STRING_CONST( "test_objectmap" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_objectmap_memory_system( void )
{
	return memory_system_malloc();
}


static foundation_config_t test_objectmap_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
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
	object_base_t first;
	object_base_t second;

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
	object_base_t* lookup;
	FOUNDATION_UNUSED( thread );

	map = arg;
	objects = memory_allocate( 0, sizeof( object_base_t ) * 512, 0, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED );

	thread_sleep( 10 );

	for( loop = 0; loop < 32; ++loop )
	{
		thread_yield();

		for( obj = 0; obj < 512; ++obj )
		{
			objects[obj].id = objectmap_reserve( map );
			EXPECT_NE_MSGFORMAT( objects[obj].id, 0, "Unable to reserve slot for object num %d", obj );
			EXPECT_EQ_MSGFORMAT( objectmap_lookup( map, objects[obj].id ), 0, "Object num %d (%" PRIx64 ") already stored in map", obj, objects[obj].id );
			objectmap_set( map, objects[obj].id, objects + obj );
			lookup = objectmap_lookup( map, objects[obj].id );
			EXPECT_NE_MSGFORMAT( lookup, 0, "Object num %d (%" PRIx64 ") not set in map, got null on lookup", obj, objects[obj].id );
			EXPECT_EQ_MSGFORMAT( lookup, objects + obj, "Object %d (%" PRIx64 ") 0x%" PRIfixPTR " was not set at reserved slot in map, got object 0x%" PRIfixPTR, obj, objects[obj].id, (uintptr_t)( objects + obj ), (uintptr_t)lookup );
		}

		thread_yield();

		for( obj = 0; obj < 512; ++obj )
		{
			lookup = objectmap_lookup( map, objects[obj].id );
			EXPECT_NE_MSGFORMAT( lookup, 0, "Object num %d (%" PRIx64 ") not set in map, got null on lookup", obj, objects[obj].id );
			EXPECT_EQ_MSGFORMAT( lookup, objects + obj, "Object %d (%" PRIx64 ") 0x%" PRIfixPTR " was not set at reserved slot in map, got object 0x%" PRIfixPTR, obj, objects[obj].id, (uintptr_t)( objects + obj ), (uintptr_t)lookup );
			objectmap_free( map, objects[obj].id );
			lookup = objectmap_lookup( map, objects[obj].id );
			EXPECT_EQ_MSGFORMAT( lookup, 0, "Object num %d (%" PRIx64 ") still set in map, got non-null (0x%" PRIfixPTR ") on lookup", obj, objects[obj].id, (uintptr_t)lookup );
		}
	}

	memory_deallocate( objects );

	return 0;
}


DECLARE_TEST( objectmap, thread )
{
	objectmap_t* map;
	object_t thread[32];
	size_t ith;
	size_t num_threads = math_clamp( system_hardware_threads() * 4, 4, 32 );
	bool running = true;

	map = objectmap_allocate( 32000 );

	for( ith = 0; ith < num_threads; ++ith )
	{
		thread[ith] = thread_create( objectmap_thread, STRING_CONST( "objectmap_thread" ), THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], map );
	}

	test_wait_for_threads_startup( thread, num_threads );

	for( ith = 0; ith < num_threads; ++ith )
		thread_terminate( thread[ith] );

	while( running )
	{
		running = false;
		for( ith = 0; ith < num_threads; ++ith )
		{
			if( thread_is_running( thread[ith] ) )
			{
				running = true;
				thread_yield();
			}
		}
	}

	for( ith = 0; ith < num_threads; ++ith )
	{
		EXPECT_EQ( thread_result( thread[ith] ), 0 );
	}

	for( ith = 0; ith < num_threads; ++ith )
	{
		thread_destroy( thread[ith] );
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


static test_suite_t test_objectmap_suite = {
	test_objectmap_application,
	test_objectmap_memory_system,
	test_objectmap_config,
	test_objectmap_declare,
	test_objectmap_initialize,
	test_objectmap_shutdown
};


#if BUILD_MONOLITHIC

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
