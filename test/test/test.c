/* test.c  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#define TEST_COMPILE 1
#include <test/test.h>


extern void test_declare( void );
extern application_t test_application( void );


typedef struct
{
	const char*       name;
	test_fn           fn;
} test_case_t;

typedef struct
{
	const char*       name;
	test_case_t**     cases;
} test_group_t;

test_group_t** _test_groups = 0;


void test_add_test( test_fn fn, const char* group_name, const char* test_name )
{
	unsigned int ig, gsize;
	test_group_t* test_group = 0;
	test_case_t* test_case = 0;
	for( ig = 0, gsize = array_size( _test_groups ); ig < gsize; ++ig )
	{
		if( string_equal( _test_groups[ig]->name, group_name ) )
		{
			test_group = _test_groups[ig];
			break;
		}
	}

	if( !test_group )
	{
		test_group = memory_allocate_zero( sizeof( test_group_t ), 0, MEMORY_PERSISTENT );
		test_group->name = group_name;
		array_push( _test_groups, test_group );
	}

	test_case = memory_allocate_zero( sizeof( test_case_t ), 0, MEMORY_PERSISTENT );
	test_case->name = test_name;
	test_case->fn = fn;

	array_push( test_group->cases, test_case );
}


void test_run( void )
{
	unsigned int ig, gsize, ic, csize;
	int result;

	log_suppress( ERRORLEVEL_DEBUG );
	log_infof( "Running test suite: %s", environment_application()->short_name );

	for( ig = 0, gsize = array_size( _test_groups ); ig < gsize; ++ig )
	{
		log_infof( "Running tests from group %s", _test_groups[ig]->name );
		for( ic = 0, csize = array_size( _test_groups[ig]->cases ); ic < csize; ++ic )
		{
			log_infof( "  Running %s tests", _test_groups[ig]->cases[ic]->name );
			result = _test_groups[ig]->cases[ic]->fn();
			if( result < 0 )
				log_warnf( WARNING_SUSPICIOUS, "    FAILED" );
			else
				log_infof( "    PASSED" );
		}
	}	
}


void test_free( void )
{
	unsigned int ig, gsize, ic, csize;
	for( ig = 0, gsize = array_size( _test_groups ); ig < gsize; ++ig )
	{
		for( ic = 0, csize = array_size( _test_groups[ig]->cases ); ic < csize; ++ic )
		{
			memory_deallocate( _test_groups[ig]->cases[ic] );
		}
		array_deallocate( _test_groups[ig]->cases );
		memory_deallocate( _test_groups[ig] );
	}
	array_deallocate( _test_groups );
	_test_groups = 0;
}


int main_initialize( void )
{
	return foundation_initialize( memory_system_malloc(), test_application() );
}


int main_run( void* main_arg )
{
	test_declare();
	test_run();
	test_free();
	return 0;
}


void main_shutdown( void )
{
	foundation_shutdown();
}

