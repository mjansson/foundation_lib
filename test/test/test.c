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


FOUNDATION_EXTERN void test_declare( void );
FOUNDATION_EXTERN application_t test_application( void );
FOUNDATION_EXTERN int test_initialize( void );
FOUNDATION_EXTERN void test_shutdown( void );


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

static bool _test_failed = false;


void* event_thread( object_t thread, void* arg )
{
	event_block_t* block;
	event_t* event = 0;

	while( !thread_should_terminate( thread ) )
	{
		block = event_stream_process( system_event_stream() );
		while( ( event = event_next( block, 0 ) ) )
		{
			if( event->system == SYSTEM_FOUNDATION ) switch( event->id )
			{
				case FOUNDATIONEVENT_TERMINATE:
					log_warnf( WARNING_SUSPICIOUS, "Terminating test due to event" );
					process_exit( -2 );
					break;

				default:
					break;
			}
		}
		thread_sleep( 10 );
	}

	return 0;
}


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
	void* result;
	object_t thread_event;

	log_infof( "Running test suite: %s", environment_application()->short_name );

	_test_failed = false;
	
	thread_event = thread_create( event_thread, "event_thread", THREAD_PRIORITY_NORMAL, 0 );
	thread_start( thread_event, 0 );

	while( !thread_is_running( thread_event ) )
		thread_yield();
	
	for( ig = 0, gsize = array_size( _test_groups ); ig < gsize; ++ig )
	{
		log_infof( "Running tests from group %s", _test_groups[ig]->name );
		for( ic = 0, csize = array_size( _test_groups[ig]->cases ); ic < csize; ++ic )
		{
			log_infof( "  Running %s tests", _test_groups[ig]->cases[ic]->name );
			result = _test_groups[ig]->cases[ic]->fn();
			if( result != 0 )
			{
				log_warnf( WARNING_SUSPICIOUS, "    FAILED" );
				_test_failed = true;
			}
			else
			{
				log_infof( "    PASSED" );
			}
		}
	}
	
	thread_terminate( thread_event );
	thread_destroy( thread_event );
	while( thread_is_running( thread_event ) || thread_is_thread( thread_event ) )
		thread_yield();
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
	log_suppress( ERRORLEVEL_DEBUG );
	
	return foundation_initialize( memory_system_malloc(), test_application() );
}


int main_run( void* main_arg )
{
	if( test_initialize() < 0 )
		return -1;
	test_declare();
	test_run();
	test_free();
	test_shutdown();
	if( _test_failed )
	{
		process_set_exit_code( -1 );		
		return -1;
	}
	return 0;
}


void main_shutdown( void )
{
	foundation_shutdown();
}


void test_wait_for_threads_startup( const object_t* threads, unsigned int num_threads )
{
	unsigned int i;
	bool waiting = true;

	while( waiting )
	{
		waiting = false;
		for( i = 0; i < num_threads; ++i )
		{
			if( !thread_is_started( threads[i] ) )
			{
				waiting = true;
				break;
			}
		}
	}
}


void test_wait_for_threads_finish( const object_t* threads, unsigned int num_threads )
{
	unsigned int i;
	bool waiting = true;

	while( waiting )
	{
		waiting = false;
		for( i = 0; i < num_threads; ++i )
		{
			if( thread_is_running( threads[i] ) )
			{
				waiting = true;
				break;
			}
		}
	}
}


void test_wait_for_threads_exit( const object_t* threads, unsigned int num_threads )
{
	unsigned int i;
	bool keep_waiting;
	do
	{
		keep_waiting = false;
		for( i = 0; i < num_threads; ++i )
		{
			if( thread_is_thread( threads[i] ) )
			{
				keep_waiting = true;
				break;
			}
		}
		if( keep_waiting )
			thread_sleep( 10 );
	} while( keep_waiting );
}

