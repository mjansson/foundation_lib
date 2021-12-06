/* event.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

#define EVENT_BLOCK_POSTING -1
#define EVENT_BLOCK_SWAPPING -2

static atomic32_t event_serial;

static void
event_post_delay_with_flags(event_stream_t* stream, int id, object_t object, tick_t timestamp, uint16_t flags,
                            const void* payload, size_t size, va_list list) {
	event_block_t* block;
	event_t* event;
	size_t basesize;
	size_t allocsize;
	int32_t last_write;
	char* part;
	void* ptr;
	size_t psize;
	va_list clist;

	// Events must have non-zero id
	FOUNDATION_ASSERT_MSG(id, "Events must have non-zero id");
	FOUNDATION_ASSERT_MSGFORMAT(size < 0xFFFF - 16, "Events size must be less than %d", 0xFFFF - 16);
	if (!id)
		return;

	// Events must be aligned to an even 8 bytes
	basesize = sizeof(event_t) + size;
	va_copy(clist, list);
	while ((ptr = va_arg(clist, void*))) {
		psize = va_arg(clist, size_t);
		basesize += psize;
	}
	va_end(clist);

	if (basesize % 8)
		basesize += 8 - (basesize % 8);
	basesize &= 0xFFF8;

	// Delayed events have extra 8 bytes payload to hold timestamp
	allocsize = basesize;
	if (timestamp)
		allocsize += 8;

	// Lock the event block by atomic swapping the write block index
	last_write = atomic_load32(&stream->write, memory_order_acquire);
	while ((last_write < 0) ||
	       !atomic_cas32(&stream->write, EVENT_BLOCK_POSTING, last_write, memory_order_release, memory_order_acquire)) {
		thread_yield();
		last_write = atomic_load32(&stream->write, memory_order_acquire);
	}

	// We now have exclusive access to the event block
	block = stream->block + last_write;

	if ((block->used + allocsize + 2) >= block->capacity) {
		size_t prev_capacity = block->capacity + 16;
		if (prev_capacity < foundation_config().event_block_chunk) {
			block->capacity = foundation_config().event_block_chunk;
		} else {
			if (prev_capacity >= foundation_config().event_block_limit) {
				FOUNDATION_ASSERT_FAILFORMAT_LOG(0, "Event block size over limit of %u bytes",
				                                 (unsigned int)foundation_config().event_block_limit);
				error_report(ERRORLEVEL_ERROR, ERROR_OUT_OF_MEMORY);
				goto unlock;
			}
			block->capacity += foundation_config().event_block_chunk;
			if (block->capacity > foundation_config().event_block_limit)
				block->capacity = foundation_config().event_block_limit;
		}
		if (block->capacity % 16)
			block->capacity += 16 - (basesize % 16);
		block->capacity -= 16;
		block->events = block->events ? memory_reallocate(block->events, block->capacity + 16, 16, prev_capacity, 0) :
                                        memory_allocate(0, block->capacity + 16, 16, MEMORY_PERSISTENT);
	}

	event = pointer_offset(block->events, block->used);

	event->id = (uint16_t)id;
	event->serial = (uint16_t)(atomic_exchange_and_add32(&event_serial, 1, memory_order_relaxed) & 0xFFFF);
	event->size = (uint16_t)allocsize;
	event->flags = flags;
	event->object = object;

	part = (void*)&event->payload[0];
	if (size) {
		memcpy(part, payload, size);
		part += size;
	}
	va_copy(clist, list);
	while ((ptr = va_arg(clist, void*))) {
		psize = va_arg(clist, size_t);
		if (psize) {
			memcpy(part, ptr, psize);
			part += psize;
		}
	}
	va_end(clist);

	if (timestamp) {
		event->flags |= EVENTFLAG_DELAY;
		*(tick_t*)pointer_offset(event, basesize) = timestamp;
	}

	// Terminate with null id on next event
	block->used += allocsize;
	((event_t*)pointer_offset(block->events, block->used))->id = 0;

	// Re-fire beacon
	if (!block->fired && stream->beacon) {
		beacon_fire(stream->beacon);
		block->fired = true;
	}

unlock:
	// Now unlock the event block
	atomic_store32(&stream->write, last_write, memory_order_release);
}

size_t
event_payload_size(const event_t* event) {
	size_t size = event->size;
	size -= sizeof(event_t);
	if (event->flags & EVENTFLAG_DELAY)
		size -= 8;
	return size;
}

void
event_post(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size) {
	event_post_varg(stream, id, object, delivery, payload, size, nullptr);
}

void
event_post_varg(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size,
                ...) {
	va_list list;
	va_start(list, size);
	event_post_delay_with_flags(stream, id, object, delivery, 0, payload, size, list);
	va_end(list);
}

void
event_post_vlist(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size,
                 va_list list) {
	event_post_delay_with_flags(stream, id, object, delivery, 0, payload, size, list);
}

static void
event_post_varg_flags(event_stream_t* stream, int id, object_t object, tick_t delivery, uint16_t flags,
                      const void* payload, size_t size, ...) {
	va_list list;
	va_start(list, size);
	event_post_delay_with_flags(stream, id, object, delivery, flags, payload, size, list);
	va_end(list);
}

event_t*
event_next(const event_block_t* block, event_t* event) {
	tick_t curtime = 0;
	tick_t eventtime;

	while (block) {
		// Grab first event if no previous event, or grab next event
		event = (event ? pointer_offset(event, event->size) : (block && block->used ? block->events : 0));
		if (!event || !event->id)
			break;  // End of event list

		if (!(event->flags & EVENTFLAG_DELAY))
			return event;

		if (!curtime)
			curtime = time_current();

		eventtime = *(tick_t*)pointer_offset(event, event->size - 8);
		if (eventtime <= curtime)
			return event;

		// Re-post to next block
		event_post_varg_flags(block->stream, event->id, event->object, eventtime, event->flags, event->payload,
		                      event->size - (sizeof(event_t) + 8), nullptr);
	}

	return nullptr;
}

event_stream_t*
event_stream_allocate(size_t size) {
	event_stream_t* stream = memory_allocate(0, sizeof(event_stream_t), 16, MEMORY_PERSISTENT);

	event_stream_initialize(stream, size);

	return stream;
}

void
event_stream_initialize(event_stream_t* stream, size_t size) {
	atomic_store32(&stream->write, 0, memory_order_release);
	stream->read = 1;
	stream->beacon = nullptr;

	if (size < 256)
		size = 256;

	stream->block[0].events = memory_allocate(0, size, 16, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream->block[1].events = memory_allocate(0, size, 16, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	stream->block[0].used = 0;
	stream->block[1].used = 0;

	stream->block[0].capacity = size;
	stream->block[1].capacity = size;

	stream->block[0].stream = stream;
	stream->block[1].stream = stream;

	stream->block[0].fired = false;
	stream->block[1].fired = false;
}

void
event_stream_deallocate(event_stream_t* stream) {
	if (!stream)
		return;
	event_stream_finalize(stream);
	memory_deallocate(stream);
}

void
event_stream_finalize(event_stream_t* stream) {
	if (stream->block[0].events)
		memory_deallocate(stream->block[0].events);
	if (stream->block[1].events)
		memory_deallocate(stream->block[1].events);
}

event_block_t*
event_stream_process(event_stream_t* stream) {
	event_block_t* block;
	int32_t last_write, new_write;

	if (!stream)
		return 0;

	// Lock the write event block by atomic swapping the write block index
	last_write = atomic_load32(&stream->write, memory_order_acquire);
	while ((last_write < 0) || !atomic_cas32(&stream->write, EVENT_BLOCK_SWAPPING, last_write, memory_order_release,
	                                         memory_order_acquire)) {
		thread_yield();
		last_write = atomic_load32(&stream->write, memory_order_acquire);
	}

	// Reset used on last read (safe, since read can only happen on one thread)
	stream->block[stream->read].used = 0;
	stream->block[stream->read].fired = false;

	// Swap blocks
	new_write = stream->read;
	stream->read = last_write;

	block = stream->block + last_write;

	// Unlock write event block
	atomic_store32(&stream->write, new_write, memory_order_release);

	return block;
}

void
event_stream_set_beacon(event_stream_t* stream, beacon_t* beacon) {
	stream->beacon = beacon;
	int32_t current = atomic_load32(&stream->write, memory_order_acquire);
	if (beacon && (stream->block[current].used > 0))
		beacon_fire(beacon);
}
