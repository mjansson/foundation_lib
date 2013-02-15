/* event.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

//
//This sets up a lock-free structure of many-writers, single-reader and yield-spinning while waiting
//for other threads to finish writing or swap-for-read. No locks needed as only write block needs to be protected
//
//The expected contention for writing is expected to be low as the write is a quick operation and number
//of events posted to the same stream from different threads is very low
//

#define EVENT_BLOCK_POSTING  -1
#define EVENT_BLOCK_SWAPPING -2

struct _foundation_event_block
{
	volatile uint32_t                used;
	uint32_t                         capacity;
	event_t*                         events;
};

struct ALIGN(16) _foundation_event_stream
{
	volatile int32_t                 write;
	volatile int32_t                 read;
	event_block_t                    block[2];
};

static int32_t _event_serial = 1;


void event_post( event_stream_t* stream, uint8_t systemid, uint8_t id, uint16_t size, uint64_t object, const void* payload )
{
	event_block_t* block;
	event_t* event;
	bool restored_block;
	uint32_t allocsize;
	int32_t last_write;

	//Events must be aligned to an even 4 bytes
	allocsize = sizeof( event_t ) + size;
	if( allocsize % 4 )
		allocsize += 4 - ( allocsize % 4 );

	last_write = stream->write;
	while( ( last_write < 0 ) || !atomic_cas32( &stream->write, EVENT_BLOCK_POSTING, last_write ) )
	{
		thread_yield();
		last_write = stream->write;
	}
	
	block = stream->block + last_write;

	if( ( block->used + allocsize + 2 ) >= block->capacity )
	{
		block->capacity <<= 1;
		block->capacity += allocsize;
		block->events = block->events ? memory_reallocate( block->events, block->capacity + 2ULL, 16 ) : memory_allocate( block->capacity + 2ULL, 16, MEMORY_PERSISTENT );
	}

	event = pointer_offset( block->events, block->used );
	block->used += allocsize;

	event->system    = systemid;
	event->id        = id;
	event->serial    = (uint16_t)( atomic_exchange_and_add32( &_event_serial, 1 ) & 0xFFFF );
	event->size      = allocsize;
	event->object    = object;

	if( size )
		memcpy( event->payload, payload, size );

	//Terminate with null system on next event
	((event_t*)pointer_offset( block->events, block->used ))->system = 0;

	restored_block = atomic_cas32( &stream->write, last_write, EVENT_BLOCK_POSTING );
	FOUNDATION_ASSERT( restored_block );
}


void event_post_delay( event_stream_t* stream, uint8_t systemid, uint8_t id, uint16_t size, uint64_t object, const void* payload, real delay )
{
	event_post_delay_ticks( stream, systemid, id, size, object, payload, (tick_t)( delay * (real)time_ticks_per_second() ) );
}


void event_post_delay_ticks( event_stream_t* stream, uint8_t systemid, uint8_t id, uint16_t size, uint64_t object, const void* payload, tick_t delay )
{
	if( !delay )
	{
		event_post( stream, systemid, id, size, object, payload );
		return;
	}

	FOUNDATION_ASSERT_FAIL( "Delayed events not implemented yet" );	
}


event_t* event_first( const event_block_t* block )
{
	return block && block->used ? block->events : 0;
}


event_t* event_next( event_t* event )
{
	event_t* next;
	if( !event )
		return 0;
	next = pointer_offset( event, event->size );
	if( !next->system )
		return 0; // End of event list
	return next;
}


event_stream_t* event_stream_allocate( unsigned int size )
{
	event_stream_t* stream = memory_allocate_zero( sizeof( event_stream_t ), 16, MEMORY_PERSISTENT );
	stream->write = 0;
	stream->read = 1;
	if( size )
	{
		stream->block[0].events = memory_allocate( size, 16, MEMORY_PERSISTENT );
		stream->block[1].events = memory_allocate( size, 16, MEMORY_PERSISTENT );

		stream->block[0].capacity = size;
		stream->block[1].capacity = size;
	}
	return stream;
}


void event_stream_deallocate( event_stream_t* stream )
{
	if( !stream )
		return;
	if( stream->block[0].events )
		memory_deallocate( stream->block[0].events );
	if( stream->block[1].events )
		memory_deallocate( stream->block[1].events );
	memory_deallocate( stream );
}


event_block_t* event_stream_process( event_stream_t* stream )
{
	event_block_t* block;
	bool restored_block;
	int32_t last_write, new_write;

	if( !stream )
		return 0;

	last_write = stream->write;
	while( ( last_write < 0 ) || !atomic_cas32( &stream->write, EVENT_BLOCK_SWAPPING, last_write ) )
	{
		thread_yield();
		last_write = stream->write;
	}

	new_write = stream->read;
	stream->read = last_write;

	block = stream->block + last_write;

	restored_block = atomic_cas32( &stream->write, new_write, EVENT_BLOCK_SWAPPING );
	FOUNDATION_ASSERT( restored_block );

	return block;
}
