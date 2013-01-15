/* error.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation.h>


DECLARE_THREAD_LOCAL( error_t, error, ERROR_NONE )
DECLARE_THREAD_LOCAL( error_callback_fn, error_callback, 0 )


error_t error( void )
{
	error_t err = get_thread_error();
	set_thread_error( ERROR_NONE );
	return err;
}


int error_report( error_level_t level, error_t err )
{
	int result = 0;
	error_callback_fn callback = get_thread_error_callback();

	set_thread_error( err );
	if( callback )
		result = callback( level, err );

	return result;
}


void error_set_callback( error_callback_fn callback )
{
	set_thread_error_callback( callback );
}


#if !BUILD_DEPLOY || BUILD_ENABLE_DEPLOY_ERROR

#include <stdio.h>

#if PLATFORM_WINDOWS
#  define snprintf( p, s, ... ) _snprintf_s( p, s, _TRUNCATE, __VA_ARGS__ )
#endif

#define ERROR_CONTEXT_MAX_DEPTH  32

typedef struct _error_frame
{
	const char*        name;
	const char*        data;
} error_frame_t;

typedef struct _error_context
{
	error_frame_t      frame[ERROR_CONTEXT_MAX_DEPTH];
	int                depth;
} error_context_t;

DECLARE_THREAD_LOCAL( error_context_t*, error_context, 0 )


void _error_context_push( const char* name, const char* data )
{
	error_context_t* context = get_thread_error_context();
	if( !context )
	{
		context = memory_allocate_zero( sizeof( error_context_t ), 0, MEMORY_PERSISTENT );
		set_thread_error_context( context );
	}
	FOUNDATION_ASSERT_MSG( context->depth < ERROR_CONTEXT_MAX_DEPTH, "Error context stack overflow" );
	context->frame[ context->depth ].name = name;
	context->frame[ context->depth ].data = data;
	++context->depth;
}


void _error_context_pop( void )
{
	error_context_t* context = get_thread_error_context();
	if( context )
	{
		FOUNDATION_ASSERT_MSG( context->depth, "Error context stack underflow" );
		--context->depth;
	}
}


void _error_context_buffer( char* buffer, unsigned int size )
{
	error_context_t* context = get_thread_error_context();
	if( context )
	{
		int i, written;
		error_frame_t* frame = context->frame;
		for( i = 0; size && ( i < context->depth ); ++i, ++frame )
		{
			written = snprintf( buffer, size, "When %s: %s\n", frame->name ? frame->name : "<something>", frame->data ? frame->data : "" );
			if( ( written > 0 ) && ( written <= (int)size ) )
			{
				buffer += written;
				size -= written;
			}
			else
				break;
		}
	}
}


void _error_context_thread_deallocate( void )
{
	error_context_t* context = get_thread_error_context();
	if( context )
		memory_deallocate( context );
	set_thread_error_context( 0 );
}

#endif
