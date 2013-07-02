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


#if BUILD_ENABLE_DEBUG_LOG

/*! Log formatted debug message
    \param format       Log message format */
FOUNDATION_API void     log_debugf( const char* message, ... );

#else
#  define               log_debugf( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_LOG

/*! Log information message
    \param format       Log format */
FOUNDATION_API void     log_infof( const char* format, ... );

/*! Log warning message, including the current error context
    \param warn         Warning code
    \param format       Log format */
FOUNDATION_API void     log_warnf( warning_t warn, const char* format, ... );

/*! Log error message, including the current error context. Also calls error_report to report the error
    \param err          Error code
    \param format       Log format */
FOUNDATION_API void     log_errorf( error_t err, const char* format, ... );

/*! Log panic message, including the current error context. Also calls error_report to report the error
    \param err          Error code
    \param format       Log format */
FOUNDATION_API void     log_panicf( error_t err, const char* format, ... );

/*! Log the current error context. The log output is filtered at the given severity level
    \param              Severity level */
FOUNDATION_API void     log_error_context( error_level_t error_level );

/*! Control log output to stdout
    \param enable       Flag to enable/disable output to stdout */
FOUNDATION_API void     log_stdout( bool enable );

/*! Set log callback
    \param callback     New callback */
FOUNDATION_API void     log_set_callback( log_callback_fn callback );

/*! Control output of prefix information
    \param enable       Flag to enable/disable prefix output */
FOUNDATION_API void     log_enable_prefix( bool enable );

/*! Control log suppression based on severity level. Any messages at the
    given severity level or lower will be filtered and discarded.
    \param level        Severity level to discard */
FOUNDATION_API void     log_suppress( error_level_t level );

/*! Get current log supression level
    \return             Severity level begin discarded */
FOUNDATION_API error_level_t  log_get_suppression( void );

#else
#  define               log_infof( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_warnf( warn, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( warn ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_errorf( err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_ERROR, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_panicf( err, msg, ... ) /*lint -save -e717 */ do { error_report( ERRORLEVEL_PANIC, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_error_context( error_level ) /*lint -save -e717 */ do { (void)sizeof( error_level ); } while(0) /*lint -restore */
#  define               log_stdout( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define               log_set_callback( callback ) /*lint -save -e717 */ do { (void)sizeof( callback ); } while(0) /*lint -restore */
#  define               log_enable_prefix( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define               log_suppress( level ) /*lint -save -e717 */ do { (void)sizeof( level ); } while(0) /*lint -restore */
#  define               log_get_suppression() ERRORLEVEL_NONE
#endif

#define log_debug( msg ) log_debugf( "%s", msg )
#define log_info( msg ) log_infof( "%s", msg )
#define log_warn( msg ) log_warnf( "%s", msg )
#define log_error( msg ) log_errorf( "%s", msg )
#define log_panic( msg ) log_panicf( "%s", msg )
