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
FOUNDATION_API void profile_finalize( void );
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

#define profile_initialize( ... ) do { FOUNDATION_UNUSED_ARGS_4( __VA_ARGS__ ); } while(0)
#define profile_finalize() do {} while(0)
#define profile_enable( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_set_output( ... ) do { /*FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ );*/ } while(0)
#define profile_set_output_wait( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_end_frame( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define profile_begin_block_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_begin_block( ... ) profile_begin_block_( __VA_ARGS__ )
#define profile_update_block( ... ) do {} while(0)
#define profile_end_block( ... ) do {} while(0)

#define profile_log_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_log( ... ) profile_log_( __VA_ARGS__ )
#define profile_trylock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_trylock( ... ) profile_trylock_( __VA_ARGS__ )
#define profile_lock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_lock( ... ) profile_lock_( __VA_ARGS__ )
#define profile_unlock_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_unlock( ... ) profile_unlock_( __VA_ARGS__ )
#define profile_wait_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_wait( ... ) profile_wait_( __VA_ARGS__ )
#define profile_signal_( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define profile_signal( ... ) profile_signal_( __VA_ARGS__ )

#endif
