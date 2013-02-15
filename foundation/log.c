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

#include <stdio.h>
#include <stdarg.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/safewindows.h>
#  define va_copy(d,s) ((d)=(s))
#  define snprintf( p, s, ... ) _snprintf_s( p, s, _TRUNCATE, __VA_ARGS__ )
__declspec(dllimport) void __stdcall OutputDebugStringA(LPCSTR);
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <android/log.h>
#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID
#  include <unistd.h>
#  include <stdarg.h>
#  include <sys/types.h>
#  include <sys/wait.h>
#endif

static bool             _log_stdout      = true;
static bool             _log_prefix      = true;
static log_callback_fn  _log_callback    = 0;

#define make_timestamp()  ((float32_t)( (real)( time_current() - time_startup() ) / (real)time_ticks_per_second() ))


#if BUILD_ENABLE_LOG || BUILD_ENABLE_DEBUG_LOG

static void _log_outputf( int severity, const char* prefix, const char* format, va_list list, void* std )
{
	float32_t timestamp = make_timestamp();
	uint64_t tid = thread_id();
	unsigned int pid = thread_hardware();
	int need, more, remain, size = 383;
	char local_buffer[385];
	char* buffer = local_buffer;
	va_list clist;
	while(1)
	{
		//This is guaranteed to always fit in minimum size of 383 bytes defined above, so need is always > 0
		if( _log_prefix )
			need = snprintf( buffer, size, "[%.3f] <%llx:%d> %s", timestamp, tid, pid, prefix );
		else
			need = snprintf( buffer, size, "%s", prefix );

		remain = size - need;
		va_copy( clist, list );
		more = vsnprintf( buffer + need, remain, format, clist );
		va_end( clist );
			
		if( ( more > -1 ) && ( more < remain ) )
		{
			buffer[need+more] = '\n';
			buffer[need+more+1] = 0;

#if FOUNDATION_PLATFORM_WINDOWS
			OutputDebugStringA( buffer );
#endif

#if FOUNDATION_PLATFORM_ANDROID
			if( _log_stdout )
				__android_log_write( ANDROID_LOG_DEBUG + severity, environment_application()->short_name, buffer );
#else
			if( _log_stdout && std )
				fprintf( std, "%s", buffer );
#endif

			if( _log_callback )
				_log_callback( severity, buffer );

			break;
		}

		if( ( more > -1 ) && ( need > -1 ) )
			size = more + need + 1;
		else
			size *= 2;

		if( buffer != local_buffer )
			memory_deallocate( buffer );
		buffer = memory_allocate( size + 2, 0, MEMORY_TEMPORARY );
	}
	if( buffer != local_buffer )
		memory_deallocate( buffer );
}

#endif

#if BUILD_ENABLE_DEBUG_LOG

void log_debugf( const char* format, ... )
{
	va_list list;
	va_start( list, format );
	_log_outputf( ERRORLEVEL_DEBUG, "", format, list, stdout );
	va_end( list );
}

#endif

#if BUILD_ENABLE_LOG

void log_infof( const char* format, ... )
{
	va_list list;
	va_start( list, format );
	_log_outputf( ERRORLEVEL_INFO, "", format, list, stdout );
	va_end( list );
}

void log_warnf( warning_class_t wclass, const char* format, ... )
{
	va_list list;

	log_error_context( ERRORLEVEL_WARNING );

	va_start( list, format );
	_log_outputf( ERRORLEVEL_WARNING, "WARNING: ", format, list, stdout );
	va_end( list );
}


void log_errorf( error_level_t level, error_t err, const char* format, ... )
{
	va_list list;

	log_error_context( ERRORLEVEL_ERROR );

	va_start( list, format );
	_log_outputf( ERRORLEVEL_ERROR, "ERROR: ", format, list, stderr );
	va_end( list );

	error_report( level, err );
}


static void _log_error_contextf( error_level_t error_level, void* std, const char* format, ... )
{
	va_list list;
	va_start( list, format );
	_log_outputf( error_level, "", format, list, std );
	va_end( list );
}


void log_error_context( error_level_t error_level )
{
	int i;
	error_context_t* context = error_context();
	if( context )
	{
		error_frame_t* frame = context->frame;
		for( i = 0; i < context->depth; ++i, ++frame )
			_log_error_contextf( error_level, stderr, "When %s: %s", frame->name ? frame->name : "<something>", frame->data ? frame->data : "" );
	}
}

#endif


#if BUILD_ENABLE_LOG

void log_stdout( bool enable )
{
	_log_stdout = enable;
}


void log_set_callback( log_callback_fn callback )
{
	_log_callback = callback;
}


void log_enable_prefix( bool enable )
{
	_log_prefix = enable;
}

#endif
