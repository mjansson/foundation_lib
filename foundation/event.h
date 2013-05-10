/* event.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file event.h
    Base system for event sending and processing */

#include <foundation/platform.h>
#include <foundation/types.h>

//
//Double-buffered event streams with a lock-free structure of many-writers, single-reader and yield-spinning
//over an atomic operation while waiting for other threads to finish writing or swap-for-read. No locks needed
//as only write block indicator needs to be protected (which can be done with atomic operations).
//
//The expected contention for writing is low as the write is a quick operation and number
//of events posted to the same stream from different threads is very low
//
//Current buffer used to writing events is swapped during the event_stream_process call, allowing new events
//to be posted during the event process loop (which will then be delivered and processed during the next
//event process loop)
//


/*! Post event to stream
    \param stream                   Event stream
    \param system                   System identifier
    \param id                       Event id
    \param size                     Event payload size
    \param object                   Sender
    \param payload                  Event payload
	\param delivery                 Delivery time, 0 for immediate delivery */
FOUNDATION_API void                 event_post( event_stream_t* stream, uint8_t system, uint8_t id, uint16_t size, object_t object, const void* payload, tick_t delivery );

/*! Grab next event during procesing
    \param event                    Previous event, pass in 0 for getting first event
    \return                         Next event */
FOUNDATION_API event_t*             event_next( const event_block_t* block, event_t* event );

/*! Get event actual payload size (size field in event struct may be padded and extended for internal data)
    \param event                    Event
	\return                         Payload in bytes */
FOUNDATION_API uint16_t             event_payload_size( const event_t* event );

/*! Allocate an event stream
    \param size                     Initial size of stream buffer
    \return                         Event stream */
FOUNDATION_API event_stream_t*      event_stream_allocate( unsigned int size );

/*! Deallocate an event stream
    \param stream                   Event stream */
FOUNDATION_API void                 event_stream_deallocate( event_stream_t* stream );

/*! Grab event block for processing. Processing for an event stream must only occur on one single thread at any
    given moment (not thread-safe).
    \param stream                   Event stream
    \return                         Event block for processing */
FOUNDATION_API event_block_t*       event_stream_process( event_stream_t* stream );

