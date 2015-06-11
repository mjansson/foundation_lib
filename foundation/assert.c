/* assert.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <stdio.h>
#include <stdarg.h>


#define ASSERT_BUFFER_SIZE 1024

static assert_handler_fn   _assert_handler;
static char                _assert_buffer[ASSERT_BUFFER_SIZE];

#if BUILD_ENABLE_ASSERT
#define ASSERT_STACKTRACE_MAX_DEPTH    128U
#define ASSERT_STACKTRACE_SKIP_FRAMES  1U
static void*               _assert_stacktrace[ASSERT_STACKTRACE_MAX_DEPTH];
static char                _assert_context_buffer[ASSERT_BUFFER_SIZE];
static char                _assert_stacktrace_buffer[ASSERT_BUFFER_SIZE];
static char                _assert_message_buffer[ASSERT_BUFFER_SIZE];
#endif


assert_handler_fn assert_handler( void )
{
	return _assert_handler;
}


void assert_set_handler( assert_handler_fn new_handler )
{
	_assert_handler = new_handler;
}


int assert_report( hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length, unsigned int line, const char* msg, size_t msg_length )
{
	static const char nocondition[] = "<Static fail>";
	static const char nofile[] = "<No file>";
	static const char nomsg[] = "<No message>";
	static const char assert_format[] = "****** ASSERT FAILED ******\nCondition: %*s\nFile/line: %*s : %d\n%*s%*s\n%*s\n";
#if BUILD_ENABLE_ASSERT
	string_t tracestr = { _assert_stacktrace_buffer, sizeof( _assert_stacktrace_buffer ) };
	string_t contextstr = { _assert_context_buffer, sizeof( _assert_context_buffer ) };
	string_t messagestr = { _assert_message_buffer, sizeof( _assert_message_buffer ) };
#endif

	if( !condition  ) { condition = nocondition; cond_length = sizeof( nocondition ); }
	if( !file ) { file = nofile; file_length = sizeof( nofile ); }
	if( !msg ) { msg = nomsg; msg_length = sizeof( nomsg ); }

	if( _assert_handler && ( _assert_handler != assert_report ) )
		return (*_assert_handler)( context, condition, cond_length, file, file_length, line, msg, msg_length );

#if BUILD_ENABLE_ASSERT
	contextstr = error_context_buffer( contextstr.str, contextstr.length );

	if( foundation_is_initialized() )
	{
		size_t num_frames = stacktrace_capture( _assert_stacktrace, ASSERT_STACKTRACE_MAX_DEPTH, ASSERT_STACKTRACE_SKIP_FRAMES );
		if( num_frames )
			tracestr = stacktrace_resolve( STRING_ARGS( tracestr ), _assert_stacktrace, num_frames, 0U );
		else
			tracestr = string_copy( STRING_ARGS( tracestr ), STRING_CONST( "<no stacktrace>" ) );
	}
	else
	{
		tracestr = string_copy( STRING_ARGS( tracestr ), STRING_CONST( "<no stacktrace - not initialized>" ) );
	}

	messagestr = string_format( STRING_ARGS( messagestr ), assert_format, sizeof( assert_format ) - 1,
		(int)cond_length, condition, (int)file_length, file, line,
		STRING_FORMAT( contextstr ), (int)msg_length, msg,
		STRING_FORMAT( tracestr ) );

	log_errorf( context, ERROR_ASSERT, STRING_CONST( "%*s" ), STRING_FORMAT( messagestr ) );

	system_message_box( STRING_CONST( "Assert Failure" ), STRING_ARGS( messagestr ), false );
#else
	log_errorf( context, ERROR_ASSERT, assert_format, sizeof( assert_format ) - 1,
		(int)cond_length, condition, (int)file_length, file, line,
		0, "", (int)msg_length, msg, 0, "" );
#endif

	return 1;
}


int assert_report_formatted( hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length, unsigned int line, const char* msg, size_t msg_length, ... )
{
	if( msg )
	{
		/*lint --e{438} Lint gets confused about assignment to ap */
		string_t buffer = { _assert_buffer, sizeof( _assert_buffer ) };
		va_list ap;
		va_start( ap, msg_length );
		buffer = string_vformat( STRING_ARGS( buffer ), msg, msg_length, ap );
		va_end( ap );
		return assert_report( context, condition, cond_length, file, file_length, line, STRING_ARGS( buffer ) );
	}

	return assert_report( context, condition, cond_length, file, file_length, line, 0, 0 );
}
