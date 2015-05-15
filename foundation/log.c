/* log.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <stdio.h>
#include <stdarg.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#  define snprintf( p, s, ... ) _snprintf_s( p, s, _TRUNCATE, __VA_ARGS__ )
#  define vsnprintf( s, n, format, arg ) _vsnprintf_s( s, n, _TRUNCATE, format, arg )
__declspec(dllimport) void STDCALL OutputDebugStringA(LPCSTR);
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <android/log.h>
#endif

#if FOUNDATION_PLATFORM_TIZEN
#  include <foundation/tizen.h>
#endif

#if FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#endif

#if FOUNDATION_PLATFORM_PNACL
#  include <foundation/pnacl.h>
#endif

#if BUILD_ENABLE_LOG || BUILD_ENABLE_DEBUG_LOG

static bool             _log_stdout           = true;
static bool             _log_prefix           = true;
static log_callback_fn  _log_callback;
static hashtable64_t*   _log_suppress;
static error_level_t    _log_suppress_default;

#define LOG_WARNING_NAMES 9
static char _log_warning_name[LOG_WARNING_NAMES][18] = {
	"performance",
	"deprecated",
	"bad data",
	"memory",
	"unsupported",
	"suspicious",
	"system call fail",
	"deadlock",
	"script"
};

#define LOG_ERROR_NAMES 16
static char _log_error_name[LOG_ERROR_NAMES][18] = {
	"none",
	"invalid value",
	"unsupported",
	"not implemented",
	"out of memory",
	"memory leak",
	"memory alignment",
	"internal failure",
	"access denied",
	"exception",
	"system call fail",
	"unknown type",
	"unknown resource",
	"deprecated",
	"assert",
	"script"
};


typedef struct _log_timestamp
{
	int   hours;
	int   minutes;
	int   seconds;
	int   milliseconds;
} log_timestamp_t;


static log_timestamp_t _log_make_timestamp( void )
{
	tick_t elapsed;
	tick_t ticks_per_sec;
	tick_t milliseconds;
	tick_t seconds;
	tick_t minutes;

	log_timestamp_t timestamp;

	ticks_per_sec = time_ticks_per_second();
	if( !ticks_per_sec )
	{
		memset( &timestamp, 0, sizeof( timestamp ) );
		return timestamp;
	}

	elapsed = time_current() - time_startup();
	milliseconds = ( ( elapsed % ticks_per_sec ) * 1000LL ) / ticks_per_sec;
	seconds = elapsed / ticks_per_sec;
	minutes = seconds / 60LL;

	timestamp.milliseconds = ( milliseconds % 1000LL );
	timestamp.seconds = ( seconds % 60LL );
	timestamp.minutes = ( minutes % 60LL );
	timestamp.hours = (int)( minutes / 24LL );

	return timestamp;
}

#endif

#if BUILD_ENABLE_LOG

static void FOUNDATION_ATTRIBUTE4( format, printf, 5, 0 ) _log_outputf( hash_t context, error_level_t severity, const char* prefix, size_t prefix_length, const char* format, size_t format_length, va_list list, void* std )
{
	log_timestamp_t timestamp = _log_make_timestamp();
	uint64_t tid = thread_id();
	unsigned int pid = thread_hardware();
	int need, more, remain;
	int size = 383;
	char local_buffer[385];
	char* buffer = local_buffer;
	FOUNDATION_UNUSED( format_length );
	while(1)
	{
		//This is guaranteed to always fit in minimum size of 383 bytes defined above, so need is always > 0
		if( _log_prefix )
			need = snprintf( buffer, (size_t)size, "[%d:%02d:%02d.%03d] <%" PRIx64 ":%d> %.*s", timestamp.hours, timestamp.minutes, timestamp.seconds, timestamp.milliseconds, tid, pid, (int)prefix_length, prefix );
		else
			need = snprintf( buffer, (size_t)size, "%.*s", (int)prefix_length, prefix );

		remain = size - need;
		{
			va_list clist;
			va_copy( clist, list );
			more = vsnprintf( buffer + need, (size_t)remain, format, clist );
			va_end( clist );
		}

		if( ( more > -1 ) && ( more < remain ) )
		{
			buffer[need+more] = '\n';
			buffer[need+more+1] = 0;

#if FOUNDATION_PLATFORM_WINDOWS
			OutputDebugStringA( buffer );
#endif

#if FOUNDATION_PLATFORM_ANDROID
			FOUNDATION_UNUSED( std );
			if( _log_stdout )
				__android_log_write( ANDROID_LOG_DEBUG + severity - 1, environment_application()->short_name, buffer );
#elif FOUNDATION_PLATFORM_TIZEN
			FOUNDATION_UNUSED( std );
			if( _log_stdout )
				dlog_print( DLOG_DEBUG + severity - 1, environment_application()->short_name, "%s", buffer );
#elif FOUNDATION_PLATFORM_PNACL
			FOUNDATION_UNUSED( std );
			if( _log_stdout )
				pnacl_post_log( context, severity, buffer, (size_t)( need + more + 1 ) );
#else
			if( _log_stdout && std )
				fprintf( std, "%s", buffer );
#endif

			if( _log_callback )
				_log_callback( context, severity, buffer, (size_t)( need + more ) );

			break;
		}

		if( ( more > -1 ) && ( need > -1 ) )
			size = more + need + 1;
		else
			size *= 2;

		if( buffer != local_buffer )
			memory_deallocate( buffer );
		buffer = memory_allocate( 0, (size_t)size + 2, 0, MEMORY_TEMPORARY );
	}
	if( buffer != local_buffer )
		memory_deallocate( buffer );
}

#endif


#if BUILD_ENABLE_LOG && BUILD_ENABLE_DEBUG_LOG


void log_debugf( hash_t context, const char* format, size_t length, ... )
{
	va_list list;
	va_start( list, length );
	if( log_suppress( context ) < ERRORLEVEL_DEBUG )
		_log_outputf( context, ERRORLEVEL_DEBUG, "", 0, format, length, list, stdout );
	va_end( list );
}


#endif


#if BUILD_ENABLE_LOG


void log_infof( hash_t context, const char* format, size_t length, ... )
{
	va_list list;
	va_start( list, length );
	if( log_suppress( context ) < ERRORLEVEL_INFO )
		_log_outputf( context, ERRORLEVEL_INFO, "", 0, format, length, list, stdout );
	va_end( list );
}


void log_warnf( hash_t context, warning_t warn, const char* format, size_t length, ... )
{
	char buffer[32];
	string_t prefix;
	va_list list;

	if( log_suppress( context ) >= ERRORLEVEL_WARNING )
		return;

	log_error_context( context, ERRORLEVEL_WARNING );

	if( warn < LOG_WARNING_NAMES )
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "WARNING [%s]: " ), _log_warning_name[warn] );
	else
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "WARNING [%d]: " ), warn );

	va_start( list, length );
	_log_outputf( context, ERRORLEVEL_WARNING, prefix.str, prefix.length, format, length, list, stdout );
	va_end( list );
}


void log_errorf( hash_t context, error_t err, const char* format, size_t length, ... )
{
	char buffer[32];
	string_t prefix;
	va_list list;

	if( log_suppress( context ) >= ERRORLEVEL_ERROR )
		return;

	log_error_context( context, ERRORLEVEL_ERROR );

	if( err < LOG_ERROR_NAMES )
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "ERROR [%s]: " ), _log_error_name[err] );
	else
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "ERROR [%d]: " ), err );

	va_start( list, length );
	_log_outputf( context, ERRORLEVEL_ERROR, prefix.str, prefix.length, format, length, list, stderr );
	va_end( list );

	error_report( ERRORLEVEL_ERROR, err );
}


void log_panicf( hash_t context, error_t err, const char* format, size_t length, ... )
{
	char buffer[32];
	string_t prefix;
	va_list list;

	log_error_context( context, ERRORLEVEL_PANIC );

	if( err < LOG_ERROR_NAMES )
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "PANIC [%s]: " ), _log_error_name[err] );
	else
		prefix = string_format_buffer( buffer, 32, STRING_CONST( "PANIC [%d]: " ), err );

	va_start( list, length );
	_log_outputf( context, ERRORLEVEL_PANIC, prefix.str, prefix.length, format, length, list, stderr );
	va_end( list );

	error_report( ERRORLEVEL_PANIC, err );
}


static void FOUNDATION_ATTRIBUTE4( format, printf, 4, 6 ) _log_error_contextf( hash_t context, error_level_t error_level, void* std, const char* format, size_t length, ... )
{
	va_list list;
	va_start( list, length );
	_log_outputf( context, error_level, "", 0, format, length, list, std );
	va_end( list );
}


void log_error_context( hash_t context, error_level_t error_level )
{
	size_t i;
	error_context_t* err_context = error_context();
	if( err_context && ( log_suppress( context ) < error_level ) )
	{
		error_frame_t* frame = err_context->frame;
		for( i = 0; i < err_context->depth; ++i, ++frame )
			_log_error_contextf( context, error_level, error_level > ERRORLEVEL_WARNING ? stderr : stdout, STRING_CONST( "When %s: %s" ), frame->name ? frame->name : "<something>", frame->data ? frame->data : "" );
	}
}


void log_enable_stdout( bool enable )
{
	_log_stdout = enable;
}


log_callback_fn log_callback( void )
{
	return _log_callback;
}


void log_set_callback( log_callback_fn callback )
{
	_log_callback = callback;
}


void log_enable_prefix( bool enable )
{
	_log_prefix = enable;
}


void log_set_suppress( hash_t context, error_level_t level )
{
	if( !context )
		_log_suppress_default = level;
	else if( _log_suppress )
		hashtable64_set( _log_suppress, context, level + 1 );
}


error_level_t log_suppress( hash_t context )
{
	if( !context )
		return _log_suppress_default;
	else if( _log_suppress )
	{
		uint64_t level = hashtable64_get( _log_suppress, context );
		if( level > 0 )
			return (error_level_t)( level - 1 );
	}
	return _log_suppress_default;
}


void log_suppress_clear( void )
{
	_log_suppress_default = ERRORLEVEL_NONE;
    if( _log_suppress )
        hashtable64_clear( _log_suppress );
}


#endif


int _log_initialize( void )
{
#if BUILD_ENABLE_LOG
	_log_suppress = hashtable64_allocate( 149 );
#endif
	return 0;
}


void _log_shutdown( void )
{
#if BUILD_ENABLE_LOG
	hashtable64_deallocate( _log_suppress );
	_log_suppress = 0;
#endif
}

