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

FOUNDATION_API void     log_debugf( const char* message, ... );

#else
#  define               log_debugf( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_LOG

FOUNDATION_API void     log_infof( const char* format, ... );
FOUNDATION_API void     log_warnf( warning_class_t wclass, const char* format, ... );

/*! Log error. Also calls error_report to report the error
    \param level        Error level
    \param err          Error code
    \param format       Log format */
FOUNDATION_API void     log_errorf( error_level_t level, error_t err, const char* format, ... );
FOUNDATION_API void     log_error_context( error_level_t error_level );

FOUNDATION_API void     log_stdout( bool enable );

FOUNDATION_API void     log_set_callback( log_callback_fn callback );

FOUNDATION_API void     log_enable_prefix( bool enable );

FOUNDATION_API void     log_suppress( error_level_t level );

#else
#  define               log_infof( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_warnf( wclass, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_errorf( level, err, msg, ... ) /*lint -save -e717 */ do { error_report( level, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               log_error_context( error_level ) /*lint -save -e717 */ do { (void)sizeof( error_level ); } while(0) /*lint -restore */
#  define               log_stdout( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define               log_set_callback( callback ) /*lint -save -e717 */ do { (void)sizeof( callback ); } while(0) /*lint -restore */
#  define               log_enable_prefix( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#  define               log_suppress( level ) /*lint -save -e717 */ do { (void)sizeof( level ); } while(0) /*lint -restore */
#endif

