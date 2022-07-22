/* log.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file log.h
\brief Log output, filtering and context handling

Log output, filtering and context. Logging is centered around two concepts, a context and a level.
Log messages can be filtered on both, with a specific threshold available for each context.

A context is identified by a hash value, normally a static string hash.

A log level is either debug, info, warning, error or panic (in increasing order of significance).

Using different function calls for different log levels allows build-time contol over code
generation. Disabling log debug message build flag would make all log_debug/log_debugf calls
to be statically removed at compile time instead of filtered at runtime, reducing binary
size and call overhead. By enabling the build flag and instead using a log context level
filter a runtime selection of log messages can be selected instead. */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/error.h>

#if BUILD_ENABLE_LOG && BUILD_ENABLE_DEBUG_LOG

/*! Log an unformatted debug message in the specific context
\param context Log context
\param msg Log message
\param msg_length Length of message */
FOUNDATION_API void
log_debug(hash_t context, const char* msg, size_t msg_length);

/*! Log formatted debug message in specified log context
\param context Log context
\param format Log message format specifier
\param format_length Length of log message format specifier */
FOUNDATION_API void
log_debugf(hash_t context, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(2, 4);

#endif

#if BUILD_ENABLE_LOG

/*! \fn void log_info(hash_t context, const char* msg, size_t msg_length)
Log an unformatted info message in the specific context
\param context Log context
\param msg Log message
\param msg_length Length of message */
FOUNDATION_API void
log_info(hash_t context, const char* msg, size_t msg_length);

/*! Log formatted info message in specified log context
\param context Log context
\param format Log message format
\param format_length Length of log message format specifier */
FOUNDATION_API void
log_infof(hash_t context, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(2, 4);

/*! \fn void log_warn(hash_t context, warning_t warn, const char* msg, size_t msg_length)
Log an unformatted warning message in the specific context
\param context Log context
\param warn Warning type
\param msg Log message
\param msg_length Length of message */
FOUNDATION_API void
log_warn(hash_t context, warning_t warn, const char* msg, size_t msg_length);

/*! Log formatted warning message in specified log context
\param context Log context
\param warn Warning type
\param format Log message format
\param format_length Length of log message format specifier */
FOUNDATION_API void
log_warnf(hash_t context, warning_t warn, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(3, 5);

/*! \fn void log_error(hash_t context, error_t err, const char* msg, size_t msg_length)
Log an unformatted error message in the specific context. Will also
report the error code through error_report
\param context Log context
\param err Error code
\param msg Log message
\param msg_length Length of message */
FOUNDATION_API void
log_error(hash_t context, error_t err, const char* msg, size_t msg_length);

/*! Log formatted error message in specified log context. Will also
report the error code through error_report
\param context Log context
\param err Error code
\param format Log message format
\param format_length Length of log message format specifier */
FOUNDATION_API void
log_errorf(hash_t context, error_t err, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(3, 5);

/*! \fn void log_panic(hash_t context, error_t err, const char* msg, size_t msg_length)
Log an unformatted panic message in the specific context. Will also
report the error code through error_report
\param context Log context
\param err Error code
\param msg Log message
\param msg_length Length of message */
FOUNDATION_API void
log_panic(hash_t context, error_t err, const char* msg, size_t msg_length);

/*! Log formatted panic message in specified log context
\param context Log context
\param err Error code
\param format Log message format
\param format_length Length of log message format specifier */
FOUNDATION_API void
log_panicf(hash_t context, error_t err, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(3, 5);

/*! \fn void log_error_context(hash_t context, error_level_t error_level)
Log the current error context in the specified log context. The log output is filtered
at the given severity level
\param context Log context
\param error_level Severity level */
FOUNDATION_API void
log_error_context(hash_t context, error_level_t error_level);

/*! Get the currently set log handler
\return Log handler */
FOUNDATION_API log_handler_fn
log_handler(void);

/*! Set log handler
\param handler New handler */
FOUNDATION_API void
log_set_handler(log_handler_fn handler);

/*! Control log output to stdout
\param enable Flag to enable/disable output to stdout */
FOUNDATION_API void
log_enable_stdout(bool enable);

/*! Query log output to stdout
\return true if output to stdout is enabled, false if disabled */
FOUNDATION_API bool
log_stdout(void);

/*! Control output of prefix information
\param enable Flag to enable/disable prefix output */
FOUNDATION_API void
log_enable_prefix(bool enable);

/*! Control automatic newline
\param enable Flag to enable/disable automatic newline */
FOUNDATION_API void
log_enable_auto_newline(bool enable);

/*! Control log suppression based on severity level. Any messages at the
given severity level or lower will be filtered and discarded. If a log context
has no explicit supression level the default (0) context supression level will be used.
\param context Log context
\param level Severity level to discard */
FOUNDATION_API void
log_set_suppress(hash_t context, error_level_t level);

/*! Get current log supression level for the given context
\param context Log context
\return Severity level being discarded for the given context */
FOUNDATION_API error_level_t
log_suppress(hash_t context);

/*! Clear all log suppression for given context, or all context if null
\param context Log context to clear, clears all contexts if set to zero */
FOUNDATION_API void
log_suppress_clear(hash_t context);

#endif

#if !BUILD_ENABLE_LOG || !BUILD_ENABLE_DEBUG_LOG

#define log_debug_(...)                         \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_debug(...) log_debug_(__VA_ARGS__)
#define log_debugf_(...)                        \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_debugf(...) log_debugf_(__VA_ARGS__)

#endif

#if !BUILD_ENABLE_LOG

#define log_info_(...)                          \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_info(...) log_info_(__VA_ARGS__)
#define log_infof_(...)                         \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_infof(...) log_infof_(__VA_ARGS__)

#define log_warn_(...)                          \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_warn(...) log_warn_(__VA_ARGS__)
#define log_warnf_(...)                         \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_warnf(...) log_warnf_(__VA_ARGS__)

#define log_error_(...)                                                               \
	do {                                                                              \
		error_report(ERRORLEVEL_ERROR, FOUNDATION_PREPROCESSOR_ELEM(1, __VA_ARGS__)); \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__);                                       \
	} while (0)
#define log_error(...) log_error_(__VA_ARGS__)
#define log_errorf_(...)                                                              \
	do {                                                                              \
		error_report(ERRORLEVEL_ERROR, FOUNDATION_PREPROCESSOR_ELEM(1, __VA_ARGS__)); \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__);                                       \
	} while (0)
#define log_errorf(...) log_errorf_(__VA_ARGS__)

#define log_panic_(...)                                                               \
	do {                                                                              \
		error_report(ERRORLEVEL_PANIC, FOUNDATION_PREPROCESSOR_ELEM(1, __VA_ARGS__)); \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__);                                       \
	} while (0)
#define log_panic(...) log_panic_(__VA_ARGS__)
#define log_panicf_(...)                                                              \
	do {                                                                              \
		error_report(ERRORLEVEL_PANIC, FOUNDATION_PREPROCESSOR_ELEM(1, __VA_ARGS__)); \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__);                                       \
	} while (0)
#define log_panicf(...) log_panicf_(__VA_ARGS__)

#define log_error_context(context, level) \
	do {                                  \
		FOUNDATION_UNUSED(context);       \
		FOUNDATION_UNUSED(level);         \
	} while (0)
#define log_handler() 0
#define log_set_handler(...)                    \
	do {                                        \
		FOUNDATION_UNUSED_VARARGS(__VA_ARGS__); \
	} while (0)
#define log_enable_stdout(enable)  \
	do {                           \
		FOUNDATION_UNUSED(enable); \
	} while (0)
#define log_stdout() false
#define log_enable_prefix(enable)  \
	do {                           \
		FOUNDATION_UNUSED(enable); \
	} while (0)
#define log_set_suppress(context, level) \
	do {                                 \
		FOUNDATION_UNUSED(context);      \
		FOUNDATION_UNUSED(level);        \
	} while (0)
#define log_suppress(context) ERRORLEVEL_NONE
#define log_suppress_clear() \
	do {                     \
	} while (0)

#endif
