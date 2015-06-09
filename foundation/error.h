/* error.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


FOUNDATION_API error_t            error( void );
FOUNDATION_API int                error_report( error_level_t level, error_t err );
FOUNDATION_API error_callback_fn  error_callback( void );
FOUNDATION_API void               error_set_callback( error_callback_fn callback );


#if BUILD_ENABLE_ERROR_CONTEXT

#define _error_context_push_proxy( name, namelen, data, datalen ) do { _error_context_push( (name), (namelen), (data), (datalen) ); } while(0)
#define error_context_push( ... )             do { _error_context_push_proxy( __VA_ARGS__ ); } while(0)
#define error_context_pop()                   do { _error_context_pop(); } while(0)
#define error_context_clear()                 do { _error_context_clear(); } while(0)
#define _error_context_buffer_proxy( str, length )  _error_context_buffer( str, length )
#define error_context_buffer( ... )           _error_context_buffer_proxy( __VA_ARGS__ )
#define error_context()                       _error_context()
#define error_context_declare_local( decl )   decl
#define error_context_thread_deallocate()     do { _error_context_thread_deallocate(); } while(0)

FOUNDATION_API void                           _error_context_push( const char* name, size_t name_length, const char* data, size_t data_length );
FOUNDATION_API void                           _error_context_pop( void );
FOUNDATION_API void                           _error_context_clear( void );
FOUNDATION_API string_t                       _error_context_buffer( char* str, size_t length );
FOUNDATION_API error_context_t*               _error_context( void );
FOUNDATION_API void                           _error_context_thread_deallocate( void );

#else

#define _error_context_push( name, namelen, data, datalen ) /*lint -save -e506 -e751 */ do { (void)sizeof( name ); (void)sizeof( namelen ); (void)sizeof( data ); (void)sizeof( datalen ); } while(0) /*lint -restore -e506 -e751 */
#define error_context_push( ... )             do { _error_context_push( __VA_ARGS__ ); } while(0)
#define error_context_pop()                   do { /* */ } while(0)
#define error_context_clear()                 do { /* */ } while(0)
#define error_context_buffer( str, length )   string_copy( str, length, 0, 0 )
#define error_context()                       0
#define error_context_declare_local( decl )   do { /* */ } while(0)
#define error_context_thread_deallocate()     do { /* */ } while(0)

#endif
