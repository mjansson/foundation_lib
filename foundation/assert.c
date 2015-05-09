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


#define ASSERT_BUFFER_SIZE             2048U

static assert_handler_fn _assert_handler;
static char              _assert_buffer[ASSERT_BUFFER_SIZE];

#if BUILD_ENABLE_ASSERT
#define ASSERT_STACKTRACE_MAX_DEPTH    128U
#define ASSERT_STACKTRACE_SKIP_FRAMES  1U
static void*             _assert_stacktrace[ASSERT_STACKTRACE_MAX_DEPTH];
static char              _assert_context_buffer[ASSERT_BUFFER_SIZE];
static char              _assert_stacktrace_buffer[ASSERT_BUFFER_SIZE];
static char              _assert_message_buffer[ASSERT_BUFFER_SIZE];
#endif


assert_handler_fn assert_handler( void )
{
	return _assert_handler;
}


void assert_set_handler( assert_handler_fn new_handler )
{
	_assert_handler = new_handler;
}


int assert_report( hash_t context, const char* condition, const char* file, unsigned int line, string_const_t msg )
{
	static const char nocondition[] = "<Static fail>";
	static const char nofile[] = "<No file>";
	static const char nomsg[] = "<No message>";
	static const char assert_format[] = "****** ASSERT FAILED ******\nCondition: %s\nFile/line: %s : %d\n%s%s\n%s\n";
	string_t tracestr = { ASSERT_BUFFER_SIZE, _assert_stacktrace_buffer };
	string_t contextstr = { ASSERT_BUFFER_SIZE, _assert_context_buffer };
	string_t messagestr = { ASSERT_BUFFER_SIZE, _assert_message_buffer };

	if( !condition  ) condition = nocondition;
	if( !file       ) file      = nofile;
	if( !msg.length ) msg       = string_const( nomsg, sizeof( nomsg ) );

	if( _assert_handler && ( _assert_handler != assert_report ) )
		return (*_assert_handler)( context, condition, file, line, msg );

#if BUILD_ENABLE_ASSERT
	contextstr = error_context_string( contextstr );

	if( foundation_is_initialized() )
	{
		unsigned int num_frames = stacktrace_capture( _assert_stacktrace, ASSERT_STACKTRACE_MAX_DEPTH, ASSERT_STACKTRACE_SKIP_FRAMES );
		if( num_frames )
			tracestr = stacktrace_resolve( tracestr, _assert_stacktrace, num_frames, 0U );
		else
			tracestr = string_copy( tracestr, string_const( "<no stacktrace>", 15 ) );
	}
	else
	{
		tracestr = string_copy( tracestr, string_const( "<no stacktrace - not initialized>", 32 ) );
	}

	messagestr = string_format_string( messagestr, assert_format, condition, file, line, contextstr.str, msg.str, tracestr.str );

	log_errorf( context, ERROR_ASSERT, "%s", messagestr.str );

	system_message_box( "Assert Failure", messagestr.str, false );
#else
	log_errorf( context, ERROR_ASSERT, assert_format, condition, file, line, "", msg.str, "" );
#endif

	return 1;
}


int assert_report_formatted( hash_t context, const char* condition, const char* file, unsigned int line, const char* msg, ... )
{
	string_t buffer = { ASSERT_BUFFER_SIZE, _assert_buffer };
	if( msg )
	{
		/*lint --e{438} Lint gets confused about assignment to ap */
		va_list ap;
		va_start( ap, msg );
		buffer = string_vformat_string( buffer, msg, ap );
		va_end( ap );
	}
	else
	{
		buffer = string_resize( buffer, 0, 0 );
	}
	return assert_report( context, condition, file, line, string_to_const( buffer ) );
}
