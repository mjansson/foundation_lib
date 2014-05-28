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

/*! \file log.h
    Log output, filtering, context handling and callbacks. Logging is centered around two concepts,
    a context and a level. Log messages can be filtered on both, with a specific threshold available
    for each context.

    A context is identified by a hash value, normally a static string hash.

    A log level is either debug, info, warning, error or panic (in increasing order of significance).

    Using different function calls for different log levels allows build-time contol over code
    generation. Disabling log debug message build flag would make all log_debug/log_debugf calls
    to be statically removed at compile time instead of filtered at runtime, reducing binary
    size and call overhead. By enabling the build flag and instead using a log context level
    filter a runtime selection of log messages can be selected instead. */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/hashstrings.h>


/*! \def log_debug
    Log an unformatted debug message in the specific context */

/*! \fn log_debugf
    Log formatted debug message in specified log context
    \param context                              Log context
    \param format                               Log message format */

/*! \def log_info
    Log an unformatted info message in the specific context */

/*! \fn log_infof
    Log formatted info message in specified log context
    \param context                              Log context
    \param format                               Log message format */

/*! \def log_warn
    Log an unformatted warning message in the specific context */

/*! \fn log_warnf
    Log formatted warning message in specified log context
    \param context                              Log context
    \param warn                                 Warning type
    \param format                               Log message format */

/*! \def log_error
    Log an unformatted error message in the specific context. Will also
    report the error code through error_report */

/*! \fn log_errorf
    Log formatted error message in specified log context. Will also
    report the error code through error_report
    \param context                              Log context
    \param err                                  Error code
    \param format                               Log message format */

/*! \def log_panic
    Log an unformatted panic message in the specific context. Will also
    report the error code through error_report */

/*! \fn log_panicf
    Log formatted panic message in specified log context
    \param context                              Log context
    \param err                                  Error code
    \param format                               Log message format */

/*! \fn log_error_context
    Log the current error context in the specified log context. The log output is filtered at the given severity level
    \param context                              Log context
    \param error_level                          Severity level */

/*! \fn log_set_callback
    Set log callback
    \param callback                             New callback */

/*! \fn log_enable_stdout
    Control log output to stdout
    \param enable                               Flag to enable/disable output to stdout */

/*! \fn log_enable_prefix
    Control output of prefix information
    \param enable                               Flag to enable/disable prefix output */

/*! \fn log_set_suppress
    Control log suppression based on severity level. Any messages at the
    given severity level or lower will be filtered and discarded. If a log context
    has no explicit supression level the default (0) context supression level will be used.
    \param context                              Log context
    \param level                                Severity level to discard */

/*! \fn log_suppress
    Get current log supression level
    \param context                              Log context
    \return                                     Severity level begin discarded */

#if BUILD_ENABLE_DEBUG_LOG

#define log_debug( context, msg )               log_debugf( context, "%s", msg )

FOUNDATION_API void                             log_debugf( uint64_t context, const char* format, ... );

#else
#  define log_debug( context, msg ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_debugf( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_LOG
#define log_info( context, msg )                log_infof( context, "%s", msg )
FOUNDATION_API void                             log_infof( uint64_t context, const char* format, ... );
#define log_warn( context, warn, msg )          log_warnf( context, warn, "%s", msg )
FOUNDATION_API void                             log_warnf( uint64_t context, warning_t warn, const char* format, ... );
#define log_error( context, err, msg )          log_errorf( context, err, "%s", msg )
FOUNDATION_API void                             log_errorf( uint64_t context, error_t err, const char* format, ... );
#define log_panic( context, err, msg )          log_panicf( context, err, "%s", msg )
FOUNDATION_API void                             log_panicf( uint64_t context, error_t err, const char* format, ... );
FOUNDATION_API void                             log_error_context( uint64_t context, error_level_t error_level );
FOUNDATION_API void                             log_set_callback( log_callback_fn callback );
FOUNDATION_API void                             log_enable_stdout( bool enable );
FOUNDATION_API void                             log_enable_prefix( bool enable );
FOUNDATION_API void                             log_set_suppress( uint64_t context, error_level_t level );
FOUNDATION_API error_level_t                    log_suppress( uint64_t context );
#else
#  define log_info( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_infof( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_warn( context, warn, msg ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_warnf( context, warn, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_error( context, err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_errorf( context, err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_panic( context, err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_panicf( context, err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_error_context( context, error_level ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( error_level ); } while(0) /*lint -restore */
#  define log_set_callback( callback ) /*lint -save -e717 */ do { (void)sizeof( callback ); } while(0) /*lint -restore */
#  define log_enable_stdout( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define log_enable_prefix( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define log_set_suppress( context, level ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( level ); } while(0) /*lint -restore */
#  define log_suppress( context ) ERRORLEVEL_NONE
#endif
