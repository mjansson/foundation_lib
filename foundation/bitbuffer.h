/* bitbuffer.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file bitbuffer.h
\brief Bit buffer I/O

Bit buffer I/O for reading and writing tightly packed bits from/to a memory buffer or a stream.
The bit buffers support reading and writing integers up to 128 bits and floats up to 64 bits
in one operation.

The bit buffer is not inherently thread safe, synchronization must be done by caller in a
multithreaded used case. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a bitbuffer interface for a given memory buffer
\param buffer  Memory buffer for I/O (must be 32-bit aligned)
\param size    Size of memory buffer (must be a multiple of 4 bytes)
\param swap    Byte order swap flag
\return        Bit buffer object */
FOUNDATION_API bitbuffer_t*
bitbuffer_allocate_buffer(void* buffer, size_t size, bool swap);

/*! Allocate a bitbuffer interface for a given stream
\param stream  Stream for I/O
\return        Bit buffer object */
FOUNDATION_API bitbuffer_t*
bitbuffer_allocate_stream(stream_t* stream);

/*! Deallocate bit buffer and free resources
\param bitbuffer  Bit buffer object */
FOUNDATION_API void
bitbuffer_deallocate(bitbuffer_t* bitbuffer);

/*! Initialize a bitbuffer interface for a given memory buffer
\param bitbuffer  Bit buffer object
\param buffer     Memory buffer for I/O (must be 32-bit aligned)
\param size       Size of memory buffer (must be a multiple of 4 bytes)
\param swap       Byte order swap flag */
FOUNDATION_API void
bitbuffer_initialize_buffer(bitbuffer_t* bitbuffer, void* buffer, size_t size, bool swap);

/*! Initialize a bitbuffer interface for a given stream
\param bitbuffer  Bit buffer object
\param stream     Stream for I/O */
FOUNDATION_API void
bitbuffer_initialize_stream(bitbuffer_t* bitbuffer, stream_t* stream);

/*! Finalize bit buffer and free resources
\param bitbuffer  Bit buffer object */
FOUNDATION_API void
bitbuffer_finalize(bitbuffer_t* bitbuffer);

/*! Read up to 32 bits of data as an integer
\param bitbuffer  Bit buffer object
\param bits       Number of bits to read (max 32)
\return           Data read */
FOUNDATION_API uint32_t
bitbuffer_read32(bitbuffer_t* bitbuffer, unsigned int bits);

/*! Read up to 64 bits of data as an integer
\param bitbuffer  Bit buffer object
\param bits       Number of bits to read (max 64)
\return           Data read */
FOUNDATION_API uint64_t
bitbuffer_read64(bitbuffer_t* bitbuffer, unsigned int bits);

/*! Read up to 128 bits of data as an integer
\param bitbuffer  Bit buffer object
\param bits       Number of bits to read (max 128)
\return           Data read */
FOUNDATION_API uint128_t
bitbuffer_read128(bitbuffer_t* bitbuffer, unsigned int bits);

/*! Read 32 bits of data as a float
\param bitbuffer  Bit buffer object
\return           Data read */
FOUNDATION_API float32_t
bitbuffer_read_float32(bitbuffer_t* bitbuffer);

/*! Read 64 bits of data as a float
\param bitbuffer  Bit buffer object
\return           Data read */
FOUNDATION_API float64_t
bitbuffer_read_float64(bitbuffer_t* bitbuffer);

/*! Write up to 32 bits of integer data
\param bitbuffer  Bit buffer object
\param value      Data to write
\param bits       Number of bits to write (max 32) */
FOUNDATION_API void
bitbuffer_write32(bitbuffer_t* bitbuffer, uint32_t value, unsigned int bits);

/*! Write up to 64 bits of integer data
\param bitbuffer  Bit buffer object
\param value      Data to write
\param bits       Number of bits to write (max 64) */
FOUNDATION_API void
bitbuffer_write64(bitbuffer_t* bitbuffer, uint64_t value, unsigned int bits);

/*! Write up to 128 bits of integer data
\param bitbuffer  Bit buffer object
\param value      Data to write
\param bits       Number of bits to write (max 128) */
FOUNDATION_API void
bitbuffer_write128(bitbuffer_t* bitbuffer, uint128_t value, unsigned int bits);

/*! Write 32 bits of float data
\param bitbuffer  Bit buffer object
\param value      Data to write */
FOUNDATION_API void
bitbuffer_write_float32(bitbuffer_t* bitbuffer, float32_t value);

/*! Write 64 bits of float data
\param bitbuffer  Bit buffer object
\param value      Data to write */
FOUNDATION_API void
bitbuffer_write_float64(bitbuffer_t* bitbuffer, float64_t value);

/*! Align input to next even 32-bit boundary. Any remaining pending bit data is discarded,
a new 32 bit chunk is read from the buffer and bit pointer is set to first bit in that
chunk. If a full 32 bit chunk is available nothing is done, unless force flag is set
in which case the chunk is discarded and a new chunk is read.
\param bitbuffer  Bit buffer
\param force      Force reading new data even if full 32 bits are available */
FOUNDATION_API void
bitbuffer_align_read(bitbuffer_t* bitbuffer, bool force);

/*! Force output of the current write chunk to the buffer. Unwritten bits are padded with
zero. If no pending data availble (no bits written since last chunk output), nothing
is done unless force flag is set in which case a full zero chunk is written.
\param bitbuffer  Bit buffer
\param force      Force writing data even if no bits are pending */
FOUNDATION_API void
bitbuffer_align_write(bitbuffer_t* bitbuffer, bool force);

/*! Discard pending read data. This will restore the bit pointer to the start of the
current 32 bit chunk, effectively discarding the effect of previous reads on the
32 bit chunk and allowing a re-read of the data.
\param bitbuffer  Bit buffer */
FOUNDATION_API void
bitbuffer_discard_read(bitbuffer_t* bitbuffer);

/*! Discard pending write data. This will restore the bit pointer to the start of the
current 32 bit chunk, effectively discaring any previous writes of the 32 bit chunk
and allowing a re-write of the data.
\param bitbuffer  Bit buffer */
FOUNDATION_API void
bitbuffer_discard_write(bitbuffer_t* bitbuffer);
