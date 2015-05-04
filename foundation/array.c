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
static const uint32_t ARRAY_WATERMARK = 0x52524145U;

static const unsigned int ARRAY_DEFAULT_ALIGN = 16U;


const void* _array_verifyfn( const void* const* arr )
{
	if( !FOUNDATION_VALIDATE_MSG( !(*arr) || ( _array_raw_const(*arr)[2] == ARRAY_WATERMARK ), "Invalid array (bad watermark)" ) )
		return 0;
	if( !FOUNDATION_VALIDATE_MSG( !(*arr) || ( _array_raw_const(*arr)[1] <= _array_raw_const(*arr)[0] ), "Invalid array (size > capacity)" ) )
		return 0;
	return *arr;
}


void* _array_resizefn( void** arr, unsigned int elements, size_t itemsize )
{
	if( !(*arr) )
		_array_growfn( arr, elements, 1, itemsize );
	else if( _array_rawcapacity( *arr ) < elements )
		_array_growfn( arr, elements - _array_rawcapacity( *arr ), 1, itemsize );
	if( *arr )
		_array_rawsize( *arr ) = elements;
	return *arr;
}


void* _array_growfn( void** arr, unsigned int increment, unsigned int factor, size_t itemsize )
{
	unsigned int prev_capacity = *arr ? _array_rawcapacity( *arr ) : 0;
	unsigned int capacity = *arr ? ( factor * prev_capacity + increment ) : increment;
	size_t       prev_storage_size = itemsize * prev_capacity;
	size_t       storage_size = itemsize * capacity;
	unsigned int header_size = 4U * _array_header_size;
	size_t       prev_used_buffer_size = prev_storage_size + header_size;
	size_t       buffer_size = storage_size + header_size;
	uint32_t*    buffer = *arr ? memory_reallocate( _array_raw( *arr ), buffer_size, ARRAY_DEFAULT_ALIGN, prev_used_buffer_size ) : memory_allocate( 0, buffer_size, ARRAY_DEFAULT_ALIGN, MEMORY_PERSISTENT );
	FOUNDATION_ASSERT_MSG( buffer, "Failed to reallocate array storage" );
	if( buffer )
	{
		buffer[0] = capacity;
		if( !*arr )
		{
			buffer[1] = 0;
			buffer[2] = ARRAY_WATERMARK;
			buffer[3] = (uint32_t)itemsize;
		}
		*arr = buffer + _array_header_size;
	}
	return *arr;
}
