/* profile.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file profile.h
    Profiling API */

#include <foundation/platform.h>
#include <foundation/types.h>


#if BUILD_ENABLE_PROFILE

/*! Initialize profiling library. The string passed to the function must be
    constant over the execution of the application (until profile_shutdown() is called)
    and should identify the application/build/machine or whatever you want it to identify */
FOUNDATION_API void profile_initialize( char* identifier, void* buffer, uint64_t size );

/*! Shutdown profiling library and free resources. Will call the writer callback
    with a null pointer and zero size to indicate end of transmission */
FOUNDATION_API void profile_shutdown( void );


/*! Set output function. The function will be called with blocks of data
    to write to whatever output stream you want. */
FOUNDATION_API void profile_output( profile_write_fn writer );

/*! Toggle profiling. A positive argument enables all profiling calls,
    a negative disables all calls. */
FOUNDATION_API void profile_enable( int enable );


/*! End a frame. Inserts a token into the profiling stream that identifies the end
    of a frame, effectively grouping profile information together in a block */
FOUNDATION_API void profile_end_frame( uint64_t counter );


/*! Begin a named profile timing block. The string passed to this function must
    be constant until the block is written to the output stream. Every call to
	profile_begin_block must be matched with a call to profile_end_block */
FOUNDATION_API void profile_begin_block( const char* message );

/*! Update the current active block. Call this function regularly for blocks
    running for a prolonged time */
FOUNDATION_API void profile_update_block( void );

/*! End the current active block */
FOUNDATION_API void profile_end_block( void );


/*! Insert log message. The string passed to this function must
    be constant until the block is written to the output stream. */
FOUNDATION_API void profile_log( const char* message );


/*! Lock notification. Call this method right before the thread tries
    to acquire a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream. */
FOUNDATION_API void profile_trylock( const char* name );

/*! Lock notification. Call this method right after the thread has
    acquired a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream. */
FOUNDATION_API void profile_lock( const char* name );

/*! Lock notification. Call this method right after the thread has
    released a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream. */
FOUNDATION_API void profile_unlock( const char* name );


#else

#define profile_initialize( identifier, buffer, size ) do { (void)sizeof( identifier ); (void)sizeof( buffer ); (void)sizeof( size ); } while(0)
#define profile_shutdown() do {} while(0)

#define profile_output( writer ) do { (void)sizeof( writer ); } while(0)
#define profile_enable( enable ) do { (void)sizeof( enable ); } while(0)

#define profile_end_frame( counter ) do { (void)sizeof( counter ); } while(0)

#define profile_begin_block( msg ) do { (void)sizeof( msg ); } while(0)
#define profile_update_block() do {} while(0)
#define profile_end_block() do {} while(0)

#define profile_log( message ) do { (void)sizeof( message ); } while(0)

#define profile_trylock( name ) do { (void)sizeof( name ); } while(0)
#define profile_lock( name ) do { (void)sizeof( name ); } while(0)
#define profile_unlock( name ) do { (void)sizeof( name ); } while(0)

#endif
