/* array.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


//'FARR' in ascii
#define _array_watermark 0x52524145

void _array_verifyfn( const void* const* arr )
{
	FOUNDATION_ASSERT_MSG( !(*arr) || ( _array_raw_const(*arr)[2] == _array_watermark ), "Invalid array" );
}


void _array_growfn( void* arr, int increment, int factor, int itemsize )
{
	void**   parr = (void**)arr;
	int      capacity = *parr ? ( factor * _array_rawcapacity(*parr) + increment ) : increment;
	int      storage_size = itemsize * capacity;
	uint64_t header_size = 4ULL * _array_header_size;
	uint64_t buffer_size = (unsigned int)storage_size + header_size;
	int*     buffer = *parr ? memory_reallocate( _array_raw( *parr ), buffer_size, 0 ) : memory_allocate( buffer_size, 16, MEMORY_PERSISTENT );
	FOUNDATION_ASSERT_MSG( buffer, "Failed to reallocate array storage" );
	if( buffer )
	{
		buffer[0] = capacity;
		if( !*parr )
		{
			buffer[1] = 0;
			buffer[2] = _array_watermark;
		}
		*parr = buffer + _array_header_size;
	}
}
