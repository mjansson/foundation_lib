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
    Stream for unnamed pipes */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Create an unnamed pipe stream */
FOUNDATION_API stream_t*             pipe_allocate( void );

/*! Close read end of pipe */
FOUNDATION_API void                  pipe_close_read( stream_t* pipe );

/*! Close write end of pipe */
FOUNDATION_API void                  pipe_close_write( stream_t* pipe );


#if FOUNDATION_PLATFORM_WINDOWS

FOUNDATION_API void*                 pipe_read_handle( stream_t* pipe );
FOUNDATION_API void*                 pipe_write_handle( stream_t* pipe );

#elif FOUNDATION_PLATFORM_POSIX

FOUNDATION_API int                   pipe_read_fd( stream_t* pipe );
FOUNDATION_API int                   pipe_write_fd( stream_t* pipe );

#endif
