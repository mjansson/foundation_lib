/* event.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

/*! \file event.h
\brief Events posting and processing

Base system for event posting and processing.

Double-buffered event streams with a lock-free structure of many-writers, single-reader and
yield-spinning over an atomic operation while waiting for other threads to finish writing or
swap-for-read. No locks needed as only write block indicator needs to be protected (which
can be done with atomic operations).

The expected contention for writing is low as the write is a quick operation and number
of events posted to the same stream from different threads is very low.

Current buffer used to writing events is swapped during the event_stream_process call,
allowing new events to be posted during the event process loop (which will then be delivered
and processed during the next event process loop).

Delayed events will not be delivered for processing until the delivery timestamp has passed.
Delivery is not guaranteed until next pass of <code>event_stream_process</code> and
<code>event_next</code> iteration.

Event posting is thread safe. Event processing is not thread safe and must be contained
to a single thread. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Post event to stream. This operation is thread-safe and will spin loop until operation
can be completed if in contention with another thread.
\param stream    Event stream
\param id        Event id
\param object    Sender
\param delivery  Delivery time, 0 for immediate delivery
\param payload   Event payload
\param size      Event payload size */
FOUNDATION_API void
event_post(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size);

/*! Post event to stream with multiple payload fragments. Each fragment must be a
pointer and size pair of arguments, and terminated with a null pointer sentinel argument.
This operation is thread-safe and will spin loop until operation can be completed if in
contention with another thread.
\param stream    Event stream
\param id        Event id
\param object    Sender
\param delivery  Delivery time, 0 for immediate delivery
\param payload   Event payload
\param size      Event payload size */
FOUNDATION_API void
event_post_varg(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size, ...)
    FOUNDATION_ATTRIBUTE(sentinel);

/*! Post event to stream with multiple payload fragments. Each fragment must be a
pointer and size pair of arguments, and terminated with a null pointer sentinel argument.
This operation is thread-safe and will spin loop until operation can be completed if in
contention with another thread.
\param stream    Event stream
\param id        Event id
\param object    Sender
\param delivery  Delivery time, 0 for immediate delivery
\param payload   Event payload
\param size      Event payload size
\param list      Payload fragment argument list */
FOUNDATION_API void
event_post_vlist(event_stream_t* stream, int id, object_t object, tick_t delivery, const void* payload, size_t size,
                 va_list list);

/*! Get next event during procesing
\param block Event block
\param event Previous event, pass in 0 for getting first event
\return      Next event */
FOUNDATION_API event_t*
event_next(const event_block_t* block, event_t* event);

/*! Get event actual payload size (size field in event struct may be padded and extended
for internal data)
\param event Event
\return      Payload in bytes */
FOUNDATION_API size_t
event_payload_size(const event_t* event);

/*! Allocate an event stream. Use #event_stream_deallocate to deallocate the returned stream.
\param size Initial size of stream buffer
\return     Event stream */
FOUNDATION_API event_stream_t*
event_stream_allocate(size_t size);

/*! Deallocate an event stream which was allocated by #event_stream_allocate
\param stream Event stream */
FOUNDATION_API void
event_stream_deallocate(event_stream_t* stream);

/*! Initialize an event stream. Use #event_stream_finalize to finalize the returned stream.
\param stream Event stream
\param size   Initial size of stream buffer */
FOUNDATION_API void
event_stream_initialize(event_stream_t* stream, size_t size);

/*! Finalize an event stream which was initialized by #event_stream_initialize
and free resources.
\param stream Event stream */
FOUNDATION_API void
event_stream_finalize(event_stream_t* stream);

/*! Get event block for processing. Processing for an event stream must only occur on one
single thread at any given moment (not thread-safe).
\param stream Event stream
\return       Event block for processing */
FOUNDATION_API event_block_t*
event_stream_process(event_stream_t* stream);

/*! Set beacon to fire when an event is posted in the pending event block
\param stream Event stream
\param beacon Beacon to fire */
FOUNDATION_API void
event_stream_set_beacon(event_stream_t* stream, beacon_t* beacon);
