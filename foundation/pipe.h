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
    Stream for unnamed pipes, usable for inter-process communication. Pipe
    read/write calls are blocking. */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Create an unnamed pipe stream
    \return                          New pipe stream */
FOUNDATION_API stream_t*             pipe_allocate( void );

/*! Close read end of pipe
    \param pipe                      Pipe stream */
FOUNDATION_API void                  pipe_close_read( stream_t* pipe );

/*! Close write end of pipe
    \param pipe                      Pip stream */
FOUNDATION_API void                  pipe_close_write( stream_t* pipe );


#if FOUNDATION_PLATFORM_WINDOWS

/*! Windows only, get OS handle for read end of pipe
    \param mutex                     Pipe stream
    \return                          Read object handle */
FOUNDATION_API void*                 pipe_read_handle( stream_t* pipe );

/*! Windows only, get OS handle for write end of pipe
    \param mutex                     Pipe stream
    \return                          Write object handle */
FOUNDATION_API void*                 pipe_write_handle( stream_t* pipe );

#elif FOUNDATION_PLATFORM_POSIX

/*! Posix only, get OS file descriptor for read end of pipe
    \param mutex                     Pipe stream
    \return                          Read file descriptor */
FOUNDATION_API int                   pipe_read_fd( stream_t* pipe );

/*! Posix only, get OS file descriptor for write end of pipe
    \param mutex                     Pipe stream
    \return                          Write file descriptor */
FOUNDATION_API int                   pipe_write_fd( stream_t* pipe );

#endif
