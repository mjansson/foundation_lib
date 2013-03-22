/* ringbuffer.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file ringbuffer.h
    Simple ringbuffer abstraction */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Allocate ringbuffer of given size
    \param size                          Size in bytes
    \return                              Ringbuffer */
FOUNDATION_API ringbuffer_t*             ringbuffer_allocate( unsigned int size );

/*! Free ringbuffer memory
    \param buffer                        Ringbuffer */
FOUNDATION_API void                      ringbuffer_deallocate( ringbuffer_t* buffer );

/*! Get ringbuffer size
    \return                              Size of ringbuffer */
FOUNDATION_API unsigned int              ringbuffer_size( ringbuffer_t* size );

/*! Reset ringbuffer read/write pointers and statistics to initial state
    \param buffer                        Ringbuffer */
FOUNDATION_API void                      ringbuffer_reset( ringbuffer_t* buffer );

/*! Read from buffer
    \param buffer                        Ringbuffer
    \param dest                          Destination pointer
    \param num                           Number of bytes requested to be read
    \return                              Number of bytes actually read */
FOUNDATION_API unsigned int              ringbuffer_read( ringbuffer_t* buffer, void* dest, unsigned int num );

/*! Write to buffer
    \param buffer                        Ringbuffer
    \param source                        Source pointer
    \param num                           Number of bytes requested to be written
    \return                              Number of bytes actually written */
FOUNDATION_API unsigned int              ringbuffer_write( ringbuffer_t* buffer, const void* source, unsigned int num );

/*! Get total number of bytes read
    \param buffer                        Ringbuffer
    \return                              Total number of bytes read */
FOUNDATION_API uint64_t                  ringbuffer_total_read( ringbuffer_t* buffer );

/*! Get total number of bytes written
    \param buffer                        Ringbuffer
    \return                              Total number of bytes written */
FOUNDATION_API uint64_t                  ringbuffer_total_written( ringbuffer_t* buffer );

/*! Allocate a ringbuffer stream, which is basically a stream wrapped on top of a ringbuffer. Reads and writes
    block on semaphores on missing data, making it usable for ringbuffer threaded i/o
    \param buffer_size                   Size of ringbuffer
    \param total_size                    Total size of stream, 0 if infinite */
FOUNDATION_API stream_t*                 ringbuffer_stream_allocate( unsigned int buffer_size, uint64_t total_size );
