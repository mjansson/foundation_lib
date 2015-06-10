/* log.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/error.h>


#if BUILD_ENABLE_LOG && BUILD_ENABLE_DEBUG_LOG

FOUNDATION_API void              log_debug( hash_t context, const char* msg, size_t msg_length );
FOUNDATION_API void              log_debugf( hash_t context, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 2, 4 );

#endif

#if BUILD_ENABLE_LOG

FOUNDATION_API void              log_info( hash_t context, const char* format, size_t format_length );
FOUNDATION_API void              log_infof( hash_t context, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 2, 4 );
FOUNDATION_API void              log_warn( hash_t context, warning_t warn, const char* format, size_t format_length );
FOUNDATION_API void              log_warnf( hash_t context, warning_t warn, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 5 );
FOUNDATION_API void              log_error( hash_t context, error_t err, const char* format, size_t format_length );
FOUNDATION_API void              log_errorf( hash_t context, error_t err, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 5 );
FOUNDATION_API void              log_panic( hash_t context, error_t err, const char* format, size_t format_length );
FOUNDATION_API void              log_panicf( hash_t context, error_t err, const char* format, size_t format_length, ... ) FOUNDATION_ATTRIBUTE4( format, printf, 3, 5 );

FOUNDATION_API void              log_error_context( hash_t context, error_level_t error_level );
FOUNDATION_API log_callback_fn   log_callback( void );
FOUNDATION_API void              log_set_callback( log_callback_fn callback );
FOUNDATION_API void              log_enable_stdout( bool enable );
FOUNDATION_API void              log_enable_prefix( bool enable );
FOUNDATION_API void              log_set_suppress( hash_t context, error_level_t level );
FOUNDATION_API error_level_t     log_suppress( hash_t context );
FOUNDATION_API void              log_suppress_clear( void );

#endif

#if !BUILD_ENABLE_LOG || !BUILD_ENABLE_DEBUG_LOG

#define log_debug_( ... ) do { FOUNDATION_UNUSED_ARGS_3( __VA_ARGS__ ); } while(0)
#define log_debug( ... ) log_debug_( __VA_ARGS__ )
#define log_debugf_( ... ) do { FOUNDATION_UNUSED_VARARGS_3( __VA_ARGS__ ); } while(0)
#define log_debugf( ... ) log_debugf_( __VA_ARGS__ )

#endif

#if !BUILD_ENABLE_LOG

#define log_info_( ... ) do { FOUNDATION_UNUSED_ARGS_3( __VA_ARGS__ ); } while(0)
#define log_info( ... ) log_info_( __VA_ARGS__ )
#define log_infof_( ... ) do { FOUNDATION_UNUSED_VARARGS_3( __VA_ARGS__ ); } while(0)
#define log_infof( ... ) log_infof_( __VA_ARGS__ )

#define log_warn_( ... ) do { FOUNDATION_UNUSED_ARGS_4( __VA_ARGS__ ); } while(0)
#define log_warn( ... ) log_warn_( __VA_ARGS__ )
#define log_warnf_( ... ) do { FOUNDATION_UNUSED_VARARGS_4( __VA_ARGS__ ); } while(0)
#define log_warnf( ... ) log_warnf_( __VA_ARGS__ )

#define log_error_( ... ) do { FOUNDATION_UNUSED_ARGS_4( __VA_ARGS__ ); } while(0)
#define log_error( ... ) log_error_( __VA_ARGS__ )
#define log_errorf_( ... ) do { FOUNDATION_UNUSED_VARARGS_4( __VA_ARGS__ ); } while(0)
#define log_errorf( ... ) log_errorf_( __VA_ARGS__ )

#define log_panic_( ... ) do { FOUNDATION_UNUSED_ARGS_4( __VA_ARGS__ ); } while(0)
#define log_panic( ... ) log_panic_( __VA_ARGS__ )
#define log_panicf_( ... ) do { FOUNDATION_UNUSED_VARARGS_4( __VA_ARGS__ ); } while(0)
#define log_panicf( ... ) log_panicf_( __VA_ARGS__ )

#define log_error_context( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define log_callback() do {} while(0)
#define log_set_callback( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define log_enable_stdout( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define log_enable_prefix( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define log_set_suppress( ... ) do { FOUNDATION_UNUSED_ARGS_2( __VA_ARGS__ ); } while(0)
#define log_suppress( ... ) do { FOUNDATION_UNUSED_ARGS_1( __VA_ARGS__ ); } while(0)
#define log_suppress_clear() do {} while(0)

#endif
