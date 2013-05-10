/* main.c  -  Foundation atomic test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation atomic tests";
	app.short_name = "test_atomic";
	app.config_dir = "test_atomic";
	app.flags = APPLICATION_UTILITY;
	return app;
}


volatile int32_t val_32  = 0;
volatile int64_t val_64  = 0;
volatile void*   val_ptr = 0;


void* inc_thread( object_t thread, void* arg )
{
	int loop = 0;
	int icount = 0;
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 256; ++icount )
		{
			atomic_incr32( &val_32 );
			atomic_incr64( &val_64 );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


void* dec_thread( object_t thread, void* arg )
{
	int loop = 0;
	int icount = 0;
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 256; ++icount )
		{
			atomic_decr32( &val_32 );
			atomic_decr64( &val_64 );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


void* add_thread( object_t thread, void* arg )
{
	int loop = 0;
	int32_t icount = 0;
	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		for( icount = 0; icount < 128; ++icount )
		{
			atomic_add32( &val_32, icount % 2 ? -icount : icount );
			atomic_exchange_and_add64( &val_64, icount % 2 ? -icount : icount );
		}
		for( icount = 0; icount < 128; ++icount )
		{
			atomic_exchange_and_add32( &val_32, icount % 2 ? icount : -icount );
			atomic_add64( &val_64, icount % 2 ? icount : -icount );
		}

		++loop;
		thread_yield();
	}
	return 0;
}


typedef struct
{
	int32_t val_32;
	int64_t val_64;
	void*   val_ptr;
} cas_value_t;

void* cas_thread( object_t thread, void* arg )
{
	int loop = 0;
	cas_value_t val = *(cas_value_t*)arg;

	thread_sleep( 10 );

	while( !thread_should_terminate( thread ) && ( loop < 65535 ) )
	{
		while( !atomic_cas32( &val_32, val.val_32, 0 ) )
			thread_yield();
		while( !atomic_cas32( &val_32, 0, val.val_32 ) )
			thread_yield();
		while( !atomic_cas64( &val_64, val.val_64, 0 ) )
			thread_yield();
		while( !atomic_cas64( &val_64, 0, val.val_64 ) )
			thread_yield();
		while( !atomic_cas_ptr( &val_ptr, val.val_ptr, 0 ) )
			thread_yield();
		while( !atomic_cas_ptr( &val_ptr, 0, val.val_ptr ) )
			thread_yield();

		++loop;
		thread_yield();
	}
	return 0;
}


DECLARE_TEST( atomic, incdec )
{
	int num_threads = 32;
	int ithread;
	object_t threads[32];
	bool thread_running[32];
	
	for( ithread = 0; ithread < num_threads; ++ithread )
	{
		threads[ithread] = thread_create( ithread % 2 ? dec_thread : inc_thread, ithread % 2 ? "dec" : "inc", THREAD_PRIORITY_NORMAL, 0 );
		thread_running[ithread] = false;
	}
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], 0 );
	
	while( true )
	{
		bool not_started = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( !thread_running[ithread] )
			{
				if( thread_is_running( threads[ithread] ) )
					thread_running[ithread] = true;
				else
					not_started = true;
			}
		}
		if( !not_started )
			break;
		thread_sleep( 1 );
	}
	
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] );
	while( true )
	{
		bool has_thread = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( thread_is_thread( threads[ithread] ) )
			{
				has_thread = true;
				break;
			}
		}
		if( !has_thread )
			break;
		thread_sleep( 10 );
	}
	thread_sleep( 100 );

	EXPECT_EQ( val_32, 0 );
	EXPECT_EQ( val_64, 0 );

	return 0;
}


DECLARE_TEST( atomic, add )
{
	int num_threads = 32;
	int ithread;
	object_t threads[32];
	bool thread_running[32];
	
	for( ithread = 0; ithread < num_threads; ++ithread )
	{
		threads[ithread] = thread_create( add_thread, "add", THREAD_PRIORITY_NORMAL, 0 );
		thread_running[ithread] = false;
	}
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], 0 );
	
	while( true )
	{
		bool not_started = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( !thread_running[ithread] )
			{
				if( thread_is_running( threads[ithread] ) )
					thread_running[ithread] = true;
				else
					not_started = true;
			}
		}
		if( !not_started )
			break;
		thread_sleep( 1 );
	}
	
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] );
	while( true )
	{
		bool has_thread = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( thread_is_thread( threads[ithread] ) )
			{
				has_thread = true;
				break;
			}
		}
		if( !has_thread )
			break;
		thread_sleep( 10 );
	}
	thread_sleep( 100 );

	EXPECT_EQ( val_32, 0 );
	EXPECT_EQ( val_64, 0 );

	return 0;
}


DECLARE_TEST( atomic, cas )
{
	int num_threads = 32;
	int ithread;
	object_t threads[32];
	cas_value_t cas_values[32];
	bool thread_running[32];
	
	for( ithread = 0; ithread < num_threads; ++ithread )
	{
		threads[ithread] = thread_create( cas_thread, "cas", THREAD_PRIORITY_NORMAL, 0 );
		thread_running[ithread] = false;
		cas_values[ithread].val_32 = ithread;
		cas_values[ithread].val_64 = ithread;
		cas_values[ithread].val_ptr = (void*)(uintptr_t)ithread;
	}
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_start( threads[ithread], &cas_values[ithread] );
	
	while( true )
	{
		bool not_started = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( !thread_running[ithread] )
			{
				if( thread_is_running( threads[ithread] ) )
					thread_running[ithread] = true;
				else
					not_started = true;
			}
		}
		if( !not_started )
			break;
		thread_sleep( 1 );
	}
	
	for( ithread = 0; ithread < num_threads; ++ithread )
		thread_destroy( threads[ithread] );
	while( true )
	{
		bool has_thread = false;
		for( ithread = 0; ithread < num_threads; ++ithread )
		{
			if( thread_is_thread( threads[ithread] ) )
			{
				has_thread = true;
				break;
			}
		}
		if( !has_thread )
			break;
		thread_sleep( 10 );
	}
	thread_sleep( 100 );

	EXPECT_EQ( val_32, 0 );
	EXPECT_EQ( val_64, 0 );
	EXPECT_EQ( val_ptr, 0 );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( atomic, incdec );
	ADD_TEST( atomic, add );
	ADD_TEST( atomic, cas );
}
