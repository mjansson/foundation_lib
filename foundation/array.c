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
static const int ARRAY_WATERMARK = 0x52524145;

static const unsigned int ARRAY_DEFAULT_ALIGN = 16U;


void _array_verifyfn( const void* const* arr )
{
	FOUNDATION_ASSERT_MSG( !(*arr) || ( _array_raw_const(*arr)[2] == ARRAY_WATERMARK ), "Invalid array" );
}


void _array_growfn( void* arr, int increment, int factor, int itemsize )
{
	void**   parr = (void**)arr;
	int      prev_capacity = *parr ? _array_rawcapacity(*parr) : 0;
	int      capacity = *parr ? ( factor * prev_capacity + increment ) : increment;
	int      prev_used_size = itemsize * prev_capacity;
	int      storage_size = itemsize * capacity;
	uint64_t header_size = 4ULL * _array_header_size;
	uint64_t prev_used_buffer_size = (unsigned int)prev_used_size + header_size;
	uint64_t buffer_size = (unsigned int)storage_size + header_size;
	int*     buffer = *parr ? memory_reallocate( _array_raw( *parr ), buffer_size, ARRAY_DEFAULT_ALIGN, prev_used_buffer_size ) : memory_allocate( buffer_size, ARRAY_DEFAULT_ALIGN, MEMORY_PERSISTENT );
	FOUNDATION_ASSERT_MSG( buffer, "Failed to reallocate array storage" );
	if( buffer )
	{
		buffer[0] = capacity;
		if( !*parr )
		{
			buffer[1] = 0;
			buffer[2] = ARRAY_WATERMARK;
		}
		*parr = buffer + _array_header_size;
	}
}
