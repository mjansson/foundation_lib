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
    and should identify the application/build/machine or whatever you want it to identify.
    Memory buffer should be large enough to hold data for ~100ms to avoid excessive
    calls to output flush function. The profile subsystem will not allocate any memory,
    it only uses the passed in work buffer. Recommended size is at least 256KiB.
    Maximum usable size is 4MiB, since the profiling system can only use 65k blocks of 64 bytes each.
    \param identifier                    Application identifier
    \param buffer                        Work temporary buffer
    \param size                          Size of work buffer */
FOUNDATION_API void profile_initialize( const char* identifier, void* buffer, uint64_t size );

/*! Shutdown profiling library and free resources. Will call the writer callback
    with a null pointer and zero size to indicate end of transmission */
FOUNDATION_API void profile_shutdown( void );

/*! Toggle profiling. A true argument enables all profiling calls,
    a false disables all calls.
    \param enable                        Enable flag */
FOUNDATION_API void profile_enable( bool enable );

/*! Set output function. The function will be called with blocks of data
    to write to whatever output stream you want.
    \param writer                        New writer function */
FOUNDATION_API void profile_set_output( profile_write_fn writer );

/*! Control profile output rate by setting time between flushes in milliseconds.
    Default is 100ms. Increase rate when passing a smaller buffer to initialization,
    decrease rate if passing a larger buffer
    \param ms                            Wait time in milliseconds */
FOUNDATION_API void profile_set_output_wait( int ms );

/*! End a frame. Inserts a token into the profiling stream that identifies the end
    of a frame, effectively grouping profile information together in a block
    \param counter                       Frame counter */
FOUNDATION_API void profile_end_frame( uint64_t counter );

/*! Begin a named profile timing block. The string passed to this function must
    be constant until the block is written to the output stream. Every call to
	profile_begin_block must be matched with a call to profile_end_block
    \param message                       Block message (name) */
FOUNDATION_API void profile_begin_block( const char* message );

/*! Update the current active block. Call this function regularly for blocks
    running for a prolonged time */
FOUNDATION_API void profile_update_block( void );

/*! End the current active block */
FOUNDATION_API void profile_end_block( void );

/*! Insert log message. The string passed to this function must
    be constant until the block is written to the output stream.
    \param message                       Message to insert into the profile stream */
FOUNDATION_API void profile_log( const char* message );

/*! Lock notification. Call this method right before the thread tries
    to acquire a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream.
    \param name                          Lock name */
FOUNDATION_API void profile_trylock( const char* name );

/*! Lock notification. Call this method right after the thread has
    acquired a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream.
    \param name                          Lock name */
FOUNDATION_API void profile_lock( const char* name );

/*! Lock notification. Call this method right after the thread has
    released a lock on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream.
    \param name                          Lock name */
FOUNDATION_API void profile_unlock( const char* name );

/*! Wait notification. Call this method right before the thread enters a wait state
    on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream.
    \param name                          Lock name */
FOUNDATION_API void profile_wait( const char* name );

/*! Signal notification. Call this method right before the thread signals state
    on a mutually exclusive resource. The string passed to this function must
    be constant until the block is written to the output stream.
    \param name                          Lock name */
FOUNDATION_API void profile_signal( const char* name );

#else

#define profile_initialize( identifier, buffer, size ) do { (void)sizeof( identifier ); (void)sizeof( buffer ); (void)sizeof( size ); } while(0)
#define profile_shutdown() do {} while(0)

#define profile_enable( enable ) do { (void)sizeof( enable ); } while(0)
#define profile_set_output( writer ) do { (void)writer; } while(0)
#define profile_set_output_wait( ms ) do{ (void)sizeof( ms ); } while(0)

#define profile_end_frame( counter ) do { (void)sizeof( counter ); } while(0)
#define profile_begin_block( msg ) do { (void)sizeof( msg ); } while(0)
#define profile_update_block() do {} while(0)
#define profile_end_block() do {} while(0)
#define profile_log( message ) do { (void)sizeof( message ); } while(0)
#define profile_trylock( name ) do { (void)sizeof( name ); } while(0)
#define profile_lock( name ) do { (void)sizeof( name ); } while(0)
#define profile_unlock( name ) do { (void)sizeof( name ); } while(0)
#define profile_wait( name ) do { (void)sizeof( name ); } while(0)
#define profile_signal( name ) do { (void)sizeof( name ); } while(0)

#endif
