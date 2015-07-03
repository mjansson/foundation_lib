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
\brief Extended assert macros
\details Extended assert macros and callback handling. Assert enabling/disabling is controlled by
build settings in build.h, where disabling asserts will replace the code with a null or static
statement (depending on the type of assert). */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/crash.h>
#include <foundation/string.h>

/*! \brief Get the currently set global assert handler
Get the currently set global assert handler
\return Assert handler */
FOUNDATION_API assert_handler_fn
assert_handler(void);

/*! \brief Set global assert handlder
Set the current global assert handler. This will be called for each triggered assert and thus
needs to be thread safe. Set to null to disable assert callbacks and use the default behaviour.
\param new_handler New global assert handler
\see assert_report */
FOUNDATION_API void
assert_set_handler(assert_handler_fn new_handler);

/*! \brief Report assert
\details Report assert to the global assert handler. If the global assert handler is set
to null use the default behaviour, which is to print the assert report to the log (error level)
and popup a system message box for UI applications.
\param context Log context
\param condition Assert condition expression
\param cond_length Length of condition expression in bytes
\param file Name of source file triggering assert
\param file_length Length of file name in bytes
\param line Line number triggering assert
\param msg Assert information message
\param msg_length Length of information message in bytes
\return 1 if assert was not handled and execution should break,
        0 if assert handled and execution can continue */
FOUNDATION_API int
assert_report(hash_t context, const char* condition,
              size_t cond_length, const char* file, size_t file_length, unsigned int line,
              const char* msg, size_t msg_length);

/*! \brief Report assert
Report assert with a formatted message. Will call assert_report with the final formatted message.
\see assert_report
\param context Log context
\param condition Assert condition expression
\param cond_length Length of condition expression in bytes
\param file Name of source file triggering assert
\param file_length Length of file name in bytes
\param line Line number triggering assert
\param format Assert information message format specifier
\param format_length Length of information message format specifier in bytes
\return 1 if assert was not handled and execution should break,
        0 if assert handled and execution can continue */
FOUNDATION_API int
assert_report_formatted(hash_t context, const char* condition,
                        size_t cond_length, const char* file, size_t file_length, unsigned int line,
                        const char* format, size_t format_length, ...)
                        FOUNDATION_ATTRIBUTE4(format, printf, 7, 9);

#if BUILD_ENABLE_ASSERT

/*! \brief Base assert with message
Assert the given condition. If assert fails, call the assert report callback with the given
message and optionally (depending on callback) cause a breakpoint.
\param cond Assert condition
\param msg Assert message */
#define FOUNDATION_ASSERT(cond) do { \
  if ((!(cond)) && assert_report(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, \
                                 __LINE__, 0, 0)) \
    crash_debug_break(); \
  } while(0)

/*! \brief Base assert with pre-formatted message
Assert the given condition. If assert fails, call the assert report callback with the given
pre-formatted message and optionally (depending on callback) cause a breakpoint.
\param cond Assert condition
\param msg Assert message */
#define FOUNDATION_ASSERT_MSG(cond, msg) do { \
  if ((!(cond)) && assert_report(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, \
                                 __LINE__, (msg), string_length((msg)))) \
    crash_debug_break(); \
  } while(0)

/*! \brief Base assert with formatted message
Assert the given condition. If assert fails, call the assert report callback with the given
formatted message and optionally (depending on callback) cause a breakpoint.
\param cond Assert condition
\param msg Assert message format specifier */
#define FOUNDATION_ASSERT_MSGFORMAT(cond, msg, ...) do { \
  if ((!(cond)) && assert_report_formatted(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, \
                                           __LINE__, (msg), string_length((msg)), __VA_ARGS__)) \
    crash_debug_break(); \
  } while(0)

/*! \brief Statically failed assert
Statically fail assert, call the assert report callback with the given message and optionally
(depending on callback) cause a breakpoint.
\param msg Assert message */
#define FOUNDATION_ASSERT_FAIL(msg) do { \
  if (assert_report(0ULL, 0, 0, __FILE__, sizeof(__FILE__) - 1, __LINE__, (msg), string_length((msg)))) \
    crash_debug_break(); \
  } while(0)

/*!\brief Statically failed assert with log fallback
Statically fail assert, call the assert report callback with the given message and optionally
(depending on callback) cause a breakpoint. If asserts are not enabled in build config the
message will be logged as an error in the given log context.
\param context Log context
\param msg Assert and log message */
#define FOUNDATION_ASSERT_FAIL_LOG(context, msg) do { \
  if (assert_report(context, 0, 0, __FILE__, sizeof(__FILE__) - 1, __LINE__, (msg), string_length((msg)))) \
    crash_debug_break(); \
  } while(0)

/*! \brief Statically failed assert with formatted message
Statically fail assert, call the assert report callback with the given formatted message and
optionally (depending on callback) cause a breakpoint.
\param msg Assert message format specifier */
#define FOUNDATION_ASSERT_FAILFORMAT(msg, ...) do { \
  if (assert_report_formatted(0ULL, 0, 0, __FILE__, sizeof(__FILE__) - 1, __LINE__, \
                              (msg), string_length((msg)), __VA_ARGS__)) \
    crash_debug_break(); \
  } while(0)

/*! \brief Statically failed assert with log fallback
Statically fail assert, call the assert report callback with the given formatted message and
optionally (depending on callback) cause a breakpoint. If asserts are not enabled in build
config the formatted message will be logged as an error in the given log context.
\param context Log context
\param msg Assert and log message format specifier */
#define FOUNDATION_ASSERT_FAILFORMAT_LOG(context, msg, ...) do { \
  if (assert_report_formatted(context, 0, 0, __FILE__, sizeof(__FILE__) - 1, __LINE__, \
                              (msg), string_length((msg)), __VA_ARGS__)) \
    crash_debug_break(); \
  } while(0)

/*! \brief Assert memory alignment
Assert that the given memory address has the given alignment
\param addr Memory address
\param alignment Alignment requirement */
#define FOUNDATION_ASSERT_ALIGNMENT(addr, alignment) do { \
  FOUNDATION_ASSERT_MSG(((uintptr_t)(addr) % (uintptr_t)(alignment)) == 0, "Mis-aligned memory"); \
  } while(0)

/*! \def FOUNDATION_ASSERT_PLATFORM_ALIGNMENT
\brief Assert memory alignment
Assert that the given memory address has the given alignment if the platform
requires specific alignments for some data types. Evaluates to a no-op if the
platform does not require specific alignments.
\param addr Memory address
\param alignment Alignment requirement */
#if FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64
#define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT(addr, alignment) do { \
  FOUNDATION_ASSERT_ALIGNMENT(addr, alignment); \
  } while(0)
#else
#define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT(addr, alignment) do { \
  (void)sizeof(addr); \
  (void)sizeof(alignment); \
  } while(0)
#endif

/*! \brief Assert condition with evaluated boolean value
Assert the given condition. If assert fails, call the assert report callback without
a message and optionally (depending on callback) cause a breakpoint. The value of the
validate expression will be true if condition is true and false if the condition fails,
even if asserts are disabled in the build config. This allows constructs such as
<code>if( !FOUNDATION_VALIDATE( condition ) ) return;</code> to remain safe and properly
evaluated even when asserts are statically disabled.
\param cond Assert condition */
#define FOUNDATION_VALIDATE(cond) ( \
  (!(cond)) ? \
    (assert_report(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, __LINE__, 0, 0) ? \
      (crash_debug_break(), false) : \
      false) : \
    true)

/*! \brief Assert condition with message and evaluated boolean value
Assert the given condition. If assert fails, call the assert report callback with the
given message and optionally (depending on callback) cause a breakpoint. The value of
the validate expression will be true if condition is true and false if the condition
fails, even if asserts are disabled in the build config. This allows constructs such as
<code>if( !FOUNDATION_VALIDATE_MSG( condition, "message" ) ) return;</code> to
remain safe and properly evaluated even when asserts are statically disabled.
\param cond Assert condition
\param msg Assert message */
#define FOUNDATION_VALIDATE_MSG(cond, msg) ( \
  (!(cond)) ? \
    (assert_report(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, __LINE__, \
                   (msg), string_length((msg))) ? \
      (crash_debug_break(), false) : \
      false) : \
    true)

/*! \brief Assert condition with formatted message and evaluated boolean value
Assert the given condition. If assert fails, call the assert report callback with the
given formatted message and optionally (depending on callback) cause a breakpoint. The
value of the validate expression will be true if condition is true and false if the
condition fails, even if asserts are disabled in the build config. This allows constructs
such as <code>if( !FOUNDATION_VALIDATE_MSGFORMAT( condition, "%s", message ) ) return;</code>
to remain safe and properly evaluated even when asserts are statically disabled.
\param cond Assert condition
\param msg Assert message format specifier */
#define FOUNDATION_VALIDATE_MSGFORMAT(cond, msg, ...) ( \
  (!(cond)) ? \
    (assert_report_formatted(0ULL, #cond, sizeof(#cond) - 1, __FILE__, sizeof(__FILE__) - 1, __LINE__, \
                             (msg), string_length((msg)), __VA_ARGS__) ? \
      (crash_debug_break(), false) : \
      false) : \
    true)

#else

#  define FOUNDATION_ASSERT( cond ) do { (void)sizeof( cond ); } while(0)
#  define FOUNDATION_ASSERT_MSG( cond, msg ) do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_MSGFORMAT( cond, msg, ... ) do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAIL( msg ) do { (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAIL_LOG( context, msg ) do { log_errorf( context, ERROR_ASSERT, STRING_CONST( "%s" ), msg ); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT( msg, ... ) do { (void)sizeof( msg ); } while(0)
#  define FOUNDATION_ASSERT_FAILFORMAT_LOG( context, msg, ... ) do { log_errorf( context, ERROR_ASSERT, msg, string_length( msg ), __VA_ARGS__ ); } while(0)
#  define FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ) do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0)
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0)
#  define FOUNDATION_VALIDATE( cond ) (cond)
#  define FOUNDATION_VALIDATE_MSG( cond, msg ) ( ( !(cond) ) ? ( (void)sizeof( msg ), false ) : true )
#  define FOUNDATION_VALIDATE_MSGFORMAT( cond, msg, ... ) ( ( !(cond) ) ? ( (void)sizeof( msg ), false ) : true )

#endif

/*! \def FOUNDATION_STATIC_ASSERT
\brief Static assert
Static assert evaluated at compile time
\param cond Assert condition
\param msg Assert "message" identifier which should be unquoted on the form
<code>some_kind_of_message</code> */
#if FOUNDATION_COMPILER_CLANG || (FOUNDATION_COMPILER_GCC && (FOUNDATIN_GCC_VERSION >= 40600))
#  define FOUNDATION_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#elif FOUNDATION_COMPILER_MSVC && ( _MSC_VER > 1600 )
#  define FOUNDATION_STATIC_ASSERT(cond, msg) static_assert(cond, msg)
#elif defined( __COUNTER__ )
#  define FOUNDATION_STATIC_ASSERT(cond, msg) /*lint -e{506, 751, 778} */ \
  typedef char FOUNDATION_PREPROCESSOR_JOIN(_static_assert_number_, __COUNTER__)[(cond)? 1 : -1]
#else
#  define FOUNDATION_STATIC_ASSERT(cond, msg) /*lint -e{506, 751, 778} */ \
  typedef char FOUNDATION_PREPROCESSOR_JOIN(_static_assert_at_line_, __LINE__)[(cond) ? 1 : -1]
#endif

