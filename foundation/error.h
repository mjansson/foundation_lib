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
\brief Error context and reporting
\details Error context and reporting. Most error handling is done per-thread, including
current error and error callback. If error context is not enabled in the build
all error context macros evaluate to no-ops. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Get current error
Get current per-thread error, and reset current thread error to ERROR_NONE
\return          Current error for the calling thread */
FOUNDATION_API error_t
error(void);

/*! \brief Report error
Report error for the calling thread. Does not affect error state for any other thread.
If a error callback is set for the calling thread it will be called and the
return value propagated.
\param level     Error level
\param err       Error identifier
\return          Return value from error callback if set, 0 if no callback set */
FOUNDATION_API int
error_report(error_level_t level, error_t err);

/*! \brief Get error callback
Get per-thread error handling callback
\return          Error callback */
FOUNDATION_API error_callback_fn
error_callback(void);

/*! \brief Set error callback
Set new per-thread error handling callback. The callback will be called each time
the calling thread reports an error.
\param callback  Error callback */
FOUNDATION_API void
error_set_callback(error_callback_fn callback);

#if BUILD_ENABLE_ERROR_CONTEXT

#define _error_context_push_proxy( name, namelen, data, datalen ) do { \
  _error_context_push( (name), (namelen), (data), (datalen) ); \
  } while(0)

#define _error_context_buffer_proxy( str, length ) \
  _error_context_buffer( str, length )

/*! \brief Push error context
Push a new error context and associated data on the error context stack.
Both context and data must be valid for as long as it remains on the stack.
\param name Context name
\param data Context data buffer
\param data_length Context data buffer size */
#define error_context_push( ... ) do { \
  _error_context_push_proxy( __VA_ARGS__ ); \
  } while(0)

/*! \brief Pop error context
Pop the top error context off the error context stack */
#define error_context_pop() do { \
  _error_context_pop(); \
  } while(0)

/*! \brief Clear error context
Clear the error context */
#define error_context_clear() do { \
  _error_context_clear(); \
  } while(0)

/*! \brief Generate error context stack trace
Generate a error context stack description string in the given buffer, limited
to the given size
\param buffer  Destination buffer
\param size    Maximum buffer size */
#define error_context_buffer( ... ) \
  _error_context_buffer_proxy( __VA_ARGS__ )

/*! \brief Get error context
Get the current error context, or 0 if no context set/available */
#define error_context() \
  _error_context()

/*! \brief Make local declaration
Make a local declaration depending on if error contexts are enabled
in the build or not. If error contexts are disabled the expression will evaluate
to void and not evaluate any code.
\param decl    Declaration */
#define error_context_declare_local( decl ) \
  decl

/*! \brief Clean up memory
Clean up thread local storage related to error context on thread exit */
#define error_context_thread_deallocate() do { \
  _error_context_thread_deallocate(); \
  } while(0)

FOUNDATION_API void
_error_context_push(const char* name, size_t name_length, const char* data, size_t data_length);

FOUNDATION_API void
_error_context_pop(void);

FOUNDATION_API void
_error_context_clear(void);

FOUNDATION_API string_t
_error_context_buffer(char* str, size_t length);

FOUNDATION_API error_context_t*
_error_context(void);

FOUNDATION_API void
_error_context_thread_deallocate(void);

#else

#define _error_context_push( name, namelen, data, datalen ) /*lint -save -e506 -e751 */ do { \
  (void)sizeof( name );
  (void)sizeof( namelen );
  (void)sizeof( data );
  (void)sizeof( datalen );
  } while(0) /*lint -restore -e506 -e751 */

#define error_context_push( ... ) do { \
  _error_context_push( __VA_ARGS__ ); \
  } while(0)

#define error_context_pop() do { /* */ } while(0)

#define error_context_clear()  do { /* */ } while(0)

#define error_context_buffer( str, length ) \
  string_copy( str, length, 0, 0 )

#define error_context() 0

#define error_context_declare_local( decl ) do { /* */ } while(0)

#define error_context_thread_deallocate() do { /* */ } while(0)

#endif
