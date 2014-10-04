/* assert.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file assert.h
    Extended assert macros and callback handling. Assert enabling/disabling is controlled by build settings
    in build.h, where disabling asserts will replace the code with a null or static statement (depending on
    the type of assert). */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/crash.h>


/*! Get the currently set global assert handler
    \return                         Assert handler */
FOUNDATION_API assert_handler_fn    assert_handler( void );

/*! Set the current global assert handler. This will be called for each
    triggered assert and thus needs to be thread safe. Set to null to
    disable assert callbacks and use the default behaviour.
    \param new_handler              New global assert handler
	\see assert_handler_fn
	\see assert_report */
FOUNDATION_API void                 assert_set_handler( assert_handler_fn new_handler );

/*! Report assert to the global assert handler. If the global assert handler is set
    to null use the default behaviour, which is to print the assert report
    to the log (error level) and popup a system message box for UI applications.
    \param context                  Log context
    \param condition                Assert condition expression
    \param file                     Source file triggering assert
    \param line                     Line number triggering assert
    \param msg                      Assert information message
    \return                         1 if assert was not handled and execution should break, 0 if assert handled and execution can continue */
FOUNDATION_API int                  assert_report( uint64_t context, const char* condition, const char* file, int line, const char* msg );

/*! Report assert with a formatted message.
    \see assert_report
	\param context                  Log context
	\param condition                Assert condition expression
	\param file                     Source file triggering assert
	\param line                     Line number triggering assert
	\param msg                      Assert information message format specifier
    \return                         1 if assert was not handled and execution should break, 0 if assert handled and execution can continue */
FOUNDATION_API int                  assert_report_formatted( uint64_t context, const char* condition, const char* file, int line, const char* msg, ... );

/*! \def FOUNDATION_ASSERT
    Assert the given condition. If assert fails, call the assert report callback without a message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT) */

/*! \def FOUNDATION_ASSERT_MSG
    Assert the given condition. If assert fails, call the assert report callback with the given message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT) */

/*! \def FOUNDATION_ASSERT_MSGFORMAT
    Assert the given condition. If assert fails, call the assert report callback with the given formatted message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT) */

/*! \def FOUNDATION_ASSERT_FAIL
    Statically fail assert, call the assert report callback with the given message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT) */

/*! \def FOUNDATION_ASSERT_FAIL_LOG
    Statically fail assert, call the assert report callback with the given message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT). If asserts are not enabled in build config the message will be logged as an error in the given log context */

/*! \def FOUNDATION_ASSERT_FAILFORMAT
    Statically fail assert, call the assert report callback with the given formatted message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT) */

/*! \def FOUNDATION_ASSERT_FAILFORMAT_LOG
    Statically fail assert, call the assert report callback with the given formatted message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT). If asserts are not enabled in build config the formatted message will be logged as an error in the given log context */

/*! \def FOUNDATION_ASSERT_ALIGNMENT
    Assert that the given memory address has the given alignment */

/*! \def FOUNDATION_ASSERT_PLATFORM_ALIGNMENT
    Assert that the given memory address has the given alignment if the platform requires specific alignments for some data types */

/*! \def FOUNDATION_VALIDATE
    Assert the given condition. If assert fails, call the assert report callback without a message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT). The value of the validate expression will be true if condition is true and false if the condition fails, even if asserts are disabled in the build config. This allows constructs such as if( !FOUNDATION_VALIDATE( condition ) ) return; to remain safe and properly evaluated even when asserts are statically disabled. */

/*! \def FOUNDATION_VALIDATE_MSG
    Assert the given condition. If assert fails, call the assert report callback with the given message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT). The value of the validate expression will be true if condition is true and false if the condition fails, even if asserts are disabled in the build config. This allows constructs such as if( !FOUNDATION_VALIDATE_MSG( condition, "message" ) ) return; to remain safe and properly evaluated even when asserts are statically disabled. */

/*! \def FOUNDATION_VALIDATE_MSGFORMAT
    Assert the given condition. If assert fails, call the assert report callback with the given formatted message and optionally (depending on callback) cause a breakpoint (FOUNDATION_BREAKPOINT). The value of the validate expression will be true if condition is true and false if the condition fails, even if asserts are disabled in the build config. This allows constructs such as if( !FOUNDATION_VALIDATE_MSGFORMAT( condition, "%s", message ) ) return; to remain safe and properly evaluated even when asserts are statically disabled. */

#if BUILD_ENABLE_ASSERT

#  define FOUNDATION_ASSERT( cond ) do { if( ( !(cond) ) && assert_report( 0ULL, #cond, __FILE__, __LINE__, 0 ) ) crash_debug_break(); } while(0)
#  define FOUNDATION_ASSERT_MSG( cond, msg ) do { if( ( !(cond) ) && assert_report_formatted( 0ULL, #cond, __FILE__, __LINE__, (msg) ) ) crash_debug_break(); } while(0)
#  define FOUNDATION_ASSERT_MSGFORMAT( cond, msg, ... ) do { if( ( !(cond) ) && assert_report_formatted( 0ULL, #cond, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) crash_debug_break(); } while(0)
#  define FOUNDATION_ASSERT_FAIL( msg ) do { if( assert_report( 0ULL, 0, __FILE__, __LINE__, (msg) ) ) crash_debug_break(); } while(0)
#  define FOUNDATION_ASSERT_FAIL_LOG( context, msg ) do { if( assert_report( context, 0, __FILE__, __LINE__, (msg) ) ) crash_debug_break(); (void)sizeof( context ); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT( msg, ... ) do { if( assert_report_formatted( 0ULL, 0, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) crash_debug_break(); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT_LOG( context, msg, ... ) do { if( assert_report_formatted( context, 0, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) crash_debug_break(); (void)sizeof( context ); } while(0)

#  define FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ) do { FOUNDATION_ASSERT_MSG( ( (uintptr_t)(addr) % (uintptr_t)(alignment) ) == 0, "Mis-aligned memory" ); } while(0)
#  if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) do { FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ); } while(0)
#else
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0)
#endif

#  define FOUNDATION_VALIDATE( cond ) ( ( !(cond) ) ? ( assert_report( 0ULL, #cond, __FILE__, __LINE__, 0 ) ? ( crash_debug_break(), false ) : false ) : true )
#  define FOUNDATION_VALIDATE_MSG( cond, msg ) ( ( !(cond) ) ? ( assert_report( 0ULL, #cond, __FILE__, __LINE__, (msg) ) ? ( crash_debug_break(), false ) : false ) : true )
#  define FOUNDATION_VALIDATE_MSGFORMAT( cond, msg, ... ) ( ( !(cond) ) ? ( assert_report_formatted( 0ULL, #cond, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ? ( crash_debug_break(), false ) : false ) : true )

#else

#  define FOUNDATION_ASSERT( cond ) do { (void)sizeof( cond ); } while(0)
#  define FOUNDATION_ASSERT_MSG( cond, msg ) do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_MSGFORMAT( cond, msg, ... ) do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAIL( msg ) do { (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAIL_LOG( context, msg ) do { log_errorf( context, ERROR_ASSERT, "%s", msg ); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT( msg, ... ) do { (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT_LOG( context, msg, ... ) do { log_errorf( context, ERROR_ASSERT, msg, __VA_ARGS__ ); } while(0)
#  define FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ) do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0)
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0)
#  define FOUNDATION_VALIDATE( cond ) (cond)
#  define FOUNDATION_VALIDATE_MSG( cond, msg ) ( ( !(cond) ) ? ( (void)sizeof( msg ), false ) : true )
#  define FOUNDATION_VALIDATE_MSGFORMAT( cond, msg, ... ) ( ( !(cond) ) ? ( (void)sizeof( msg ), false ) : true )

#endif

#define FOUNDATION_STATIC_ASSERT( cond, msg ) /*lint -e{506, 751, 778} */ typedef char msg[(cond)?1:-1]
