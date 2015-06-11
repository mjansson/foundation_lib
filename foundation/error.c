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

#include <foundation/foundation.h>
#include <foundation/internal.h>


FOUNDATION_DECLARE_THREAD_LOCAL( error_t, error, 0 ) // 0 = ERROR_NONE
FOUNDATION_DECLARE_THREAD_LOCAL( error_callback_fn, error_callback, 0 )


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


error_callback_fn error_callback( void )
{
	return get_thread_error_callback();
}


void error_set_callback( error_callback_fn callback )
{
	set_thread_error_callback( callback );
}


#if BUILD_ENABLE_ERROR_CONTEXT

#include <stdio.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  define snprintf( p, s, ... ) _snprintf_s( p, s, _TRUNCATE, __VA_ARGS__ )
#endif


FOUNDATION_DECLARE_THREAD_LOCAL( error_context_t*, error_context, 0 )


void _error_context_push( const char* name, size_t name_length, const char* data, size_t data_length )
{
	error_context_t* context = get_thread_error_context();
	if( !context )
	{
		context = memory_allocate( 0, sizeof( error_context_t ) + ( sizeof( error_frame_t ) * _foundation_def.error_context_depth ), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );
		set_thread_error_context( context );
	}
	context->frame[ context->depth ].name.str = name ? name : "<something>";
	context->frame[ context->depth ].name.length = name ? name_length : 11;
	context->frame[ context->depth ].data.str = data ? data : "<nothing>";
	context->frame[ context->depth ].data.length = data ? data_length : 9;
	if( context->depth < _foundation_def.error_context_depth - 1 )
		++context->depth;
}


void _error_context_pop( void )
{
	error_context_t* context = get_thread_error_context();
	if( context && context->depth > 0 )
		--context->depth;
}


void _error_context_clear( void )
{
	error_context_t* context = get_thread_error_context();
	if( context )
		context->depth = 0;
}


string_t _error_context_buffer( char* buffer, size_t size )
{
	error_context_t* context = get_thread_error_context();
	string_t result = { buffer, size };
	if( context )
	{
		size_t i;
		string_t line;
		error_frame_t* frame = context->frame;
		for( i = 0; ( size > 1 ) && ( i < context->depth ); ++i, ++frame )
		{
			if( i )
			{
				*buffer++ = '\n';
				--size;
			}

			line = string_format( buffer, size, STRING_CONST( "When %*s: %*s" ), (int)frame->name.length, frame->name.str, (int)frame->data.length, frame->data.str );

			buffer += line.length;
			size -= line.length;
		}
	}
	if( size )
		*buffer = 0;
	result.length = (size_t)pointer_diff( buffer, result.str );
	return result;
}


error_context_t* _error_context( void )
{
	return get_thread_error_context();
}


void _error_context_thread_deallocate( void )
{
	error_context_t* context = get_thread_error_context();
	if( context )
	{
		FOUNDATION_ASSERT_MSG( !context->depth, "Error context thread exit with non-zero context stack" );
		memory_deallocate( context );
		set_thread_error_context( 0 );
	}
}

#endif
