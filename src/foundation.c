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

extern int _timer_initialize( void );
extern void _timer_shutdown( void );

extern int _thread_initialize( void );
extern void _thread_shutdown( void );

extern int _environment_initialize( const application_t application );
extern void _environment_shutdown( void );

extern int _config_initialize( void );
extern void _config_shutdown( void );


int foundation_initialize( const memory_system_t memory, const application_t application )
{
	if( _memory_initialize( memory ) < 0 )
		return -1;

	if( _timer_initialize() < 0 )
		return -1;

	if( _thread_initialize() < 0 )
		return -1;

	if( _environment_initialize( application ) < 0 )
		return -1;

	if( _config_initialize() < 0 )
		return -1;

	return 0;
}


void foundation_shutdown( void )
{
	_config_shutdown();
	_environment_shutdown();
	_thread_shutdown();
	_timer_shutdown();
	_memory_shutdown();
}
