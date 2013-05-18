/* main.c  -  Foundation hashmap test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation hashmap tests";
	app.short_name = "test_hashmap";
	app.config_dir = "test_hashmap";
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

                   
DECLARE_TEST( hashmap, allocation )
{
	hashmap_t* map = hashmap_allocate( 0, 0 );

	EXPECT_EQ( hashmap_size( map ), 0 );
	EXPECT_EQ( hashmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( hashmap_lookup( map, (hash_t)(uintptr_t)map ), 0 );

	hashmap_deallocate( map );

	map = hashmap_allocate( 13, 127 );

	EXPECT_EQ( hashmap_size( map ), 0 );
	EXPECT_EQ( hashmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( hashmap_lookup( map, (hash_t)(uintptr_t)map ), 0 );

	hashmap_deallocate( map );
	
	return 0;
}


DECLARE_TEST( hashmap, insert )
{
	hashmap_t* map = hashmap_allocate( 0, 0 );
	void* prev = 0;
	
	EXPECT_EQ( hashmap_lookup( map, 0 ), 0 );

	prev = hashmap_insert( map, 0, map );
	EXPECT_EQ( prev, 0 );
	
	prev = hashmap_insert( map, 0, map );
	EXPECT_EQ( prev, map );
	
	prev = hashmap_insert( map, 0, 0 );
	EXPECT_EQ( prev, map );

	prev = hashmap_insert( map, 0, map );
	EXPECT_EQ( prev, 0 );
	
	prev = hashmap_insert( map, (hash_t)(uintptr_t)map, map );
	EXPECT_EQ( prev, 0 );

	EXPECT_EQ( hashmap_size( map ), 2 );
	EXPECT_EQ( hashmap_lookup( map, 0 ), map );
	EXPECT_EQ( hashmap_lookup( map, (hash_t)(uintptr_t)map ), map );

	hashmap_insert( map, 0, 0 );
	hashmap_insert( map, (hash_t)(uintptr_t)map, 0 );

	EXPECT_EQ( hashmap_size( map ), 2 );
	EXPECT_EQ( hashmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( hashmap_lookup( map, (hash_t)(uintptr_t)map ), 0 );

	hashmap_deallocate( map );
	
	return 0;
}


DECLARE_TEST( hashmap, erase )
{
	hashmap_t* map = hashmap_allocate( 0, 0 );
	void* prev = 0;
	
	EXPECT_EQ( hashmap_lookup( map, 0 ), 0 );
	EXPECT_EQ( hashmap_size( map ), 0 );

	prev = hashmap_insert( map, 0, map );
	EXPECT_EQ( prev, 0 );
	EXPECT_EQ( hashmap_size( map ), 1 );
	EXPECT_TRUE( hashmap_has_key( map, 0 ) );
	
	prev = hashmap_erase( map, 0 );
	EXPECT_EQ( prev, map );
	EXPECT_EQ( hashmap_size( map ), 0 );
	EXPECT_FALSE( hashmap_has_key( map, 0 ) );
	
	prev = hashmap_erase( map, 0 );
	EXPECT_EQ( prev, 0 );
	EXPECT_EQ( hashmap_size( map ), 0 );
	EXPECT_FALSE( hashmap_has_key( map, 0 ) );
	
	prev = hashmap_erase( map, (hash_t)(uintptr_t)map );
	EXPECT_EQ( prev, 0 );
	EXPECT_EQ( hashmap_size( map ), 0 );	
	EXPECT_FALSE( hashmap_has_key( map, (hash_t)(uintptr_t)map ) );

	hashmap_deallocate( map );

	return 0;
}


DECLARE_TEST( hashmap, lookup )
{
	hashmap_t* map = hashmap_allocate( 31, 0 );
	char* value = (void*)(uintptr_t)1234;
	hash_t key = (hash_t)4321;
	unsigned int ikey = 0;

	for( ; ikey < 1024; ++ikey, ++key, ++value )
	{
		void* prev = hashmap_insert( map, key, value );
		EXPECT_EQ( prev, 0 );
	}
	
	for( ikey = 0, key = (hash_t)4321, value = (void*)(uintptr_t)1234; ikey < 1024; ++ikey, ++key, ++value )
	{
		void* prev = hashmap_lookup( map, key );
		EXPECT_EQ( prev, value );

		EXPECT_TRUE( hashmap_has_key( map, key ) );
		hashmap_erase( map, key );
		EXPECT_FALSE( hashmap_has_key( map, key ) );		
	}

	hashmap_deallocate( map );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( hashmap, allocation );
	ADD_TEST( hashmap, insert );
	ADD_TEST( hashmap, erase );
	ADD_TEST( hashmap, lookup );
}
