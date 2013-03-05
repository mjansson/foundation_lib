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

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
#endif
#if FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#endif


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


static CONSTCALL FORCEINLINE unsigned int _memory_get_align( unsigned int align )
{
	if( align < FOUNDATION_PLATFORM_POINTER_SIZE )
		return align ?  FOUNDATION_PLATFORM_POINTER_SIZE : 0;
	align = math_align_poweroftwo( align );
	return ( align < ( FOUNDATION_PLATFORM_POINTER_SIZE * 4 ) ) ? align : ( FOUNDATION_PLATFORM_POINTER_SIZE * 4 );
}


#if FOUNDATION_PLATFORM_POSIX

static CONSTCALL void* _memory_align_pointer( void* p, unsigned int align )
{
	uintptr_t address;
	if( !p || !align )
		return p;

	address = (uintptr_t)p;
	if( address % align )
	{
		address = address + ( align - ( address % align ) );
		p = (void*)address;
	}

	return p;
}	

#endif

static void* _memory_allocate_malloc( uint64_t size, unsigned int align, memory_hint_t hint )
{
	align = _memory_get_align( align );
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_malloc( (size_t)size, align );
#elif FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_ANDROID
	void* memory = 0;
	if( !align )
		return malloc( (size_t)size );
	int result = posix_memalign( &memory, align, size );
	if( result || !memory )
		log_errorf( ERRORLEVEL_PANIC, ERROR_OUT_OF_MEMORY, "Unable to allocate memory: %s", system_error_message( 0 ) );
	return ( result == 0 ) ? memory : 0;
#else
	void* memory = malloc( size + align );
	memory = _memory_align_pointer( memory, align );
	return memory;
#endif
}


static void* _memory_allocate_zero_malloc( uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* memory = _memory_allocate_malloc( size, align, hint );
	if( memory )
		memset( memory, 0, (size_t)size );
	return memory;
}


static void* _memory_reallocate_malloc( void* p, uint64_t size, unsigned int align )
{
	align = _memory_get_align( align );
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_realloc( p, (size_t)size, align );
#else
	if( align )
	{
		void* memory = realloc( p, size + align );
		memory = _memory_align_pointer( memory, align );
		return memory;
	}
	return realloc( p, size );
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
	memory_system_t memsystem;
	memsystem.allocate = _memory_allocate_malloc;
	memsystem.allocate_zero = _memory_allocate_zero_malloc;
	memsystem.reallocate = _memory_reallocate_malloc;
	memsystem.deallocate = _memory_deallocate_malloc;
	return memsystem;
}


