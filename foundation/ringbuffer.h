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
\brief Memory ring buffer
\details Simple memory ring buffer abstraction. Read and write are not
thread safe. Synchronization needs to be done by caller. The ring buffer
stream uses a semaphore to block and notify readers. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Allocate ring buffer
Allocate ring buffer of given size. Deallocate the ring buffer with a call
to #ringbuffer_deallocate.
\param size Size in bytes
\return Ring buffer */
FOUNDATION_API ringbuffer_t*
ringbuffer_allocate(size_t size);

/*! \brief Deallocate ring buffer
Deallocate ring buffer previously allocated with a call to #ringbuffer_allocate.
\param buffer Ring buffer */
FOUNDATION_API void
ringbuffer_deallocate(ringbuffer_t* buffer);

/*! \brief Initialize ring buffer
Initialize ring buffer of given size. Finalize the ring buffer with a call
to #ringbuffer_finalize.
\param buffer Ring buffer
\param size Size in bytes */
FOUNDATION_API void
ringbuffer_initialize(ringbuffer_t* buffer, size_t size);

/*! \brief Finalize ring buffer
Finalize ring buffer previously initialized with a call to #ringbuffer_finalize.
\param buffer Ring buffer */
FOUNDATION_API void
ringbuffer_finalize(ringbuffer_t* buffer);

/*! \brief Get ring buffer size
Get ring buffer size.
\param buffer Ring buffer
\return Size of ring buffer */
FOUNDATION_API size_t
ringbuffer_size(ringbuffer_t* buffer);

/*! \brief Reset ring buffer
Reset ring buffer read/write pointers and statistics to initial state
\param buffer Ring buffer */
FOUNDATION_API void
ringbuffer_reset(ringbuffer_t* buffer);

/*! \brief Read from buffer
Read from buffer.
\param buffer Ring buffer
\param dest Destination pointer
\param num Number of bytes requested to be read
\return Number of bytes actually read */
FOUNDATION_API size_t
ringbuffer_read(ringbuffer_t* buffer, void* dest, size_t num);

/*! \brief Write to buffer
Write to buffer.
\param buffer Ring buffer
\param source Source pointer
\param num Number of bytes requested to be written
\return Number of bytes actually written */
FOUNDATION_API size_t
ringbuffer_write(ringbuffer_t* buffer, const void* source, size_t num);

/*! \brief Get total bytes read
Get total number of bytes read. Will be reset to zero in #ringbuffer_reset
\param buffer Ring buffer
\return Total number of bytes read */
FOUNDATION_API uint64_t
ringbuffer_total_read(ringbuffer_t* buffer);

/*! \brief Get total bytes written
Get total number of bytes written. Will be reset to zero in #ringbuffer_reset
\param buffer Ring buffer
\return Total number of bytes written */
FOUNDATION_API uint64_t
ringbuffer_total_written(ringbuffer_t* buffer);

/*! \brief Allocate ring buffer stream
Allocate a ringbuffer stream, which is basically a stream wrapped on top of a ringbuffer.
Reads and writes block on semaphores on missing data, making it usable for ringbuffer
threaded I/O. Stream should be deallocated by a call to #stream_deallocate
\param buffer_size Size of ringbuffer
\param total_size Total size of stream, 0 if infinite
\return Ringbuffer stream */
FOUNDATION_API stream_t*
ringbuffer_stream_allocate(size_t buffer_size, size_t total_size);

/*! \brief Initialize ring buffer stream
Initialize a ringbuffer stream, which is basically a stream wrapped on top of a ringbuffer.
Reads and writes block on semaphores on missing data, making it usable for ringbuffer
threaded I/O. Stream should be finalized by a call to #stream_finalize
\param stream Ringbuffer stream
\param buffer_size Size of ringbuffer
\param total_size Total size of stream, 0 if infinite */
FOUNDATION_API void
ringbuffer_stream_initialize(stream_ringbuffer_t* stream, size_t buffer_size, size_t total_size);
