/* pipe.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file pipe.h
\brief Unnamed pipe stream
\details Stream for unnamed pipes, usable for inter-process communication. Pipe
read/write calls are blocking. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Allocate unnamed pipe stream
Allocate an unnamed pipe stream. Deallocate the stream with a call to #stream_deallocate
\return New pipe stream */
FOUNDATION_API stream_t*
pipe_allocate(void);

/*! \brief Initialize unnamed pipe stream
Initialize an unnamed pipe stream. Finalize the stream with a call to #stream_finalize
\param stream Pipe stream */
FOUNDATION_API void
pipe_initialize(stream_pipe_t* pipe);

/*! \brief Close read end
Close read end of pipe
\param pipe Pipe stream */
FOUNDATION_API void
pipe_close_read(stream_t* pipe);

/*! \brief Close write end
Close write end of pipe
\param pipe Pipe stream */
FOUNDATION_API void
pipe_close_write(stream_t* pipe);

#if FOUNDATION_PLATFORM_WINDOWS

/*! \brief Get read handle
Windows only, get OS handle for read end of pipe
\param pipe Pipe stream
\return Read object handle */
FOUNDATION_API void*
pipe_read_handle(stream_t* pipe);

/*! \brief Get write handle
Windows only, get OS handle for write end of pipe
\param pipe Pipe stream
\return Write object handle */
FOUNDATION_API void*
pipe_write_handle(stream_t* pipe);

#endif

#if FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL

/*! \brief Get read file descriptor
Posix only, get OS file descriptor for read end of pipe
\param pipe Pipe stream
\return Read file descriptor */
FOUNDATION_API int
pipe_read_fd(stream_t* pipe);

/*! \brief Get write file descriptor
Posix only, get OS file descriptor for write end of pipe
\param pipe Pipe stream
\return Write file descriptor */
FOUNDATION_API int
pipe_write_fd(stream_t* pipe);

#endif
