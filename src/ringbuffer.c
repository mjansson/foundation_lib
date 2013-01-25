/* ringbuffer.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <internal.h>


typedef struct ALIGN(8) _foundation_ringbuffer_stream
{
	FOUNDATION_DECLARE_STREAM;
	semaphore_t              signal_read;
	semaphore_t              signal_write;
	volatile int32_t         pending_read;
	volatile int32_t         pending_write;
	uint64_t                 total_size;
	ringbuffer_t             buffer;
} ringbuffer_stream_t;

static stream_vtable_t _ringbuffer_stream_vtable = {0};


ringbuffer_t* ringbuffer_allocate( unsigned int size )
{
	ringbuffer_t* buffer = memory_allocate( sizeof( ringbuffer_t ) + size, 0, MEMORY_PERSISTENT );

	buffer->total_read = 0;
	buffer->total_write = 0;
	buffer->offset_read = 0;
	buffer->offset_write = 0;
	buffer->buffer_size = size;

	return buffer;
}


void ringbuffer_reset( ringbuffer_t* buffer )
{
	buffer->total_read = 0;
	buffer->total_write = 0;
	buffer->offset_read = 0;
	buffer->offset_write = 0;
}


void ringbuffer_deallocate( ringbuffer_t* buffer )
{
	memory_deallocate( buffer );
}


unsigned int ringbuffer_read( ringbuffer_t* buffer, void* dest, unsigned int num )
{
	unsigned int do_read = 0;
	unsigned int max_read = 0;

	unsigned int buffer_size = buffer->buffer_size;
	unsigned int offset_read = buffer->offset_read;
	unsigned int offset_write = buffer->offset_write;

	if( offset_read > offset_write )
		max_read = buffer_size - offset_read;
	else
		max_read = offset_write - offset_read;

	do_read = num;
	if( do_read > max_read )
		do_read = max_read;

	if( !do_read )
		return 0;

	if( dest )
		memcpy( dest, buffer->buffer + offset_read, do_read );

	offset_read += do_read;
	if( offset_read == buffer_size )
		offset_read = 0;

	buffer->offset_read = offset_read;
	buffer->total_read += do_read;

	if( ( do_read < num ) && ( offset_read == 0 ) && ( offset_write > 0 ) )
		do_read += ringbuffer_read( buffer, pointer_offset( dest, do_read ), num - do_read );

	return do_read;
}


unsigned int ringbuffer_write( ringbuffer_t* buffer, const void* source, unsigned int num )
{
	unsigned int do_write = 0;
	unsigned int max_write = 0;

	unsigned int buffer_size = buffer->buffer_size;
	unsigned int offset_read = buffer->offset_read;
	unsigned int offset_write = buffer->offset_write;

	if( offset_write >= offset_read )
	{
		max_write = buffer_size - offset_write;
		if( max_write && !buffer->offset_read ) //Don't read so write aligns to read, then the entire buffer is discarded
			--max_write;
	}
	else
		max_write = offset_read - offset_write - 1; //Don't read so write aligns to read, then the entire buffer is discarded

	do_write = num;
	if( do_write > max_write )
		do_write = max_write;

	if( !do_write )
		return 0;

	if( source )
		memcpy( buffer->buffer + offset_write, source, do_write );

	offset_write += do_write;
	if( offset_write == buffer_size )
	{
		FOUNDATION_ASSERT_MSG( buffer->offset_read, "Ring buffer internal failure, discarded entire buffer" );
		offset_write = 0;
	}

	buffer->offset_write = offset_write;
	buffer->total_write += do_write;

	if( ( do_write < num ) && ( offset_write == 0 ) && ( offset_read > 0 ) )
		do_write += ringbuffer_write( buffer, pointer_offset_const( source, do_write ), num - do_write );

	return do_write;
}


uint64_t ringbuffer_total_read( ringbuffer_t* buffer )
{
	return buffer ? buffer->total_read : 0;
}


uint64_t ringbuffer_total_written( ringbuffer_t* buffer )
{
	return buffer ? buffer->total_write : 0;
}



static uint64_t _ringbuffer_stream_read( stream_t* stream, void* dest, uint64_t num )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;

	unsigned int num_read = ringbuffer_read( &buffer->buffer, dest, (unsigned int)num );

	while( num_read < num )
	{
		buffer->pending_read = 1;

		if( buffer->pending_write )
			semaphore_post( &buffer->signal_read );

		semaphore_wait( &buffer->signal_write );

		num_read += ringbuffer_read( &buffer->buffer, pointer_offset( dest, num_read ), (unsigned int)( num - num_read ) );
	}

	if( buffer->pending_write )
		semaphore_post( &buffer->signal_read );

	return num_read;
}


static uint64_t _ringbuffer_stream_write( stream_t* stream, const void* source, uint64_t num )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;

	unsigned int num_write = ringbuffer_write( &buffer->buffer, source, (unsigned int)num );

	while( num_write < num )
	{
		buffer->pending_write = 1;

		if( buffer->pending_read )
			semaphore_post( &buffer->signal_write );

		semaphore_wait( &buffer->signal_read );

		num_write += ringbuffer_write( &buffer->buffer, pointer_offset_const( source, num_write ), (unsigned int)( num - num_write ) );
	}

	if( buffer->pending_read )
		semaphore_post( &buffer->signal_write );

	return num_write;
}


static bool _ringbuffer_stream_isopen( const stream_t* stream )
{
	return true;
}


static bool _ringbuffer_stream_eos( stream_t* stream )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;
	return buffer->total_size ? ( buffer->buffer.total_read == buffer->total_size ) : false;
}


static void _ringbuffer_stream_flush( stream_t* stream )
{
}


static void _ringbuffer_stream_truncate( stream_t* stream, uint64_t size )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;
	buffer->total_size = (unsigned int)size;
}


static uint64_t _ringbuffer_stream_size( stream_t* stream )
{
	return ((ringbuffer_stream_t*)stream)->total_size;
}


static void _ringbuffer_stream_seek( stream_t* stream, int64_t offset, stream_seek_mode_t direction )
{
	if( ( direction != STREAM_SEEK_CURRENT ) || ( offset < 0 ) )
	{
		log_errorf( ERRORLEVEL_ERROR, ERROR_UNSUPPORTED, "Invalid call, only forward seeking allowed on ringbuffer streams" );
		return;
	}
	
	_ringbuffer_stream_read( stream, 0, offset );
}


static int64_t _ringbuffer_stream_tell( stream_t* stream )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;
	return buffer->buffer.total_read;
}


static uint64_t _ringbuffer_stream_lastmod( const stream_t* stream )
{
	return time_current();
}


static uint64_t _ringbuffer_stream_available_read( stream_t* stream )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;
	//TODO: Will be f*ked up if buffer has wrapped around (overflow)
	return buffer->buffer.total_write - buffer->buffer.total_read;
}


static void _ringbuffer_stream_deallocate( stream_t* stream )
{
	ringbuffer_stream_t* buffer = (ringbuffer_stream_t*)stream;

	semaphore_destroy( &buffer->signal_read );
	semaphore_destroy( &buffer->signal_write );
}


stream_t* ringbuffer_stream_allocate( unsigned int buffer_size, uint64_t total_size )
{
	ringbuffer_stream_t* bufferstream = memory_allocate_zero( sizeof( ringbuffer_stream_t ) + buffer_size, 0, MEMORY_PERSISTENT );
	stream_t* stream = (stream_t*)bufferstream;

	_stream_initialize( stream );

	stream->type = STREAMTYPE_RINGBUFFER;
	stream->sequential = 1;
	stream->path = string_format( "ringbuffer://0x%p", stream );
	stream->mode = STREAM_OUT | STREAM_IN | STREAM_BINARY;

	semaphore_initialize( &bufferstream->signal_read, 0 );
	semaphore_initialize( &bufferstream->signal_write, 0 );

	bufferstream->pending_read = 0;
	bufferstream->pending_write = 0;
	bufferstream->total_size = total_size;
	bufferstream->buffer.buffer_size = buffer_size;

	stream->vtable = &_ringbuffer_stream_vtable;

	return stream;
}


void _ringbuffer_stream_initialize( void )
{
	//Setup global vtable
	_ringbuffer_stream_vtable.read = _ringbuffer_stream_read;
	_ringbuffer_stream_vtable.write = _ringbuffer_stream_write;
	_ringbuffer_stream_vtable.isopen = _ringbuffer_stream_isopen;
	_ringbuffer_stream_vtable.eos = _ringbuffer_stream_eos;
	_ringbuffer_stream_vtable.flush = _ringbuffer_stream_flush;
	_ringbuffer_stream_vtable.truncate = _ringbuffer_stream_truncate;
	_ringbuffer_stream_vtable.size = _ringbuffer_stream_size;
	_ringbuffer_stream_vtable.seek = _ringbuffer_stream_seek;
	_ringbuffer_stream_vtable.tell = _ringbuffer_stream_tell;
	_ringbuffer_stream_vtable.lastmod = _ringbuffer_stream_lastmod;
	_ringbuffer_stream_vtable.available_read = _ringbuffer_stream_available_read;
	_ringbuffer_stream_vtable.deallocate = _ringbuffer_stream_deallocate;
}


