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

#include <foundation.h>


// Internals
extern int _memory_initialize( const memory_system_t memory );
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
FOUNDATION_EXTERN void android_main( struct android_app* );
#else
FOUNDATION_EXTERN int main( int, char** );
#endif


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

	if( _environment_initialize( application ) < 0 )
		return -1;

	if( _library_initialize() < 0 )
		return -1;

	if( _system_initialize() < 0 )
		return -1;

	if( _fs_initialize() < 0 )
		return -1;

	if( _config_initialize() < 0 )
		return -1;

	//Create artificial reference to main entry point
#if FOUNDATION_PLATFORM_ANDROID
	return android_main ? 0 : -1;
#else
	return main ? 0 : -1;
#endif
}


void foundation_shutdown( void )
{
	_config_shutdown();
	_fs_shutdown();
	_system_shutdown();
	_library_shutdown();
	_environment_shutdown();
	_random_shutdown();
	_thread_shutdown();
	_time_shutdown();
	_memory_shutdown();
}
