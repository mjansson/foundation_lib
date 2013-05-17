/* bufferstream.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/internal.h>


typedef struct ALIGN(8) _foundation_stream_buffer
{
	FOUNDATION_DECLARE_STREAM;
	void*                    buffer;
	uint64_t                 current;
	uint64_t                 size;
	uint64_t                 capacity;
	bool                     own;
	bool                     grow;
} stream_buffer_t;

static stream_vtable_t _buffer_stream_vtable;


stream_t* buffer_stream_allocate( void* buffer, unsigned int mode, uint64_t size, uint64_t capacity, bool adopt, bool grow )
{
	stream_buffer_t* buffer_stream = memory_allocate_zero_context( MEMORYCONTEXT_STREAM, sizeof( stream_buffer_t ), 0, MEMORY_PERSISTENT );
	stream_t* stream = (stream_t*)buffer_stream;

	_stream_initialize( stream, system_byteorder() );

	FOUNDATION_ASSERT_MSG( adopt || !grow, "Cannot grow buffer streams that are not adopted" );

	if( !buffer )
	{
		size = 0;
		capacity = 0;
	}

	if( size > capacity )
		size = capacity;

	buffer_stream->type = STREAMTYPE_MEMORY;
	buffer_stream->path = string_format( "buffer://" STRING_FORMAT_POINTER, stream );
	buffer_stream->mode = mode & ( STREAM_OUT | STREAM_IN | STREAM_BINARY );
	buffer_stream->buffer = buffer;
	buffer_stream->size = size;
	buffer_stream->capacity = capacity;
	buffer_stream->own = adopt;
	buffer_stream->grow = ( adopt && grow );

	if( mode & STREAM_TRUNCATE )
		buffer_stream->size = 0;
	if( mode & STREAM_ATEND )
		buffer_stream->current = buffer_stream->size;

	buffer_stream->vtable = &_buffer_stream_vtable;

	return stream;
}


static void _buffer_stream_deallocate( stream_t* stream )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	if( buffer_stream->own )
		memory_deallocate( buffer_stream->buffer );
}


static uint64_t _buffer_stream_read( stream_t* stream, void* dest, uint64_t num )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;

	int64_t available = buffer_stream->size - buffer_stream->current;
	int64_t num_read = ( (int64_t)num < available ) ? (int64_t)num : available;

	if( num_read > 0 )
	{
		memcpy( dest, pointer_offset( buffer_stream->buffer, buffer_stream->current ), (size_t)num_read );
		buffer_stream->current += num_read;
		return num_read;
	}

	return 0;
}


static uint64_t _buffer_stream_write( stream_t* stream, const void* source, uint64_t num )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;

	int64_t available = buffer_stream->size - buffer_stream->current;
	int64_t want = (int64_t)num;
	int64_t num_write = 0;

	if( want > available )
	{
		if( buffer_stream->capacity >= ( buffer_stream->current + want ) )
		{
			available = want;
			buffer_stream->size = buffer_stream->current + want;
		}
		else if( buffer_stream->grow )
		{
			available = want;
			buffer_stream->capacity = buffer_stream->current + want;
			buffer_stream->buffer = memory_reallocate( buffer_stream->buffer, buffer_stream->capacity, 0 );
			buffer_stream->size = buffer_stream->capacity;
		}
		else
		{
			available = buffer_stream->capacity - buffer_stream->current;
			buffer_stream->size = buffer_stream->current + available;
		}
	}

	num_write = ( want < available ) ? want : available;
	if( num_write > 0 )
	{
		memcpy( pointer_offset( buffer_stream->buffer, buffer_stream->current ), source, (size_t)num_write );
		buffer_stream->current += num_write;
		return num_write;
	}

	return 0;
}


static bool _buffer_stream_eos( stream_t* stream )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	return buffer_stream->current >= buffer_stream->size;
}


static void _buffer_stream_flush( stream_t* stream )
{
}


static void _buffer_stream_truncate( stream_t* stream, uint64_t size )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	if( buffer_stream->capacity >= size )
	{
		buffer_stream->size = size;
	}
	else if( buffer_stream->grow )
	{
		buffer_stream->capacity = size;
		buffer_stream->buffer = memory_reallocate( buffer_stream->buffer, buffer_stream->capacity, 0 );
		buffer_stream->size = buffer_stream->capacity;
	}
	else
	{
		buffer_stream->size = buffer_stream->capacity;
	}
	if( buffer_stream->current > buffer_stream->size )
		buffer_stream->current = buffer_stream->size;
}


static uint64_t _buffer_stream_size( stream_t* stream )
{
	return ((stream_buffer_t*)stream)->size;
}


static void _buffer_stream_seek( stream_t* stream, int64_t offset, stream_seek_mode_t direction )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	int64_t new_current = 0;
	if( direction == STREAM_SEEK_CURRENT )
		new_current = (int64_t)buffer_stream->current + offset;
	else if( direction == STREAM_SEEK_BEGIN )
		new_current = offset;
	else if( direction == STREAM_SEEK_END )
		new_current = buffer_stream->size + offset;

	if( new_current < 0 )
		buffer_stream->current = 0;
	else if( new_current > (int64_t)buffer_stream->size )
		buffer_stream->current = buffer_stream->size;
	else
		buffer_stream->current = new_current;
}


static int64_t _buffer_stream_tell( stream_t* stream )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	return buffer_stream->current;
}


static uint64_t _buffer_stream_lastmod( const stream_t* stream )
{
	return time_current();
}


static uint64_t _buffer_stream_available_read( stream_t* stream )
{
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	return buffer_stream->size - buffer_stream->current;
}


void _buffer_stream_initialize( void )
{
	_buffer_stream_vtable.read = _buffer_stream_read;
	_buffer_stream_vtable.write = _buffer_stream_write;
	_buffer_stream_vtable.eos = _buffer_stream_eos;
	_buffer_stream_vtable.flush = _buffer_stream_flush;
	_buffer_stream_vtable.truncate = _buffer_stream_truncate;
	_buffer_stream_vtable.size = _buffer_stream_size;
	_buffer_stream_vtable.seek = _buffer_stream_seek;
	_buffer_stream_vtable.tell = _buffer_stream_tell;
	_buffer_stream_vtable.lastmod = _buffer_stream_lastmod;
	_buffer_stream_vtable.available_read = _buffer_stream_available_read;
	_buffer_stream_vtable.deallocate = _buffer_stream_deallocate;
}
