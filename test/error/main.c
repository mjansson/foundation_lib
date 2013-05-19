/* main.c  -  Foundation error test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation error tests";
	app.short_name = "test_error";
	app.config_dir = "test_error";
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


DECLARE_TEST( error, error )
{
	EXPECT_EQ( error(), ERROR_NONE );
	EXPECT_EQ( error(), ERROR_NONE );

	error_report( ERRORLEVEL_WARNING, ERROR_ACCESS_DENIED );
	EXPECT_EQ( error(), ERROR_ACCESS_DENIED );
	EXPECT_EQ( error(), ERROR_NONE );

	error_report( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE );
	EXPECT_EQ( error(), ERROR_INVALID_VALUE );
	EXPECT_EQ( error(), ERROR_NONE );

	return 0;
}


DECLARE_TEST( error, context )
{
	error_context_t* context = 0;

	context = error_context();
	if( context )
		EXPECT_EQ( context->depth, 0 );

	error_context_push( "error test", "data" );
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 1 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 0 );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_push( "error test", "data" );
	error_context_push( "another test", "more data" );
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 2 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
	EXPECT_STREQ( context->frame[1].name, "another test" );
	EXPECT_STREQ( context->frame[1].data, "more data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 1 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();

	return 0;
}


void* error_test_thread( void )
{
	error_context_t* context = 0;

	EXPECT_EQ( error(), ERROR_NONE );
	EXPECT_EQ( error(), ERROR_NONE );

	error_report( ERRORLEVEL_WARNING, ERROR_ACCESS_DENIED );
	EXPECT_EQ( error(), ERROR_ACCESS_DENIED );
	EXPECT_EQ( error(), ERROR_NONE );

	error_report( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE );
	EXPECT_EQ( error(), ERROR_INVALID_VALUE );
	EXPECT_EQ( error(), ERROR_NONE );

	context = error_context();
	if( context )
		EXPECT_EQ( context->depth, 0 );

	error_context_push( "error test", "data" );
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 1 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 0 );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_push( "error test", "data" );
	error_context_push( "another test", "more data" );
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 2 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
	EXPECT_STREQ( context->frame[1].name, "another test" );
	EXPECT_STREQ( context->frame[1].data, "more data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();
	context = error_context();

#if BUILD_ENABLE_ERROR_CONTEXT
	EXPECT_NE( context, 0 );
	EXPECT_EQ( context->depth, 1 );
	EXPECT_STREQ( context->frame[0].name, "error test" );
	EXPECT_STREQ( context->frame[0].data, "data" );
#else
	EXPECT_EQ( context, 0 );
#endif

	error_context_pop();

	return 0;
}


void* error_thread( object_t thread, void* arg )
{
	int ipass = 0;

	thread_sleep( 10 );

	for( ipass = 0; ipass < 512; ++ipass )
	{
		if( error_test_thread() )
			return FAILED_TEST;
		thread_yield();
	}

	return 0;
}


DECLARE_TEST( error, thread )
{
	//Launch 32 threads
	object_t thread[32];
	int i;
	bool running = true;

	for( i = 0; i < 32; ++i )
	{
		thread[i] = thread_create( error_thread, "error", THREAD_PRIORITY_NORMAL, 0 );
		thread_start( thread[i], 0 );
	}

	test_wait_for_threads_startup( thread, 32 );

	test_wait_for_threads_finish( thread, 32 );

	for( i = 0; i < 32; ++i )
	{
		EXPECT_EQ( thread_result( thread[i] ), 0 );
		thread_destroy( thread[i] );
	}

	test_wait_for_threads_exit( thread, 32 );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( error, error );
	ADD_TEST( error, context );
	ADD_TEST( error, thread );
}
