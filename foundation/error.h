/* error.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#pragma once

/*! \file error.h
\brief Error context and reporting

Error context and reporting. Most error handling is done per-thread, including
current error and error handler. If error context is not enabled in the build
all error context macros evaluate to no-ops.

Error functions are thread safe. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Get current per-thread error, and reset current thread error to ERROR_NONE
\return Current error for the calling thread */
FOUNDATION_API error_t
error(void);

/*! Report error for the calling thread. Does not affect error state for any other thread.
If a error handler is set for the calling thread it will be called and the return value
propagated.
\param level Error level
\param err   Error identifier
\return      Return value from error handler if set, 0 if no handler set */
FOUNDATION_API int
error_report(error_level_t level, error_t err);

/*! Get per-thread error handling handler
\return Error handler */
FOUNDATION_API error_handler_fn
error_handler(void);

/*! Set new per-thread error handling handler. The handler will be called each time
the calling thread reports an error.
\param handler Error handler */
FOUNDATION_API void
error_set_handler(error_handler_fn handler);

/*! Clone the current error context
\return Clone of current error context */
FOUNDATION_API error_context_t*
error_context_clone(void);

/*! Replace the current error context
\param context New error context
\return Previous error context */
FOUNDATION_API error_context_t*
error_context_set(error_context_t* context);

#if BUILD_ENABLE_ERROR_CONTEXT

#define _error_context_push_proxy(...)        \
	do {                                      \
		error_context_push_impl(__VA_ARGS__); \
	} while (0)

#define _error_context_push_format_proxy(...)        \
	do {                                             \
		error_context_push_format_impl(__VA_ARGS__); \
	} while (0)

#define _error_context_buffer_proxy(...) error_context_buffer_impl(__VA_ARGS__)

/*! Push a new error context and associated data on the error context stack.
Both context and data must be valid for as long as it remains on the stack.
\param name Context name
\param name_length Context name length
\param data Context data buffer
\param data_length Context data buffer size */
#define error_context_push(...)                 \
	do {                                        \
		_error_context_push_proxy(__VA_ARGS__); \
	} while (0)

/*! Push a new error context and associated formatted data on the error context stack.
Both context and data must be valid for as long as it remains on the stack.
\param name Context name
\param name_length Context name length
\param data Context data buffer
\param data_length Context data buffer size
\param data_format Context data format string
\param data_format_length Context data format string length */
#define error_context_push_format(...)                 \
	do {                                               \
		_error_context_push_format_proxy(__VA_ARGS__); \
	} while (0)

/*! Pop the top error context off the error context stack */
#define error_context_pop()       \
	do {                          \
		error_context_pop_impl(); \
	} while (0)

/*! Clear the error context */
#define error_context_clear()       \
	do {                            \
		error_context_clear_impl(); \
	} while (0)

/*! Generate a error context stack description string in the given buffer, limited
to the given size.
\param buffer    Destination buffer
\param capacity  Buffer capacity */
#define error_context_buffer(...) _error_context_buffer_proxy(__VA_ARGS__)

/*! Get the current error context, or 0 if no context set/available. */
#define error_context() error_context_impl()

/*! Make a local declaration depending on if error contexts are enabled in the build
or not. If error contexts are disabled the expression will evaluate to void and not
evaluate any code.
\param decl Declaration */
#define error_context_declare_local(decl) decl

/*! Clean up thread local storage related to error context on thread exit. */
#define error_context_thread_finalize()       \
	do {                                      \
		error_context_thread_finalize_impl(); \
	} while (0)

FOUNDATION_API void
error_context_push_impl(const char* name, size_t name_length, const char* data, size_t data_length);

FOUNDATION_API void
error_context_push_format_impl(const char* name, size_t name_length, char* data, size_t data_length,
                               const char* data_format, size_t data_format_length, ...);

FOUNDATION_API void
error_context_pop_impl(void);

FOUNDATION_API void
error_context_clear_impl(void);

FOUNDATION_API string_t
error_context_buffer_impl(char* buffer, size_t capacity);

FOUNDATION_API error_context_t*
error_context_impl(void);

FOUNDATION_API void
error_context_thread_finalize_impl(void);

#else

#define error_context_push(...) \
	do { /* */                  \
	} while (0)
#define error_context_push_format(...) \
	do { /* */                         \
	} while (0)
#define error_context_pop() \
	do { /* */              \
	} while (0)
#define error_context_clear() \
	do { /* */                \
	} while (0)
#define error_context_buffer(str, length) string_copy(str, length, 0, 0)
#define error_context() 0
#define error_context_declare_local(decl) \
	do { /* */                            \
	} while (0)
#define error_context_thread_finalize() \
	do { /* */                          \
	} while (0)

#endif
