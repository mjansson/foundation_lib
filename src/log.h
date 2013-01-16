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

#include <platform.h>
#include <types.h>


#if BUILD_ENABLE_DEBUG_LOG

FOUNDATION_API void     debug_logf( const char* message, ... );

#else
#  define               debug_logf( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_LOG

FOUNDATION_API void     info_logf( const char* format, ... );
FOUNDATION_API void     warn_logf( warning_class_t wclass, const char* format, ... );

/*! Log error. Also calls error_report to report the error
    \param level        Error level
    \param err          Error code
    \param format       Log format */
FOUNDATION_API void     error_logf( error_level_t level, error_t err, const char* format, ... );
FOUNDATION_API void     error_log_context( error_level_t error_level );
FOUNDATION_API void     log_stdout( bool enable );

#else
#  define               info_logf( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               warn_logf( wclass, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               error_logf( level, err, msg, ... ) /*lint -save -e717 */ do { error_report( level, err ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define               error_log_context( error_level ) /*lint -save -e717 */ do { (void)sizeof( error_level ); } while(0) /*lint -restore */
#  define               log_stdout( enable ) /*lint -save -e717 */ do { (void)sizeof( enable ); } while(0) /*lint -restore */
#endif

#if BUILD_ENABLE_DEBUG_LOG || BUILD_DEBUG || ( BUILD_RELEASE && !BUILD_DEPLOY && BUILD_ENABLE_RELEASE_ASSERT )

FOUNDATION_API bool     debug_message_box( const char* title, const char* message, bool cancel_button );

#else
#  define               debug_message_box( title, msg, button ) /*lint -save -e717 */ do { (void)sizeof( title ); (void)sizeof( msg ); (void)sizeof( button ); } while(0) /*lint -restore */
#endif
