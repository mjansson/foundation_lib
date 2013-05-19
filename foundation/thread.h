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

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Create new thread
    \param fn                  Thread execution function
    \param name                Thread name
    \param priority            Thread priority
    \param stacksize           Thread stack size
    \return                    New thread object */
FOUNDATION_API object_t        thread_create( thread_fn fn, const char* name, thread_priority_t priority, unsigned int stacksize );

/*! Destroy thread. Stops the thread if it is running
    \param thread              Thread */
FOUNDATION_API void            thread_destroy( object_t thread );

/*! Start a thread if it is not already running
    \param thread              Thread
    \param data                Argument
    \return                    true if thread was started, false if error or already running */
FOUNDATION_API bool            thread_start( object_t thread, void* data );

/*! Query if thread has started
    \param thread              Thread
    \return                    true if started, false if not */
FOUNDATION_API bool            thread_is_started( object_t thread );

/*! Query if thread is running
    \param thread              Thread
    \return                    true if running, false if not */
FOUNDATION_API bool            thread_is_running( object_t thread );

/*! Query if object is a valid thread
    \param thread              Thread object
    \return                    true if a valid thread, false if not */
FOUNDATION_API bool            thread_is_thread( object_t thread );

/*! Query if calling thread is the main thread
    \return                    true if main thread, false if not */
FOUNDATION_API bool            thread_is_main( void );

/*! Mark the calling thread as the main thread */
FOUNDATION_API void            thread_set_main( void );

/*! Terminate thread execution
    \param thread              Thread */
FOUNDATION_API void            thread_terminate( object_t thread );

/*! Query if thread is scheduled for termination
    \param thread              Thread
    \return                    true if thread should terminate, false if not (keep running) */
FOUNDATION_API bool            thread_should_terminate( object_t thread );

/*! Get thread exit value */
FOUNDATION_API void*           thread_result( object_t thread );

/*! Get object handle for calling thread
    \return                    Thread object, null if not a foundation-created thread (like main application thread) */
FOUNDATION_API object_t        thread_self( void );

/*! Get name for calling thread
    \return                    Thread name */
FOUNDATION_API const char*     thread_name( void );

/*! Set name for calling thread
    \param name                Thread name */
FOUNDATION_API void            thread_set_name( const char* name );

/*! Sleep calling thread for specified time
    \param milliseconds        Milliseconds to sleep */
FOUNDATION_API void            thread_sleep( int milliseconds );

/*! Yield calling thread remaining timeslice to other threads */
FOUNDATION_API void            thread_yield( void );

/*! Get calling thread system id
    \return                    Thread id */
FOUNDATION_API uint64_t        thread_id( void );

/*! Get the hardware thread the calling thread is running on
    \return                    Hardware thread id */
FOUNDATION_API unsigned int    thread_hardware( void );

/*! Set thread affinity for calling thread
    \param hw_thread           Hardware thread id */
FOUNDATION_API void            thread_set_hardware( unsigned int hw_thread );

/*! Cleanup on thread exit */
FOUNDATION_API void            thread_cleanup( void );

#if FOUNDATION_PLATFORM_ANDROID

FOUNDATION_API void            thread_attach_jvm( void );
FOUNDATION_API void            thread_detach_jvm( void );

#endif
