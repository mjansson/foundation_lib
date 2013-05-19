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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation objectmap tests";
	app.short_name = "test_objectmap";
	app.config_dir = "test_objectmap";
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


void* objectmap_thread( object_t thread, void* arg )
{
	objectmap_t* map;
	object_base_t* objects;
	int obj;
	int loop;

	map = arg;
	objects = memory_allocate_zero( sizeof( object_base_t ) * 512, 0, MEMORY_TEMPORARY );
	
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

	map = objectmap_allocate( 32000 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread[ith] = thread_create( objectmap_thread, "objectmap_thread", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[ith], map );
	}

	test_wait_for_threads_startup( thread, 32 );

	for( ith = 0; ith < 32; ++ith )
	{
		thread_destroy( thread[ith] );
		thread_yield();
	}
	
	test_wait_for_threads_exit( thread, 32 );

	objectmap_deallocate( map );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( objectmap, initialize );
	ADD_TEST( objectmap, store );
	ADD_TEST( objectmap, thread );
}
