/* bitbuffer.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file bitbuffer.h
    Bitbuffer I/O */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Setup a bitbuffer interface for a given memory buffer
    \param bitbuffer              Bitbuffer object
    \param buffer                 Memory buffer for I/O (must be 32-bit aligned)
    \param size                   Size of memory buffer (must be a multiple of 4 bytes)
    \param swap                   Byte order swap flag */
FOUNDATION_API void               bitbuffer_initialize_buffer( bitbuffer_t* bitbuffer, void* buffer, unsigned int size, bool swap );

/*! Setup a bitbuffer interface for a given stream
    \param bitbuffer              Bitbuffer object
    \param buffer                 Stream for I/O */
FOUNDATION_API void               bitbuffer_initialize_stream( bitbuffer_t* bitbuffer, stream_t* stream );

FOUNDATION_API uint32_t           bitbuffer_read32( bitbuffer_t* bitbuffer, unsigned int bits );
FOUNDATION_API void               bitbuffer_write32( bitbuffer_t* bitbuffer, uint32_t value, unsigned int bits );

FOUNDATION_API uint64_t           bitbuffer_read64( bitbuffer_t* bitbuffer, unsigned int bits );
FOUNDATION_API void               bitbuffer_write64( bitbuffer_t* bitbuffer, uint64_t value, unsigned int bits );

FOUNDATION_API uint128_t          bitbuffer_read128( bitbuffer_t* bitbuffer, unsigned int bits );
FOUNDATION_API void               bitbuffer_write128( bitbuffer_t* bitbuffer, uint128_t value, unsigned int bits );

FOUNDATION_API float32_t          bitbuffer_read_float32( bitbuffer_t* bitbuffer );
FOUNDATION_API void               bitbuffer_write_float32( bitbuffer_t* bitbuffer, float32_t value );

FOUNDATION_API float64_t          bitbuffer_read_float64( bitbuffer_t* bitbuffer );
FOUNDATION_API void               bitbuffer_write_float64( bitbuffer_t* bitbuffer, float64_t value );

/*! Align input to next even 32-bit boundary. Any remaining pending bit data is
    discarded, a new 32 bit chunk is read from the buffer and bit pointer is
    set to first bit in that chunk. If a full 32 bit chunk is available
    nothing is done, unless force flag is set in which case the chunk
    is discarded and a new chunk is read.
    \param bitbuffer              Bit buffer
    \param force                  Force reading new data even if full 32 bits are available */
FOUNDATION_API void               bitbuffer_align_read( bitbuffer_t* bitbuffer, bool force );

/*! Force output of the current write chunk to the buffer. Unwritten bits
    are padded with zero. If no pending data availble (no bits written since
    last chunk output), nothing is done unless force flag is set in which
    case a full zero chunk is written.
    \param bitbuffer              Bit buffer
    \param force                  Force writing data even if no bits are pending */
FOUNDATION_API void               bitbuffer_align_write( bitbuffer_t* bitbuffer, bool force );

/*! Discard pending read data. This will restore the bit pointer to the start
    of the current 32 bit chunk, effectively discarding the effect of previous
    reads on the chunk.
    \param bitbuffer              Bit buffer */
FOUNDATION_API void               bitbuffer_discard_read( bitbuffer_t* bitbuffer );

/*! Discard pending write data
    \param bitbuffer              Bit buffer */
FOUNDATION_API void               bitbuffer_discard_write( bitbuffer_t* bitbuffer );

