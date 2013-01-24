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

#include <platform.h>
#include <types.h>


/*! Initialize an event during posting
    \param stream                   Event stream
    \param system                   System identifier
    \param id                       Event id
    \param size                     Event payload size
    \param object                   Sender
    \param payload                  Event payload */
FOUNDATION_API void                 event_post( event_stream_t* stream, uint8_t system, uint8_t id, uint16_t size, object_t object, const void* payload );

/*! Initialize an event during posting
    \param stream                   Event stream
    \param system                   System identifier
    \param id                       Event id
    \param size                     Event size
    \param object                   Sender
    \param payload                  Event payload
    \param delay                    Delay in seconds */
FOUNDATION_API void                 event_post_delay( event_stream_t* stream, uint8_t system, uint8_t id, uint16_t size, object_t object, const void* payload, real delay );

/*! Initialize an event during posting
    \param stream                   Event stream
    \param system                   System identifier
    \param id                       Event id
    \param size                     Event size
    \param object                   Sender
    \param payload                  Event payload
    \param delay                    Delay in ticks */
FOUNDATION_API void                 event_post_delay_ticks( event_stream_t* stream, uint8_t system, uint8_t id, uint16_t size, object_t object, const void* payload, time_t delay );

/*! Grab first event for processing
    \param block                    Event block
    \return                         First event */
FOUNDATION_API event_t*             event_first( const event_block_t* block );

/*! Grab next event during procesing
    \param event                    Previous event
    \return                         Next event */
FOUNDATION_API event_t*             event_next( event_t* event );

/*! Allocate an event stream
    \param size                     Initial size of stream buffer
    \return                         Event stream */
FOUNDATION_API event_stream_t*      event_stream_allocate( unsigned int size );

/*! Deallocate an event stream
    \param stream                   Event stream */
FOUNDATION_API void                 event_stream_deallocate( event_stream_t* stream );

/*! Grab event block for processing
    \param stream                   Event stream
    \return                         Event block for processing */
FOUNDATION_API event_block_t*       event_stream_process( event_stream_t* stream );
