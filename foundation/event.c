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


#define EVENT_BLOCK_POSTING  -1
#define EVENT_BLOCK_SWAPPING -2

struct _foundation_event_block
{
	volatile uint32_t                used;
	uint32_t                         capacity;
	event_stream_t*                  stream;
	event_t*                         events;
};

struct ALIGN(16) _foundation_event_stream
{
	volatile int32_t                 write;
	volatile int32_t                 read;
	event_block_t                    block[2];
};

static int32_t _event_serial = 1;


static void _event_post_delay_with_flag( event_stream_t* stream, uint8_t systemid, uint8_t id, uint16_t size, uint64_t object, const void* payload, uint16_t flags, uint64_t timestamp )
{
	event_block_t* block;
	event_t* event;
	bool restored_block;
	uint32_t basesize;
	uint32_t allocsize;
	int32_t last_write;

	//Events must be aligned to an even 8 bytes
	basesize = sizeof( event_t ) + size;
	if( basesize % 8 )
		basesize += 8 - ( basesize % 8 );

	//Delayed events have extra 8 bytes payload to hold timestamp
	allocsize = basesize;
	if( timestamp )
		allocsize += 8;

	//Lock the event block by atomic swapping the write block index
	last_write = stream->write;
	while( ( last_write < 0 ) || !atomic_cas32( &stream->write, EVENT_BLOCK_POSTING, last_write ) )
	{
		thread_yield();
		last_write = stream->write;
	}
	
	//We now have exclusive access to the event block
	block = stream->block + last_write;

	if( ( block->used + allocsize + 2 ) >= block->capacity )
	{
#define BLOCK_CHUNK_SIZE ( 32 * 1024 )
		if( block->capacity < BLOCK_CHUNK_SIZE )
		{
			block->capacity <<= 1;
			block->capacity += allocsize;
		}
		else
		{
			block->capacity += BLOCK_CHUNK_SIZE;
			FOUNDATION_ASSERT_MSG( block->capacity < BUILD_SIZE_EVENT_BLOCK_LIMIT, "Event stream block size > 4Mb" );
			error_report( ERRORLEVEL_ERROR, ERROR_OUT_OF_MEMORY );
		}
		if( block->capacity % 16 )
			block->capacity += 16 - ( basesize % 16 );			
		block->events = block->events ? memory_reallocate( block->events, block->capacity + 2ULL, 16 ) : memory_allocate( block->capacity + 2ULL, 16, MEMORY_PERSISTENT );
	}

	event = pointer_offset( block->events, block->used );

	event->system    = systemid;
	event->id        = id;
	event->serial    = (uint16_t)( atomic_exchange_and_add32( &_event_serial, 1 ) & 0xFFFF );
	event->size      = allocsize;
	event->flags     = 0;
	event->object    = object;

	if( size )
		memcpy( event->payload, payload, size );

	if( timestamp )
	{
		event->flags |= EVENTFLAG_DELAY;
		*(uint64_t*)pointer_offset( event, basesize ) = timestamp;
	}

	//Terminate with null system on next event
	block->used += allocsize;
	((event_t*)pointer_offset( block->events, block->used ))->system = 0;

	//Now unlock the event block
	restored_block = atomic_cas32( &stream->write, last_write, EVENT_BLOCK_POSTING );
	FOUNDATION_ASSERT( restored_block );
}


uint16_t event_payload_size( const event_t* event )
{
	uint16_t size = event->size - sizeof( event_t );
	if( event->flags & EVENTFLAG_DELAY )
		size -= 8;
	return size;
}


void event_post( event_stream_t* stream, uint8_t systemid, uint8_t id, uint16_t size, uint64_t object, const void* payload, tick_t delivery )
{
	_event_post_delay_with_flag( stream, systemid, id, size, object, payload, 0, delivery );
}


event_t* event_next( const event_block_t* block, event_t* event )
{
	uint64_t curtime = 0;
	uint64_t eventtime;

	do
	{
		//Grab first event if no previous event, or grab next event
		event = ( event ? pointer_offset( event, event->size ) : ( block && block->used ? block->events : 0 ) );
		if( !event || !event->system )
			return 0; // End of event list

		if( !( event->flags & EVENTFLAG_DELAY ) )
			return event;

		if( !curtime )
			curtime = time_current();

		eventtime = *(uint64_t*)pointer_offset( event, event->size - 8 );
		if( eventtime < curtime )
			return event;

		//Re-post to next block
		_event_post_delay_with_flag( block->stream, event->system, event->id, event->size - ( sizeof( event_t ) + 8 ), event->object, event->payload, event->flags, eventtime );
	} while( true );

	return 0;
}


event_stream_t* event_stream_allocate( unsigned int size )
{
	event_stream_t* stream = memory_allocate_zero( sizeof( event_stream_t ), 16, MEMORY_PERSISTENT );
	stream->write = 0;
	stream->read = 1;

	if( size < 256 )
		size = 256;
	
	stream->block[0].events = memory_allocate( size, 16, MEMORY_PERSISTENT );
	stream->block[1].events = memory_allocate( size, 16, MEMORY_PERSISTENT );

	stream->block[0].capacity = size;
	stream->block[1].capacity = size;

	stream->block[0].stream = stream;
	stream->block[1].stream = stream;

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

	//Lock the write event block by atomic swapping the write block index
	last_write = stream->write;
	while( ( last_write < 0 ) || !atomic_cas32( &stream->write, EVENT_BLOCK_SWAPPING, last_write ) )
	{
		thread_yield();
		last_write = stream->write;
	}

	//Reset used on last read (safe, since read can only happen on one thread)
	stream->block[ stream->read ].used = 0;

	//Swap blocks
	new_write = stream->read;
	stream->read = last_write;

	block = stream->block + last_write;

	//Unlock write event block
	restored_block = atomic_cas32( &stream->write, new_write, EVENT_BLOCK_SWAPPING );
	FOUNDATION_ASSERT( restored_block );

	return block;
}
