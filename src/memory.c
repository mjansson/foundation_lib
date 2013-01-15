/* memory.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static memory_system_t _memsys;


int _memory_initialize( const memory_system_t memory )
{
	_memsys = memory;
	return 0;
}


void _memory_shutdown( void )
{
}


void* memory_allocate( uint64_t size, unsigned int align, memory_hint_t hint )
{
	return _memsys.allocate( size, align, hint );
}


void* memory_allocate_zero( uint64_t size, unsigned int align, memory_hint_t hint )
{
	return _memsys.allocate_zero( size, align, hint );
}


void* memory_reallocate( void* p, uint64_t size, unsigned int align )
{
	return _memsys.reallocate( p, size, align );
}


void memory_deallocate( void* p )
{
	_memsys.deallocate( p );
}
