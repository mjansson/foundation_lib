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
    Log output */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/hashstrings.h>


#if BUILD_ENABLE_DEBUG_LOG


#define log_debug( msg )                        log_context_debugf( 0, "%s", msg )
#define log_debugf( msg, ... )                  log_context_debugf( 0, msg, __VA_ARGS__ )
#define log_context_debug( context, msg )       log_context_debugf( context, "%s", msg )

/*! Log formatted debug message in specified log context
    \param context                              Log context
    \param format                               Log message format */
FOUNDATION_API void                             log_context_debugf( uint64_t context, const char* format, ... );

#else
#  define log_debug( msg ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_debugf( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_debug( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_debugf( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_LOG

#define log_info( msg )                         log_context_infof( 0, "%s", msg )
#define log_infof( msg, ... )                   log_context_infof( 0, msg, __VA_ARGS__ )
#define log_context_info( context, msg )        log_context_infof( context, "%s", msg )

/*! Log information message in specified log context
    \param context                              Log context
    \param format                               Log format */
FOUNDATION_API void                             log_context_infof( uint64_t context, const char* format, ... );

#define log_warn( warn, msg )                   log_context_warnf( 0, warn, "%s", msg )
#define log_warnf( warn, msg, ... )             log_context_warnf( 0, warn, msg, __VA_ARGS__ )
#define log_context_warn( context, warn, msg )  log_context_warnf( context, warn, "%s", msg )

/*! Log warning message in specified log context, including the current error context
    \param context                              Log context
    \param warn                                 Warning code
    \param format                               Log format */
FOUNDATION_API void                             log_context_warnf( uint64_t context, warning_t warn, const char* format, ... );

#define log_error( err, msg )                   log_context_errorf( 0, err, "%s", msg )
#define log_errorf( err, msg, ... )             log_context_errorf( 0, err, msg, __VA_ARGS__ )
#define log_context_error( context, err, msg )  log_context_errorf( context, err, "%s", msg )

/*! Log error message in specified log context, including the current error context. Also calls error_report to report the error
    \param context                              Log context
    \param err                                  Error code
    \param format                               Log format */
FOUNDATION_API void                             log_context_errorf( uint64_t context, error_t err, const char* format, ... );

#define log_panic( err, msg )                   log_context_panicf( 0, err, "%s", msg )
#define log_panicf( err, msg, ... )             log_context_panicf( 0, err, msg, __VA_ARGS__ )
#define log_context_panic( context, err, msg )  log_context_panicf( context, err, "%s", msg )

/*! Log panic message in the specified log context, including the current error context. Also calls error_report to report the error
    \param context                              Log context
    \param err                                  Error code
    \param format                               Log format */
FOUNDATION_API void                             log_context_panicf( uint64_t context, error_t err, const char* format, ... );

/*! Log the current error context in the specified log context. The log output is filtered at the given severity level
    \param context                              Log context
    \param error_level                          Severity level */
FOUNDATION_API void                             log_error_context( uint64_t context, error_level_t error_level );

/*! Set log callback
    \param callback                             New callback */
FOUNDATION_API void                             log_set_callback( log_callback_fn callback );

/*! Control log output to stdout
    \param enable                               Flag to enable/disable output to stdout */
FOUNDATION_API void                             log_enable_stdout( bool enable );

/*! Control output of prefix information
    \param enable                               Flag to enable/disable prefix output */
FOUNDATION_API void                             log_enable_prefix( bool enable );

/*! Control log suppression based on severity level. Any messages at the
    given severity level or lower will be filtered and discarded.
    \param context                              Log context
    \param level                                Severity level to discard */
FOUNDATION_API void                             log_set_suppress( uint64_t context, error_level_t level );

/*! Get current log supression level
    \param context                              Log context
    \return                                     Severity level begin discarded */
FOUNDATION_API error_level_t                    log_suppress( uint64_t context );

#else
#  define log_info( msg ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_infof( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_info( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_infof( context, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_warn( warn, msg ) /*lint -save -e717 */ do { (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_warnf( warn, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_warn( context, warn, msg ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_warnf( context, warn, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_error( err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_errorf( err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_error( context, err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_errorf( context, err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_panic( err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_panicf( err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_panic( context, err, msg ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_context_panicf( context, err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( context ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define log_error_context( context, error_level ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( error_level ); } while(0) /*lint -restore */
#  define log_set_callback( callback ) /*lint -save -e717 */ do { (void)sizeof( callback ); } while(0) /*lint -restore */
#  define log_enable_stdout( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define log_enable_prefix( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define log_set_suppress( context, level ) /*lint -save -e717 */ do { (void)sizeof( context ); (void)sizeof( level ); } while(0) /*lint -restore */
#  define log_suppress( context ) ERRORLEVEL_NONE
#endif
