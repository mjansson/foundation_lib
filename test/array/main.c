/* main.c  -  Foundation array test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation array tests";
	app.short_name = "test_array";
	app.config_dir = "test_array";
	return app;
}

typedef struct _basic_type
{
	int              intval;
	object_t         objval;
} basic_t;


typedef struct _complex_type
{
	int              intval;
	union
	{
		int          intval;
		real         realval;
		void*        ptrval;
		basic_t      basicval;
		object_t     objval;
	}                unionval;
	char             charval;
	basic_t          basicval;
	void*            ptrval;
} complex_t;


DECLARE_TEST( array, allocation )
{
	void**     array_ptr = 0;
	int*       array_int = 0;
	object_t*  array_obj = 0;
	basic_t*   array_basic = 0;
	complex_t* array_complex = 0;
	
	EXPECT_EQ( array_size( array_ptr ), 0 );
	EXPECT_EQ( array_size( array_int ), 0 );
	EXPECT_EQ( array_size( array_obj ), 0 );
	EXPECT_EQ( array_size( array_basic ), 0 );
	EXPECT_EQ( array_size( array_complex ), 0 );

	EXPECT_EQ( array_capacity( array_ptr ), 0 );
	EXPECT_EQ( array_capacity( array_int ), 0 );
	EXPECT_EQ( array_capacity( array_obj ), 0 );
	EXPECT_EQ( array_capacity( array_basic ), 0 );
	EXPECT_EQ( array_capacity( array_complex ), 0 );
	
	// Reserve
	{
		array_reserve( array_ptr, 0 );
		array_reserve( array_int, 0 );
		array_reserve( array_obj, 0 );
		array_reserve( array_basic, 0 );
		array_reserve( array_complex, 0 );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 0 );
		EXPECT_EQ( array_capacity( array_int ), 0 );
		EXPECT_EQ( array_capacity( array_obj ), 0 );
		EXPECT_EQ( array_capacity( array_basic ), 0 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		EXPECT_EQ( array_ptr, 0 );
		EXPECT_EQ( array_int, 0 );
		EXPECT_EQ( array_obj, 0 );
		EXPECT_EQ( array_basic, 0 );
		EXPECT_EQ( array_complex, 0 );

		array_reserve( array_ptr, 32 );
		array_reserve( array_int, 33 );
		array_reserve( array_obj, 63 );
		array_reserve( array_basic, 1024 );
		array_reserve( array_complex, 32742 );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 32 );
		EXPECT_EQ( array_capacity( array_int ), 33 );
		EXPECT_EQ( array_capacity( array_obj ), 63 );
		EXPECT_EQ( array_capacity( array_basic ), 1024 );
		EXPECT_EQ( array_capacity( array_complex ), 32742 );
	}
	// Deallocate
	{
		array_deallocate( array_ptr );
		array_deallocate( array_int );
		array_deallocate( array_obj );
		array_deallocate( array_basic );
		array_deallocate( array_complex );
	
		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 0 );
		EXPECT_EQ( array_capacity( array_int ), 0 );
		EXPECT_EQ( array_capacity( array_obj ), 0 );
		EXPECT_EQ( array_capacity( array_basic ), 0 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		EXPECT_EQ( array_ptr, 0 );
		EXPECT_EQ( array_int, 0 );
		EXPECT_EQ( array_obj, 0 );
		EXPECT_EQ( array_basic, 0 );
		EXPECT_EQ( array_complex, 0 );
	}
	// Grow
	{
		array_grow( array_ptr, 75284 );
		array_grow( array_int, 8295 );
		array_grow( array_obj, 610 );
		array_grow( array_basic, 24 );
		array_grow( array_complex, 0 );

		EXPECT_EQ( array_size( array_ptr ), 75284 );
		EXPECT_EQ( array_size( array_int ), 8295 );
		EXPECT_EQ( array_size( array_obj ), 610 );
		EXPECT_EQ( array_size( array_basic ), 24 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 75284 );
		EXPECT_EQ( array_capacity( array_int ), 8295 );
		EXPECT_EQ( array_capacity( array_obj ), 610 );
		EXPECT_EQ( array_capacity( array_basic ), 24 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		array_grow( array_ptr, 75284 );
		array_grow( array_int, 8295 );
		array_grow( array_obj, 610 );
		array_grow( array_basic, 24 );
		array_grow( array_complex, 0 );

		EXPECT_EQ( array_size( array_ptr ), 75284 * 2 );
		EXPECT_EQ( array_size( array_int ), 8295 * 2 );
		EXPECT_EQ( array_size( array_obj ), 610 * 2 );
		EXPECT_EQ( array_size( array_basic ), 24 * 2 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 75284 * 2 );
		EXPECT_EQ( array_capacity( array_int ), 8295 * 2 );
		EXPECT_EQ( array_capacity( array_obj ), 610 * 2 );
		EXPECT_EQ( array_capacity( array_basic ), 24 * 2 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		array_grow( array_ptr, -1 );
		array_grow( array_int, -2 );
		array_grow( array_obj, -3 );
		array_grow( array_basic, -4 );
		array_grow( array_complex, -5 );

		EXPECT_EQ( array_size( array_ptr ), 75284 * 2 - 1 );
		EXPECT_EQ( array_size( array_int ), 8295 * 2 - 2 );
		EXPECT_EQ( array_size( array_obj ), 610 * 2 - 3 );
		EXPECT_EQ( array_size( array_basic ), 24 * 2 - 4 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 75284 * 2 );
		EXPECT_EQ( array_capacity( array_int ), 8295 * 2 );
		EXPECT_EQ( array_capacity( array_obj ), 610 * 2 );
		EXPECT_EQ( array_capacity( array_basic ), 24 * 2 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		EXPECT_EQ( array_complex, 0 );
	}
	// Clear
	{
		array_clear( array_ptr );
		array_clear( array_int );
		array_clear( array_obj );
		array_clear( array_basic );
		array_clear( array_complex );
	
		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 75284 * 2 );
		EXPECT_EQ( array_capacity( array_int ), 8295 * 2 );
		EXPECT_EQ( array_capacity( array_obj ), 610 * 2 );
		EXPECT_EQ( array_capacity( array_basic ), 24 * 2 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		EXPECT_EQ( array_complex, 0 );

		array_deallocate( array_ptr );
		array_deallocate( array_int );
		array_deallocate( array_obj );
		array_deallocate( array_basic );
		array_deallocate( array_complex );
	
		array_clear( array_ptr );
		array_clear( array_int );
		array_clear( array_obj );
		array_clear( array_basic );
		array_clear( array_complex );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 0 );
		EXPECT_EQ( array_capacity( array_int ), 0 );
		EXPECT_EQ( array_capacity( array_obj ), 0 );
		EXPECT_EQ( array_capacity( array_basic ), 0 );
		EXPECT_EQ( array_capacity( array_complex ), 0 );

		EXPECT_EQ( array_ptr, 0 );
		EXPECT_EQ( array_int, 0 );
		EXPECT_EQ( array_obj, 0 );
		EXPECT_EQ( array_basic, 0 );
		EXPECT_EQ( array_complex, 0 );
	}
	// Reserve + grow
	{
		void**     array_ptr_prev = 0;
		int*       array_int_prev = 0;
		object_t*  array_obj_prev = 0;
		basic_t*   array_basic_prev = 0;
		complex_t* array_complex_prev = 0;

		array_reserve( array_ptr, 1 );
		array_reserve( array_int, 2 );
		array_reserve( array_obj, 3 );
		array_reserve( array_basic, 4 );
		array_reserve( array_complex, 5 );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 2 );
		EXPECT_EQ( array_capacity( array_obj ), 3 );
		EXPECT_EQ( array_capacity( array_basic ), 4 );
		EXPECT_EQ( array_capacity( array_complex ), 5 );

		EXPECT_NE( array_ptr, 0 );
		EXPECT_NE( array_int, 0 );
		EXPECT_NE( array_obj, 0 );
		EXPECT_NE( array_basic, 0 );
		EXPECT_NE( array_complex, 0 );

		array_ptr_prev = array_ptr;
		array_int_prev = array_int;
		array_obj_prev = array_obj;
		array_basic_prev = array_basic;
		array_complex_prev = array_complex;

		array_grow( array_ptr, 1 );
		array_grow( array_int, 2 );
		array_grow( array_obj, 3 );
		array_grow( array_basic, 4 );
		array_grow( array_complex, 5 );

		EXPECT_EQ( array_size( array_ptr ), 1 );
		EXPECT_EQ( array_size( array_int ), 2 );
		EXPECT_EQ( array_size( array_obj ), 3 );
		EXPECT_EQ( array_size( array_basic ), 4 );
		EXPECT_EQ( array_size( array_complex ), 5 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 2 );
		EXPECT_EQ( array_capacity( array_obj ), 3 );
		EXPECT_EQ( array_capacity( array_basic ), 4 );
		EXPECT_EQ( array_capacity( array_complex ), 5 );

		EXPECT_EQ( array_ptr, array_ptr_prev );
		EXPECT_EQ( array_int, array_int_prev );
		EXPECT_EQ( array_obj, array_obj_prev );
		EXPECT_EQ( array_basic, array_basic_prev );
		EXPECT_EQ( array_complex, array_complex_prev );

		array_grow( array_ptr, 1 );
		array_grow( array_int, 2 );
		array_grow( array_obj, 3 );
		array_grow( array_basic, 4 );
		array_grow( array_complex, 5 );

		EXPECT_EQ( array_size( array_ptr ), 1 * 2 );
		EXPECT_EQ( array_size( array_int ), 2 * 2 );
		EXPECT_EQ( array_size( array_obj ), 3 * 2 );
		EXPECT_EQ( array_size( array_basic ), 4 * 2 );
		EXPECT_EQ( array_size( array_complex ), 5 * 2 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 * 2 );
		EXPECT_EQ( array_capacity( array_int ), 2 * 2 );
		EXPECT_EQ( array_capacity( array_obj ), 3 * 2 );
		EXPECT_EQ( array_capacity( array_basic ), 4 * 2 );
		EXPECT_EQ( array_capacity( array_complex ), 5 * 2 );

		EXPECT_NE( array_ptr, array_ptr_prev );
		EXPECT_NE( array_int, array_int_prev );
		EXPECT_NE( array_obj, array_obj_prev );
		EXPECT_NE( array_basic, array_basic_prev );
		EXPECT_NE( array_complex, array_complex_prev );
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	return 0;
}


DECLARE_TEST( array, copy )
{
	int        i;
	void**     array_ptr = 0;
	int*       array_int = 0;
	object_t*  array_obj = 0;
	basic_t*   array_basic = 0;
	complex_t* array_complex = 0;

	void**     copy_ptr = 0;
	int*       copy_int = 0;
	object_t*  copy_obj = 0;
	basic_t*   copy_basic = 0;
	complex_t* copy_complex = 0;

	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i;
		complex.basicval.objval = i + 1;
		complex.intval = i + 2;
		complex.ptrval = 0;
		complex.charval = (char)i;
		complex.unionval.basicval.intval = i + 3;
		complex.unionval.basicval.objval = i + 4;

		array_push( array_ptr, (void*)((uintptr_t)i) );
		array_push( array_int, i );
		array_push( array_obj, i );
		array_push( array_basic, basic );
		array_push( array_complex, complex );
	}

	array_copy( copy_ptr, array_ptr );
	array_copy( copy_int, array_int );
	array_copy( copy_obj, array_obj );
	array_copy( copy_basic, array_basic );
	array_copy( copy_complex, array_complex );

	EXPECT_EQ( array_size( copy_ptr ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_int ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_obj ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_basic ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_complex ), array_size( array_ptr ) );

	for( i = 0; i < 255; ++i )
	{
		EXPECT_EQ( copy_ptr[i], array_ptr[i] );
		EXPECT_EQ( copy_int[i], array_int[i] );
		EXPECT_EQ( copy_obj[i], array_obj[i] );
		EXPECT_EQ( memcmp( copy_basic + i, array_basic + i, sizeof( *copy_basic ) ), 0 );
		EXPECT_EQ( memcmp( copy_complex + i, array_complex + i, sizeof( *copy_complex ) ), 0 );
	}

	array_copy( copy_ptr, array_ptr );
	array_copy( copy_int, array_int );
	array_copy( copy_obj, array_obj );
	array_copy( copy_basic, array_basic );
	array_copy( copy_complex, array_complex );

	EXPECT_EQ( array_size( copy_ptr ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_int ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_obj ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_basic ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_complex ), array_size( array_ptr ) );

	for( i = 0; i < 255; ++i )
	{
		EXPECT_EQ( copy_ptr[i], array_ptr[i] );
		EXPECT_EQ( copy_int[i], array_int[i] );
		EXPECT_EQ( copy_obj[i], array_obj[i] );
		EXPECT_EQ( memcmp( copy_basic + i, array_basic + i, sizeof( *copy_basic ) ), 0 );
		EXPECT_EQ( memcmp( copy_complex + i, array_complex + i, sizeof( *copy_complex ) ), 0 );
	}

	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i;
		complex.basicval.objval = i + 1;
		complex.intval = i + 2;
		complex.ptrval = 0;
		complex.charval = (char)i;
		complex.unionval.basicval.intval = i + 3;
		complex.unionval.basicval.objval = i + 4;

		array_push( copy_ptr, (void*)((uintptr_t)i) );
		array_push( copy_int, i );
		array_push( copy_obj, i );
		array_push( copy_basic, basic );
		array_push( copy_complex, complex );
	}

	array_erase( copy_ptr, 0 );
	array_erase( copy_int, 0 );
	array_erase( copy_obj, 0 );
	array_erase( copy_basic, 0 );
	array_erase( copy_complex, 0 );

	array_copy( copy_ptr, array_ptr );
	array_copy( copy_int, array_int );
	array_copy( copy_obj, array_obj );
	array_copy( copy_basic, array_basic );
	array_copy( copy_complex, array_complex );

	EXPECT_EQ( array_size( copy_ptr ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_int ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_obj ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_basic ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_complex ), array_size( array_ptr ) );

	for( i = 0; i < 255; ++i )
	{
		EXPECT_EQ( copy_ptr[i], array_ptr[i] );
		EXPECT_EQ( copy_int[i], array_int[i] );
		EXPECT_EQ( copy_obj[i], array_obj[i] );
		EXPECT_EQ( memcmp( copy_basic + i, array_basic + i, sizeof( *copy_basic ) ), 0 );
		EXPECT_EQ( memcmp( copy_complex + i, array_complex + i, sizeof( *copy_complex ) ), 0 );
	}

	array_clear( copy_ptr );
	array_clear( copy_int );
	array_clear( copy_obj );
	array_clear( copy_basic );
	array_clear( copy_complex );

	array_copy( copy_ptr, array_ptr );
	array_copy( copy_int, array_int );
	array_copy( copy_obj, array_obj );
	array_copy( copy_basic, array_basic );
	array_copy( copy_complex, array_complex );

	EXPECT_EQ( array_size( copy_ptr ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_int ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_obj ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_basic ), array_size( array_ptr ) );
	EXPECT_EQ( array_size( copy_complex ), array_size( array_ptr ) );

	for( i = 0; i < 255; ++i )
	{
		EXPECT_EQ( copy_ptr[i], array_ptr[i] );
		EXPECT_EQ( copy_int[i], array_int[i] );
		EXPECT_EQ( copy_obj[i], array_obj[i] );
		EXPECT_EQ( memcmp( copy_basic + i, array_basic + i, sizeof( *copy_basic ) ), 0 );
		EXPECT_EQ( memcmp( copy_complex + i, array_complex + i, sizeof( *copy_complex ) ), 0 );
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	array_deallocate( copy_ptr );
	array_deallocate( copy_int );
	array_deallocate( copy_obj );
	array_deallocate( copy_basic );
	array_deallocate( copy_complex );

	return 0;
}


DECLARE_TEST( array, pushpop )
{
	int        i, j;
	void**     array_ptr = 0;
	int*       array_int = 0;
	object_t*  array_obj = 0;
	basic_t*   array_basic = 0;
	complex_t* array_complex = 0;

	// Push to empty
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = 1;
		basic.objval = 2;

		complex.basicval.intval = 3;
		complex.basicval.objval = 4;
		complex.charval = 5;
		complex.intval = 6;
		complex.ptrval = 0;
		complex.unionval.realval = REAL_C(1.0);

		array_push( array_ptr, 0 );
		array_push( array_int, 0 );
		array_push( array_obj, 0 );
		array_push( array_basic, basic );
		array_push( array_complex, complex );

		EXPECT_EQ( array_size( array_ptr ), 1 );
		EXPECT_EQ( array_size( array_int ), 1 );
		EXPECT_EQ( array_size( array_obj ), 1 );
		EXPECT_EQ( array_size( array_basic ), 1 );
		EXPECT_EQ( array_size( array_complex ), 1 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );

		EXPECT_EQ( array_ptr[0], 0 );
		EXPECT_EQ( array_int[0], 0 );
		EXPECT_EQ( array_obj[0], 0 );
		EXPECT_EQ( memcmp( array_basic, &basic, sizeof( basic ) ), 0 );
		EXPECT_EQ( memcmp( array_complex, &complex, sizeof( complex ) ), 0 );
	}
	// Pop
	{
		array_pop( array_ptr );
		array_pop( array_int );
		array_pop( array_obj );
		array_pop( array_basic );
		array_pop( array_complex );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );

		array_pop_safe( array_ptr );
		array_pop_safe( array_int );
		array_pop_safe( array_obj );
		array_pop_safe( array_basic );
		array_pop_safe( array_complex );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );
	}
	// Push to non-empty
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i;
		complex.basicval.objval = i + 1;
		complex.intval = i + 2;
		complex.ptrval = 0;
		complex.charval = (char)i;
		complex.unionval.basicval.intval = i + 3;
		complex.unionval.basicval.objval = i + 4;

		if( !( i % 2 ) )
		{
			array_push( array_ptr, (void*)((uintptr_t)i) );
			array_push( array_int, i );
			array_push( array_obj, i );
			array_push( array_basic, basic );
			array_push( array_complex, complex );
		}
		else
		{
			void* iptr = (void*)((uintptr_t)i);
			object_t iobj = i;
			array_push_memcpy( array_ptr, &iptr );
			array_push_memcpy( array_int, &i );
			array_push_memcpy( array_obj, &iobj );
			array_push_memcpy( array_basic, &basic );
			array_push_memcpy( array_complex, &complex );
		}

		EXPECT_EQ( array_size( array_ptr ), i + 1 );
		EXPECT_EQ( array_size( array_int ), i + 1 );
		EXPECT_EQ( array_size( array_obj ), i + 1 );
		EXPECT_EQ( array_size( array_basic ), i + 1 );
		EXPECT_EQ( array_size( array_complex ), i + 1 );

		EXPECT_GE( array_capacity( array_ptr ), i + 1 );
		EXPECT_GE( array_capacity( array_int ), i + 1 );
		EXPECT_GE( array_capacity( array_obj ), i + 1 );
		EXPECT_GE( array_capacity( array_basic ), i + 1 );
		EXPECT_GE( array_capacity( array_complex ), i + 1 );

		for( j = 0; j <= i; ++j )
		{
			EXPECT_EQ( array_ptr[j], (void*)((uintptr_t)j) );
			EXPECT_EQ( array_int[j], j );
			EXPECT_EQ( array_obj[j], j );
			EXPECT_EQ( array_basic[j].intval, j );
			EXPECT_EQ( array_basic[j].objval, j + 1 );
			EXPECT_EQ( array_complex[j].basicval.intval, j );
			EXPECT_EQ( array_complex[j].basicval.objval, j + 1 );
			EXPECT_EQ( array_complex[j].intval, j + 2 );
			EXPECT_EQ( array_complex[j].charval, (char)j );
			EXPECT_EQ( array_complex[j].ptrval, 0 );
			EXPECT_EQ( array_complex[j].unionval.basicval.intval, j + 3 );
			EXPECT_EQ( array_complex[j].unionval.basicval.objval, j + 4 );
		}
	}
	// Pop while non-empty
	for( ; i > 0; --i )
	{
		array_pop( array_ptr );
		array_pop( array_int );
		array_pop( array_obj );
		array_pop( array_basic );
		array_pop( array_complex );

		EXPECT_EQ( array_size( array_ptr ), i - 1 );
		EXPECT_EQ( array_size( array_int ), i - 1 );
		EXPECT_EQ( array_size( array_obj ), i - 1 );
		EXPECT_EQ( array_size( array_basic ), i - 1 );
		EXPECT_EQ( array_size( array_complex ), i - 1 );

		EXPECT_GE( array_capacity( array_ptr ), i - 1 );
		EXPECT_GE( array_capacity( array_int ), i - 1 );
		EXPECT_GE( array_capacity( array_obj ), i - 1 );
		EXPECT_GE( array_capacity( array_basic ), i - 1 );
		EXPECT_GE( array_capacity( array_complex ), i - 1 );

		for( j = 0; j < i; ++j )
		{
			EXPECT_EQ( array_ptr[j], (void*)((uintptr_t)j) );
			EXPECT_EQ( array_int[j], j );
			EXPECT_EQ( array_obj[j], j );
			EXPECT_EQ( array_basic[j].intval, j );
			EXPECT_EQ( array_basic[j].objval, j + 1 );
			EXPECT_EQ( array_complex[j].basicval.intval, j );
			EXPECT_EQ( array_complex[j].basicval.objval, j + 1 );
			EXPECT_EQ( array_complex[j].intval, j + 2 );
			EXPECT_EQ( array_complex[j].charval, (char)j );
			EXPECT_EQ( array_complex[j].ptrval, 0 );
			EXPECT_EQ( array_complex[j].unionval.basicval.intval, j + 3 );
			EXPECT_EQ( array_complex[j].unionval.basicval.objval, j + 4 );
		}
	}
	// Interleaved push-pops
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		for( j = 0; j < 2; ++j )
		{
			basic.intval = i + j;
			basic.objval = i + j + 1;

			complex.basicval.intval = i + j;
			complex.basicval.objval = i + j + 1;
			complex.intval = i + j + 2;
			complex.ptrval = 0;
			complex.charval = (char)(i + j);
			complex.unionval.basicval.intval = i + j + 3;
			complex.unionval.basicval.objval = i + j + 4;

			if( !( i % 2 ) )
			{
				array_push( array_ptr, (void*)((uintptr_t)(i+j)) );
				array_push( array_int, i+j );
				array_push( array_obj, i+j );
				array_push( array_basic, basic );
				array_push( array_complex, complex );
			}
			else
			{
				void* iptr = (void*)((uintptr_t)(i+j));
				object_t iobj = i + j;
				int ii = i + j;
				array_push_memcpy( array_ptr, &iptr );
				array_push_memcpy( array_int, &ii );
				array_push_memcpy( array_obj, &iobj );
				array_push_memcpy( array_basic, &basic );
				array_push_memcpy( array_complex, &complex );
			}
		}

		array_pop( array_ptr );
		array_pop( array_int );
		array_pop( array_obj );
		array_pop( array_basic );
		array_pop( array_complex );

		EXPECT_EQ( array_size( array_ptr ), i + 1 );
		EXPECT_EQ( array_size( array_int ), i + 1 );
		EXPECT_EQ( array_size( array_obj ), i + 1 );
		EXPECT_EQ( array_size( array_basic ), i + 1 );
		EXPECT_EQ( array_size( array_complex ), i + 1 );

		EXPECT_GE( array_capacity( array_ptr ), i + 1 );
		EXPECT_GE( array_capacity( array_int ), i + 1 );
		EXPECT_GE( array_capacity( array_obj ), i + 1 );
		EXPECT_GE( array_capacity( array_basic ), i + 1 );
		EXPECT_GE( array_capacity( array_complex ), i + 1 );

		for( j = 0; j <= i; ++j )
		{
			EXPECT_EQ( array_ptr[j], (void*)((uintptr_t)j) );
			EXPECT_EQ( array_int[j], j );
			EXPECT_EQ( array_obj[j], j );
			EXPECT_EQ( array_basic[j].intval, j );
			EXPECT_EQ( array_basic[j].objval, j + 1 );
			EXPECT_EQ( array_complex[j].basicval.intval, j );
			EXPECT_EQ( array_complex[j].basicval.objval, j + 1 );
			EXPECT_EQ( array_complex[j].intval, j + 2 );
			EXPECT_EQ( array_complex[j].charval, (char)j );
			EXPECT_EQ( array_complex[j].ptrval, 0 );
			EXPECT_EQ( array_complex[j].unionval.basicval.intval, j + 3 );
			EXPECT_EQ( array_complex[j].unionval.basicval.objval, j + 4 );
		}
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	return 0;
}


DECLARE_TEST( array, inserterase )
{
	int        i, clamped_i, j, k;
	bool       found;

	void**     array_ptr = 0;
	int*       array_int = 0;
	object_t*  array_obj = 0;
	basic_t*   array_basic = 0;
	complex_t* array_complex = 0;

	void**     copy_ptr = 0;
	int*       copy_int = 0;
	object_t*  copy_obj = 0;
	basic_t*   copy_basic = 0;
	complex_t* copy_complex = 0;

	// Insert in empty
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = 1;
		basic.objval = 2;

		complex.basicval.intval = 3;
		complex.basicval.objval = 4;
		complex.charval = 5;
		complex.intval = 6;
		complex.ptrval = 0;
		complex.unionval.realval = REAL_C(1.0);

		array_insert( array_ptr, 0, 0 );
		array_insert( array_int, 0, 0 );
		array_insert( array_obj, 0, 0 );
		array_insert( array_basic, 0, basic );
		array_insert( array_complex, 0, complex );

		EXPECT_EQ( array_size( array_ptr ), 1 );
		EXPECT_EQ( array_size( array_int ), 1 );
		EXPECT_EQ( array_size( array_obj ), 1 );
		EXPECT_EQ( array_size( array_basic ), 1 );
		EXPECT_EQ( array_size( array_complex ), 1 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );

		EXPECT_EQ( array_ptr[0], 0 );
		EXPECT_EQ( array_int[0], 0 );
		EXPECT_EQ( array_obj[0], 0 );
		EXPECT_EQ( memcmp( array_basic, &basic, sizeof( basic ) ), 0 );
		EXPECT_EQ( memcmp( array_complex, &complex, sizeof( complex ) ), 0 );
	}
	// Erase
	{
		array_erase( array_ptr, 0 );
		array_erase( array_int, 0 );
		array_erase( array_obj, 0 );
		array_erase( array_basic, 0 );
		array_erase( array_complex, 0 );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );

		array_erase_safe( array_ptr, -1 );
		array_erase_safe( array_int, -1234 );
		array_erase_safe( array_obj, 0 );
		array_erase_safe( array_basic, 1 );
		array_erase_safe( array_complex, 1234 );

		EXPECT_EQ( array_size( array_ptr ), 0 );
		EXPECT_EQ( array_size( array_int ), 0 );
		EXPECT_EQ( array_size( array_obj ), 0 );
		EXPECT_EQ( array_size( array_basic ), 0 );
		EXPECT_EQ( array_size( array_complex ), 0 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );
	}
	// Insert safe in empty
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = 1;
		basic.objval = 2;

		complex.basicval.intval = 3;
		complex.basicval.objval = 4;
		complex.charval = 5;
		complex.intval = 6;
		complex.ptrval = 0;
		complex.unionval.realval = REAL_C(1.0);

		array_insert_safe( array_ptr, -1234, 0 );
		array_insert_safe( array_int, -1, 0 );
		array_insert_safe( array_obj, 0, 0 );
		array_insert_safe( array_basic, 1, basic );
		array_insert_safe( array_complex, 1234, complex );

		EXPECT_EQ( array_size( array_ptr ), 1 );
		EXPECT_EQ( array_size( array_int ), 1 );
		EXPECT_EQ( array_size( array_obj ), 1 );
		EXPECT_EQ( array_size( array_basic ), 1 );
		EXPECT_EQ( array_size( array_complex ), 1 );

		EXPECT_EQ( array_capacity( array_ptr ), 1 );
		EXPECT_EQ( array_capacity( array_int ), 1 );
		EXPECT_EQ( array_capacity( array_obj ), 1 );
		EXPECT_EQ( array_capacity( array_basic ), 1 );
		EXPECT_EQ( array_capacity( array_complex ), 1 );

		EXPECT_EQ( array_ptr[0], 0 );
		EXPECT_EQ( array_int[0], 0 );
		EXPECT_EQ( array_obj[0], 0 );
		EXPECT_EQ( memcmp( array_basic, &basic, sizeof( basic ) ), 0 );
		EXPECT_EQ( memcmp( array_complex, &complex, sizeof( complex ) ), 0 );
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	// Insert sequence
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert_safe( array_ptr, 129 - i, (void*)((uintptr_t)i) );
		array_insert_safe( array_int, 129 - i, i );
		array_insert_safe( array_obj, 129 - i, i );
		array_insert_safe( array_basic, 129 - i, basic );
		array_insert_safe( array_complex, 129 - i, complex );

		EXPECT_EQ( array_size( array_ptr ), i + 1 );
		EXPECT_EQ( array_size( array_int ), i + 1 );
		EXPECT_EQ( array_size( array_obj ), i + 1 );
		EXPECT_EQ( array_size( array_basic ), i + 1 );
		EXPECT_EQ( array_size( array_complex ), i + 1 );

		EXPECT_GE( array_capacity( array_ptr ), i + 1 );
		EXPECT_GE( array_capacity( array_int ), i + 1 );
		EXPECT_GE( array_capacity( array_obj ), i + 1 );
		EXPECT_GE( array_capacity( array_basic ), i + 1 );
		EXPECT_GE( array_capacity( array_complex ), i + 1 );

		EXPECT_EQ( array_ptr[clamped_i], (void*)((uintptr_t)i) );
		EXPECT_EQ( array_int[clamped_i], i );
		EXPECT_EQ( array_obj[clamped_i], i );
		EXPECT_EQ( memcmp( array_basic + clamped_i, &basic, sizeof( basic ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + clamped_i, &complex, sizeof( complex ) ), 0 );
	}

	array_clear( array_ptr );
	array_clear( array_int );
	array_clear( array_obj );
	array_clear( array_basic );
	array_clear( array_complex );

	// Insert sequence
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert( array_basic, clamped_i, basic );
		array_insert( array_complex, clamped_i, complex );

		EXPECT_EQ( array_size( array_ptr ), i + 1 );
		EXPECT_EQ( array_size( array_int ), i + 1 );
		EXPECT_EQ( array_size( array_obj ), i + 1 );
		EXPECT_EQ( array_size( array_basic ), i + 1 );
		EXPECT_EQ( array_size( array_complex ), i + 1 );

		EXPECT_GE( array_capacity( array_ptr ), i + 1 );
		EXPECT_GE( array_capacity( array_int ), i + 1 );
		EXPECT_GE( array_capacity( array_obj ), i + 1 );
		EXPECT_GE( array_capacity( array_basic ), i + 1 );
		EXPECT_GE( array_capacity( array_complex ), i + 1 );

		EXPECT_EQ( array_ptr[clamped_i], (void*)((uintptr_t)i) );
		EXPECT_EQ( array_int[clamped_i], i );
		EXPECT_EQ( array_obj[clamped_i], i );
		EXPECT_EQ( memcmp( array_basic + clamped_i, &basic, sizeof( basic ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + clamped_i, &complex, sizeof( complex ) ), 0 );
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	// Erase single items
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert( array_basic, clamped_i, basic );
		array_insert( array_complex, clamped_i, complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase( array_ptr, clamped_i );
		array_erase( array_int, clamped_i );
		array_erase( array_obj, clamped_i );
		array_erase( array_basic, clamped_i );
		array_erase( array_complex, clamped_i );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	//Erase single item, safe
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert( array_basic, clamped_i, basic );
		array_insert( array_complex, clamped_i, complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase_safe( array_ptr, clamped_i );
		array_erase_safe( array_int, clamped_i );
		array_erase_safe( array_obj, clamped_i );
		array_erase_safe( array_basic, clamped_i );
		array_erase_safe( array_complex, clamped_i );

		array_erase_safe( array_ptr, -1234 );
		array_erase_safe( array_int, -1 );
		array_erase_safe( array_obj, 1024 );
		array_erase_safe( array_basic, 12345 );
		array_erase_safe( array_complex, -12345 );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	// Erase single item, memcpy
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert_memcpy( array_basic, clamped_i, &basic );
		array_insert_memcpy( array_complex, clamped_i, &complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase_memcpy( array_ptr, clamped_i );
		array_erase_memcpy( array_int, clamped_i );
		array_erase_memcpy( array_obj, clamped_i );
		array_erase_memcpy( array_basic, clamped_i );
		array_erase_memcpy( array_complex, clamped_i );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	//Erase single item, memcpy safe
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert_memcpy( array_basic, clamped_i, &basic );
		array_insert_memcpy( array_complex, clamped_i, &complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase_memcpy_safe( array_ptr, clamped_i );
		array_erase_memcpy_safe( array_int, clamped_i );
		array_erase_memcpy_safe( array_obj, clamped_i );
		array_erase_memcpy_safe( array_basic, clamped_i );
		array_erase_memcpy_safe( array_complex, clamped_i );

		array_erase_memcpy_safe( array_ptr, -1234 );
		array_erase_memcpy_safe( array_int, -1 );
		array_erase_memcpy_safe( array_obj, 1024 );
		array_erase_memcpy_safe( array_basic, 12345 );
		array_erase_memcpy_safe( array_complex, -12345 );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	// Erase single item, ordered
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert_memcpy( array_basic, clamped_i, &basic );
		array_insert_memcpy( array_complex, clamped_i, &complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase_ordered( array_ptr, clamped_i );
		array_erase_ordered( array_int, clamped_i );
		array_erase_ordered( array_obj, clamped_i );
		array_erase_ordered( array_basic, clamped_i );
		array_erase_ordered( array_complex, clamped_i );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	//Erase single item, ordered safe
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert_memcpy( array_basic, clamped_i, &basic );
		array_insert_memcpy( array_complex, clamped_i, &complex );
	}
	for( i = 0; i < 255; ++i )
	{
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic = {0};
		complex_t complex = {0};

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		complex = array_complex[clamped_i];

		array_copy( copy_ptr, array_ptr );
		array_copy( copy_int, array_int );
		array_copy( copy_obj, array_obj );
		array_copy( copy_basic, array_basic );
		array_copy( copy_complex, array_complex );

		array_erase_ordered_safe( array_ptr, clamped_i );
		array_erase_ordered_safe( array_int, clamped_i );
		array_erase_ordered_safe( array_obj, clamped_i );
		array_erase_ordered_safe( array_basic, clamped_i );
		array_erase_ordered_safe( array_complex, clamped_i );

		array_erase_ordered_safe( array_ptr, -1234 );
		array_erase_ordered_safe( array_int, -1 );
		array_erase_ordered_safe( array_obj, 1024 );
		array_erase_ordered_safe( array_basic, 12345 );
		array_erase_ordered_safe( array_complex, -12345 );

		EXPECT_EQ( array_size( array_ptr ), 254 - i );
		EXPECT_EQ( array_size( array_int ), 254 - i );
		EXPECT_EQ( array_size( array_obj ), 254 - i );
		EXPECT_EQ( array_size( array_basic ), 254 - i );
		EXPECT_EQ( array_size( array_complex ), 254 - i );

		for( j = 0; j < 254 - i; ++j )
		{
			EXPECT_NE( array_ptr[j], ptrval );
			EXPECT_NE( array_int[j], intval );
			EXPECT_NE( array_obj[j], objval );
			EXPECT_NE( memcmp( array_basic + j, &basic, sizeof( basic ) ), 0 );
			EXPECT_NE( memcmp( array_complex + j, &complex, sizeof( complex ) ), 0 );
		}

		for( j = 0; j < array_size( copy_ptr ); ++j )
		{
			if( j == clamped_i )
				continue;

			found = false;
			for( k = 0; k < 254 - i; ++k )
			{
				if( array_ptr[k] == copy_ptr[j] )
				{
					EXPECT_EQ( array_ptr[k], copy_ptr[j] );
					EXPECT_EQ( array_int[k], copy_int[j] );
					EXPECT_EQ( array_obj[k], copy_obj[j] );
					EXPECT_EQ( memcmp( array_basic + k, copy_basic + j, sizeof( basic ) ), 0 );
					EXPECT_EQ( memcmp( array_complex + k, copy_complex + j, sizeof( complex ) ), 0 );
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate( copy_ptr );
		array_deallocate( copy_int );
		array_deallocate( copy_obj );
		array_deallocate( copy_basic );
		array_deallocate( copy_complex );
	}

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	//Range erase
	for( i = 0; i < 255; ++i )
	{
		basic_t basic = {0};
		complex_t complex = {0};

		basic.intval = i;
		basic.objval = i + 1;

		complex.basicval.intval = i + 2;
		complex.basicval.objval = i + 3;
		complex.charval = i + 4;
		complex.intval = i + 5;
		complex.ptrval = (void*)((uintptr_t)(i + 6));
		complex.unionval.basicval.intval = i + 7;
		complex.unionval.basicval.objval = i + 8;

		clamped_i = math_clamp( 129 - i, 0, array_size( array_ptr ) );

		array_insert( array_ptr, clamped_i, (void*)((uintptr_t)i) );
		array_insert( array_int, clamped_i, i );
		array_insert( array_obj, clamped_i, i );
		array_insert_memcpy( array_basic, clamped_i, &basic );
		array_insert_memcpy( array_complex, clamped_i, &complex );
	}

	array_copy( copy_ptr, array_ptr );
	array_copy( copy_int, array_int );
	array_copy( copy_obj, array_obj );
	array_copy( copy_basic, array_basic );
	array_copy( copy_complex, array_complex );

	//Erasing 0 is always safe
	array_erase_ordered_range( array_ptr, -1234, 0 );
	array_erase_ordered_range( array_int, -1, 0 );
	array_erase_ordered_range( array_obj, 0, 0 );
	array_erase_ordered_range( array_basic, 1, 0 );
	array_erase_ordered_range( array_complex, 1234, 0 );

	EXPECT_EQ( array_size( array_ptr ), 255 );
	EXPECT_EQ( array_size( array_int ), 255 );
	EXPECT_EQ( array_size( array_obj ), 255 );
	EXPECT_EQ( array_size( array_basic ), 255 );
	EXPECT_EQ( array_size( array_complex ), 255 );

	for( i = 0; i < 255; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i] );
		EXPECT_EQ( array_int[i], copy_int[i] );
		EXPECT_EQ( array_obj[i], copy_obj[i] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i, sizeof( complex_t ) ), 0 );
	}

	//Erase first
	array_erase_ordered_range( array_ptr, 0, 1 );
	array_erase_ordered_range( array_int, 0, 1 );
	array_erase_ordered_range( array_obj, 0, 1 );
	array_erase_ordered_range( array_basic, 0, 1 );
	array_erase_ordered_range( array_complex, 0, 1 );

	EXPECT_EQ( array_size( array_ptr ), 254 );
	EXPECT_EQ( array_size( array_int ), 254 );
	EXPECT_EQ( array_size( array_obj ), 254 );
	EXPECT_EQ( array_size( array_basic ), 254 );
	EXPECT_EQ( array_size( array_complex ), 254 );

	for( i = 0; i < 254; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+1] );
		EXPECT_EQ( array_int[i], copy_int[i+1] );
		EXPECT_EQ( array_obj[i], copy_obj[i+1] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 1, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 1, sizeof( complex_t ) ), 0 );
	}

	//Erase last
	array_erase_ordered_range( array_ptr, 253, 1 );
	array_erase_ordered_range( array_int, 253, 1 );
	array_erase_ordered_range( array_obj, 253, 1 );
	array_erase_ordered_range( array_basic, 253, 1 );
	array_erase_ordered_range( array_complex, 253, 1 );

	EXPECT_EQ( array_size( array_ptr ), 253 );
	EXPECT_EQ( array_size( array_int ), 253 );
	EXPECT_EQ( array_size( array_obj ), 253 );
	EXPECT_EQ( array_size( array_basic ), 253 );
	EXPECT_EQ( array_size( array_complex ), 253 );

	for( i = 0; i < 253; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+1] );
		EXPECT_EQ( array_int[i], copy_int[i+1] );
		EXPECT_EQ( array_obj[i], copy_obj[i+1] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 1, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 1, sizeof( complex_t ) ), 0 );
	}

	//Erase range
	array_erase_ordered_range( array_ptr, 37, 63 );
	array_erase_ordered_range( array_int, 37, 63 );
	array_erase_ordered_range( array_obj, 37, 63 );
	array_erase_ordered_range( array_basic, 37, 63 );
	array_erase_ordered_range( array_complex, 37, 63 );

	EXPECT_EQ( array_size( array_ptr ), 190 );
	EXPECT_EQ( array_size( array_int ), 190 );
	EXPECT_EQ( array_size( array_obj ), 190 );
	EXPECT_EQ( array_size( array_basic ), 190 );
	EXPECT_EQ( array_size( array_complex ), 190 );

	for( i = 0; i < 37; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+1] );
		EXPECT_EQ( array_int[i], copy_int[i+1] );
		EXPECT_EQ( array_obj[i], copy_obj[i+1] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 1, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 1, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 190; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+64] );
		EXPECT_EQ( array_int[i], copy_int[i+64] );
		EXPECT_EQ( array_obj[i], copy_obj[i+64] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 64, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 64, sizeof( complex_t ) ), 0 );
	}

	//Erase range safe
	array_erase_ordered_range_safe( array_ptr, -1234, -1234 );
	array_erase_ordered_range_safe( array_int, -1234, 1234 );
	array_erase_ordered_range_safe( array_obj, 10, -10 );
	array_erase_ordered_range_safe( array_basic, 1234, -123 );
	array_erase_ordered_range_safe( array_complex, 1234, 1234 );

	EXPECT_EQ( array_size( array_ptr ), 190 );
	EXPECT_EQ( array_size( array_int ), 190 );
	EXPECT_EQ( array_size( array_obj ), 190 );
	EXPECT_EQ( array_size( array_basic ), 190 );
	EXPECT_EQ( array_size( array_complex ), 190 );

	for( i = 0; i < 37; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+1] );
		EXPECT_EQ( array_int[i], copy_int[i+1] );
		EXPECT_EQ( array_obj[i], copy_obj[i+1] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 1, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 1, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 190; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+64] );
		EXPECT_EQ( array_int[i], copy_int[i+64] );
		EXPECT_EQ( array_obj[i], copy_obj[i+64] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 64, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 64, sizeof( complex_t ) ), 0 );
	}

	//Erase range safe overlap start
	array_erase_ordered_range_safe( array_ptr, -1234, 1236 );
	array_erase_ordered_range_safe( array_int, -134, 136 );
	array_erase_ordered_range_safe( array_obj, -1, 3 );
	array_erase_ordered_range_safe( array_basic, 0, 2 );
	array_erase_ordered_range_safe( array_complex, -234, 236 );

	EXPECT_EQ( array_size( array_ptr ), 188 );
	EXPECT_EQ( array_size( array_int ), 188 );
	EXPECT_EQ( array_size( array_obj ), 188 );
	EXPECT_EQ( array_size( array_basic ), 188 );
	EXPECT_EQ( array_size( array_complex ), 188 );

	for( i = 0; i < 35; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+3] );
		EXPECT_EQ( array_int[i], copy_int[i+3] );
		EXPECT_EQ( array_obj[i], copy_obj[i+3] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 3, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 3, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 188; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+66] );
		EXPECT_EQ( array_int[i], copy_int[i+66] );
		EXPECT_EQ( array_obj[i], copy_obj[i+66] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 66, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 66, sizeof( complex_t ) ), 0 );
	}

	//Erase range safe overlap end
	array_erase_ordered_range_safe( array_ptr, 180, 1236 );
	array_erase_ordered_range_safe( array_int, 180, 136 );
	array_erase_ordered_range_safe( array_obj, 180, 8 );
	array_erase_ordered_range_safe( array_basic, 180, 8 );
	array_erase_ordered_range_safe( array_complex, 180, 180 );

	EXPECT_EQ( array_size( array_ptr ), 180 );
	EXPECT_EQ( array_size( array_int ), 180 );
	EXPECT_EQ( array_size( array_obj ), 180 );
	EXPECT_EQ( array_size( array_basic ), 180 );
	EXPECT_EQ( array_size( array_complex ), 180 );

	for( i = 0; i < 35; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+3] );
		EXPECT_EQ( array_int[i], copy_int[i+3] );
		EXPECT_EQ( array_obj[i], copy_obj[i+3] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 3, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 3, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 180; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+66] );
		EXPECT_EQ( array_int[i], copy_int[i+66] );
		EXPECT_EQ( array_obj[i], copy_obj[i+66] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 66, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 66, sizeof( complex_t ) ), 0 );
	}

	//Erase range safe
	array_erase_ordered_range_safe( array_ptr, 100, 10 );
	array_erase_ordered_range_safe( array_int, 100, 10 );
	array_erase_ordered_range_safe( array_obj, 100, 10 );
	array_erase_ordered_range_safe( array_basic, 100, 10 );
	array_erase_ordered_range_safe( array_complex, 100, 10 );

	EXPECT_EQ( array_size( array_ptr ), 170 );
	EXPECT_EQ( array_size( array_int ), 170 );
	EXPECT_EQ( array_size( array_obj ), 170 );
	EXPECT_EQ( array_size( array_basic ), 170 );
	EXPECT_EQ( array_size( array_complex ), 170 );

	for( i = 0; i < 35; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+3] );
		EXPECT_EQ( array_int[i], copy_int[i+3] );
		EXPECT_EQ( array_obj[i], copy_obj[i+3] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 3, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 3, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 100; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+66] );
		EXPECT_EQ( array_int[i], copy_int[i+66] );
		EXPECT_EQ( array_obj[i], copy_obj[i+66] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 66, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 66, sizeof( complex_t ) ), 0 );
	}

	for( ; i < 170; ++i )
	{
		EXPECT_EQ( array_ptr[i], copy_ptr[i+76] );
		EXPECT_EQ( array_int[i], copy_int[i+76] );
		EXPECT_EQ( array_obj[i], copy_obj[i+76] );
		EXPECT_EQ( memcmp( array_basic + i, copy_basic + i + 76, sizeof( basic_t ) ), 0 );
		EXPECT_EQ( memcmp( array_complex + i, copy_complex + i + 76, sizeof( complex_t ) ), 0 );
	}

	array_deallocate( copy_ptr );
	array_deallocate( copy_int );
	array_deallocate( copy_obj );
	array_deallocate( copy_basic );
	array_deallocate( copy_complex );

	array_deallocate( array_ptr );
	array_deallocate( array_int );
	array_deallocate( array_obj );
	array_deallocate( array_basic );
	array_deallocate( array_complex );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( array, allocation );
	ADD_TEST( array, copy );
	ADD_TEST( array, pushpop );
	ADD_TEST( array, inserterase );
}
