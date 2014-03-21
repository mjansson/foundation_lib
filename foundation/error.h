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

/*! \file error.h
    Error context and reporting. Most error handling is done per-thread, including
	current error and error callback. If error context is not enabled in the build
    all error context macros evaluate to no-ops. */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Get current per-thread error, and reset current thread error to ERROR_NONE
    \return               Current error for the calling thread */
FOUNDATION_API error_t    error( void );

/*! Report error for the calling thread. Does not affect error state for any other thread.
    If a error callback is set for the calling thread it will be called and the
    return value propagated.
    \param level          Error level
    \param err            Error identifier
    \return               Return value from error callback if set, 0 if no callback set */
FOUNDATION_API int        error_report( error_level_t level, error_t err );

/*! Set new per-thread error handling callback. The callback will be called each time
    the calling thread reports an error.
    \param callback       Error callback */
FOUNDATION_API void       error_set_callback( error_callback_fn callback );


/*! \def error_context_push
    Push a new error context and associated data on the error context stack */

/*! \def error_context_pop
    Pop the top error context off the error context stack */

/*! \def error_context_buffer
    Generate a error context stack description string in the given buffer, limited
    to the given size */

/*! \def error_context
    Get the current error context, or 0 if no context set/available */

/*! \def error_context_declare_local
    Make a local declaration depending on if error contexts are enabled in the build
    or not */

/*! \def error_context_thread_deallocate
    Clean up thread local storage related to error context on thread exit */

#if BUILD_ENABLE_ERROR_CONTEXT

#define error_context_push( name, data )      do { _error_context_push( (name), (data) ); } while(0)
#define error_context_pop()                   do { _error_context_pop(); } while(0)
#define error_context_buffer( buffer, size )  do { _error_context_buffer( buffer, size ); } while(0)
#define error_context()                       _error_context()
#define error_context_declare_local( decl )   decl
#define error_context_thread_deallocate()     do { _error_context_thread_deallocate(); } while(0)

FOUNDATION_API void                          _error_context_push( const char* name, const char* data );
FOUNDATION_API void                          _error_context_pop( void );
FOUNDATION_API void                          _error_context_buffer( char* buffer, unsigned int size );
FOUNDATION_API error_context_t*              _error_context( void );
FOUNDATION_API void                          _error_context_thread_deallocate( void );

#else

#define error_context_push( name, data )      /*lint -save -e506 -e751 */ do { (void)sizeof( name ); (void)sizeof( data ); } while(0) /*lint -restore -e506 -e751 */
#define error_context_pop()                   do { /* */ } while(0)
#define error_context_buffer( buffer, size )  /*lint -save -e506 -e751 */ do { (void)sizeof( buffer ); (void)sizeof( size ); } while(0) /*lint -restore -e506 -e751 */
#define error_context()                       0
#define error_context_declare_local( decl )   do { /* */ } while(0)
#define error_context_thread_deallocate()     do { /* */ } while(0)

#endif
