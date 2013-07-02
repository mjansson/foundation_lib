/* foundation.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


// Internals
extern int _memory_initialize( const memory_system_t memory );
extern void _memory_preallocate( void );
extern void _memory_shutdown( void );

extern int _time_initialize( void );
extern void _time_shutdown( void );

extern int _thread_initialize( void );
extern void _thread_shutdown( void );

extern int _environment_initialize( const application_t application );
extern void _environment_shutdown( void );

extern int _library_initialize( void );
extern void _library_shutdown( void );

extern int _system_initialize( void );
extern void _system_shutdown( void );

extern int _fs_initialize( void );
extern void _fs_shutdown( void );

extern int _random_initialize( void );
extern void _random_shutdown( void );

extern int _config_initialize( void );
extern void _config_shutdown( void );

//Make artificial reference to main entry point
#if FOUNDATION_PLATFORM_ANDROID
struct android_app;
extern void android_main( struct android_app* );
#else
extern int main( int, char** );
#endif

#if !BUILD_DEPLOY && FOUNDATION_PLATFORM_FAMILY_DESKTOP
extern void _static_hash_cleanup( void );
#endif

static bool _foundation_initialized = false;


int foundation_initialize( const memory_system_t memory, const application_t application )
{
	if( _memory_initialize( memory ) < 0 )
		return -1;

	if( _time_initialize() < 0 )
		return -1;

	if( _thread_initialize() < 0 )
		return -1;

	if( _random_initialize() < 0 )
		return -1;

	if( _fs_initialize() < 0 )
		return -1;

	if( _environment_initialize( application ) < 0 )
		return -1;

	if( _library_initialize() < 0 )
		return -1;

	if( _system_initialize() < 0 )
		return -1;

	if( _config_initialize() < 0 )
		return -1;

	//Parse built-in command line options
	{
		const char* const* cmdline = environment_command_line();
		for( unsigned int iarg = 0, argsize = array_size( cmdline ); iarg < argsize; ++iarg )
		{
			if( string_equal( cmdline[iarg], "--log-debug" ) )
				log_suppress( ERRORLEVEL_NONE );
			else if( string_equal( cmdline[iarg], "--log-info" ) )
				log_suppress( ERRORLEVEL_DEBUG );
		}

		config_parse_commandline( cmdline, array_size( cmdline ) );
	}

	//Artificial reference and sanity check
#if FOUNDATION_PLATFORM_ANDROID
	if( (uintptr_t)android_main < 1 )
		return -1;
#else
	if( (uintptr_t)main < 1 )
		return -1;
#endif

	_foundation_initialized = true;
	
	return 0;
}


void foundation_startup( void )
{
	_memory_preallocate();
}


void foundation_shutdown( void )
{
	_foundation_initialized = false;

	_config_shutdown();
	_fs_shutdown();
	_system_shutdown();
	_library_shutdown();
	_environment_shutdown();
	_random_shutdown();
	_thread_shutdown();
	_time_shutdown();

#if !BUILD_DEPLOY && FOUNDATION_PLATFORM_FAMILY_DESKTOP
	_static_hash_cleanup();
#endif
	
	_memory_shutdown();
}


bool foundation_is_initialized( void )
{
	return _foundation_initialized;
}
