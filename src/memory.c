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

#include <malloc.h>
#include <stdlib.h>


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



static void* _memory_allocate_malloc( uint64_t size, unsigned int align, memory_hint_t hint )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_malloc( size, align );
#elif FOUNDATION_PLATFORM_POSIX
	void* memory = 0;
	result = posix_memalign( &memory, align, size );
	if( result == 0 )
		return memory;
#else
#  error Not implemented
#endif
}


static void* _memory_allocate_zero_malloc( uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* memory = _memory_allocate_malloc( size, align, hint );
	if( memory )
		memset( memory, 0, size );
	return memory;
}


static void* _memory_reallocate_malloc( void* p, uint64_t size, unsigned int align )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_realloc( p, size, align );
#elif FOUNDATION_PLATFORM_POSIX
	//No posix_memalign_realloc
	void* memory = realloc( p, size + align );
	if( (uintptr_t)memory % align )
		memory = pointer_offset( memory, align - ( (uintptr_t)memory % align ) );
	return memory;
#else
#  error Not implemented
#endif
}


static void _memory_deallocate_malloc( void* p )
{
#if FOUNDATION_PLATFORM_WINDOWS
	if( p )
		_aligned_free( p );
#elif FOUNDATION_PLATFORM_POSIX
	free( p );
#else
#  error Not implemented
#endif
}


memory_system_t memory_system_malloc( void )
{
	memory_system_t system;
	system.allocate = _memory_allocate_malloc;
	system.allocate_zero = _memory_allocate_zero_malloc;
	system.reallocate = _memory_reallocate_malloc;
	system.deallocate = _memory_deallocate_malloc;
	return system;
}


