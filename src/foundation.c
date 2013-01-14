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
extern int _timer_initialize( void );
extern void _timer_shutdown( void );


int foundation_initialize()
{
	if( _timer_initialize() < 0 )
		return -1;

	return 0;
}


void foundation_shutdown()
{
	_timer_shutdown();
}
