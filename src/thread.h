/* thread.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file thread.h
    Thread management */

#include <platform.h>
#include <types.h>


/*! Sleep thread for specified time
    \param milliseconds        Milliseconds to sleep */
FOUNDATION_API void            thread_sleep( int milliseconds );

/*! Yield timeslice to other threads */
FOUNDATION_API void            thread_yield( void );

/*! Get current thread system id
    \return                    Thread id */
FOUNDATION_API uint64_t        thread_id( void );

/*! Get the hardware thread the current thread is running on
    \return                    Hardware thread id */
FOUNDATION_API unsigned int    thread_hardware( void );

/*! Set thread affinity
    \param hw_thread           Hardware thread id */
FOUNDATION_API void            thread_set_hardware( unsigned int hw_thread );

/*! Cleanup on thread exit */
FOUNDATION_API void            thread_cleanup( void );


#if FOUNDATION_PLATFORM_ANDROID

FOUNDATION_API void            thread_attach_jvm( void );
FOUNDATION_API void            thread_detach_jvm( void );

#endif
