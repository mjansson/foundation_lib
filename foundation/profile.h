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

#include <foundation/platform.h>
#include <foundation/types.h>

#if BUILD_ENABLE_PROFILE

FOUNDATION_API void profile_initialize( const char* identifier, size_t length, void* buffer, size_t size );
FOUNDATION_API void profile_shutdown( void );
FOUNDATION_API void profile_enable( bool enable );

FOUNDATION_API void profile_set_output( profile_write_fn writer );
FOUNDATION_API void profile_set_output_wait( unsigned int ms );

FOUNDATION_API void profile_end_frame( uint64_t counter );
FOUNDATION_API void profile_begin_block( const char* message, size_t length );
FOUNDATION_API void profile_update_block( void );
FOUNDATION_API void profile_end_block( void );

FOUNDATION_API void profile_log( const char* message, size_t length );
FOUNDATION_API void profile_trylock( const char* name, size_t length );
FOUNDATION_API void profile_lock( const char* name, size_t length );
FOUNDATION_API void profile_unlock( const char* name, size_t length );
FOUNDATION_API void profile_wait( const char* name, size_t length );
FOUNDATION_API void profile_signal( const char* name, size_t length );

#else

#define profile_initialize( identifier, buffer, size ) do { (void)sizeof( identifier ); (void)sizeof( buffer ); (void)sizeof( size ); } while(0)
#define profile_shutdown() do {} while(0)
#define profile_enable( enable ) do { (void)sizeof( enable ); } while(0)

#define profile_set_output( writer ) do { (void)writer; } while(0)
#define profile_set_output_wait( ms ) do{ (void)sizeof( ms ); } while(0)

#define profile_end_frame( counter ) do { (void)sizeof( counter ); } while(0)
#define profile_begin_block( msg, length ) do { (void)sizeof( msg ); (void)sizeof( length ); } while(0)
#define profile_update_block() do {} while(0)
#define profile_end_block() do {} while(0)

#define profile_log( message, length ) do { (void)sizeof( message ); (void)sizeof( length ); } while(0)
#define profile_trylock( name, length ) do { (void)sizeof( name ); (void)sizeof( length ); } while(0)
#define profile_lock( name, length ) do { (void)sizeof( name ); (void)sizeof( length ); } while(0)
#define profile_unlock( name, length ) do { (void)sizeof( name ); (void)sizeof( length ); } while(0)
#define profile_wait( name, length ) do { (void)sizeof( name ); (void)sizeof( length ); } while(0)
#define profile_signal( name, length ) do { (void)sizeof( name ); (void)sizeof( length ); } while(0)

#endif
