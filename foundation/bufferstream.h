/* bufferstream.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file bufferstream.h
\brief Stream for memory buffer

Stream for memory buffer, both statically sized and dynamically reallocated buffers. Streams are
not inherently thread safe, synchronization in a multithread use case must be done by caller.

Seeking in a buffer stream will not resize the storage buffer or change the current stream size.
To change stream size and allocate buffer space use #stream_truncate. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a new stream for memory buffers from an (optionally) existing buffer.
The buffer can grow up to the given capacity. If adopt and grow flags set it
will grow to any required size reallocating memory. The stream should be
deallocated with a call to #stream_deallocate.
\param buffer    Buffer
\param mode      Stream open mode
\param size      Current size of data
\param capacity  Buffer capacity
\param adopt     Take ownership of buffer
\param grow      Allow growing buffer by reallocating memory (requires buffer to be adopted)
\return          New stream */
FOUNDATION_API stream_t*
buffer_stream_allocate(void* buffer, unsigned int mode, size_t size, size_t capacity, bool adopt, bool grow);

/*! Initialize a new stream for memory buffers from an (optionally) existing buffer.
The buffer can grow up to the given capacity. If adopt and grow flags set it will
grow to any required size reallocating memory. The stream should be finalized with
a call to #stream_finalize
\param stream    Stream object
\param buffer    Buffer
\param mode      Stream open mode
\param size      Current size of data
\param capacity  Buffer capacity
\param adopt     Take ownership of buffer
\param grow      Allow growing buffer by reallocating memory (requires buffer to be adopted) */
FOUNDATION_API void
buffer_stream_initialize(stream_buffer_t* stream, void* buffer, unsigned int mode, size_t size, size_t capacity,
                         bool adopt, bool grow);
