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
    Extended assert macros */

#include <foundation/platform.h>
#include <foundation/types.h>


FOUNDATION_API assert_handler_fn    assert_handler( void );
FOUNDATION_API void                 assert_set_handler( assert_handler_fn new_handler );

FOUNDATION_API int                  assert_report( const char* condition, const char* file, int line, const char* msg );
FOUNDATION_API int                  assert_report_formatted( const char* condition, const char* file, int line, const char* msg, ... );

#undef FOUNDATION_DUMP
#undef FOUNDATION_BREAKPOINT
#undef FOUNDATION_ASSERT

#if FOUNDATION_PLATFORM_WINDOWS
FOUNDATION_EXTERN void __cdecl __debugbreak(void);
#  define FOUNDATION_DUMP() do { *(volatile int*)3 = 0; } while(0)
#  define FOUNDATION_BREAKPOINT() do { __debugbreak(); } while(0)
#elif FOUNDATION_PLATFORM_MACOSX
FOUNDATION_EXTERN void Debugger(void);
#  define FOUNDATION_DUMP() do { __builtin_trap(); } while(0)
#  define FOUNDATION_BREAKPOINT() do { Debugger(); __builtin_trap(); } while(0)
#elif FOUNDATION_PLATFORM_IOS
#  define FOUNDATION_DUMP() do { __builtin_trap(); } while(0)
#  define FOUNDATION_BREAKPOINT() do { __builtin_trap(); } while(0)
#else
#  define FOUNDATION_DUMP() do { *(volatile int*)0 = 0; } while(0)
#  define FOUNDATION_BREAKPOINT() do { *(volatile int*)0 = 0; } while(0)
#endif
                                                                                
#if BUILD_ENABLE_ASSERT

#  define FOUNDATION_ASSERT( cond ) /*lint -save -e717 */ do { if( ( !(cond) ) && assert_report( #cond, __FILE__, __LINE__, 0 ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_MSG( cond, msg ) /*lint -save -e717 */ do { if( ( !(cond) ) && assert_report_formatted( #cond, __FILE__, __LINE__, msg ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_MSGFORMAT( cond, msg, ... ) /*lint -save -e717 */ do { if( ( !(cond) ) && assert_report_formatted( #cond, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAIL( msg ) /*lint -save -e717 */ do { if( assert_report( 0, __FILE__, __LINE__, (msg) ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAIL_LOG( msg ) /*lint -save -e717 */ do { if( assert_report( 0, __FILE__, __LINE__, (msg) ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAILFORMAT( msg, ... ) /*lint -save -e717 */ do { if( assert_report_formatted( 0, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAILFORMAT_LOG( msg, ... ) /*lint -save -e717 */ do { if( assert_report_formatted( 0, __FILE__, __LINE__, (msg), __VA_ARGS__ ) ) FOUNDATION_BREAKPOINT(); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_RETURN( cond ) /*lint -save -e717 */ do { if( ( !(cond) ) && assert_report( #cond, __FILE__, __LINE__, 0 ) ) FOUNDATION_BREAKPOINT(); return; } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ) /*lint -save -e717 */ do { FOUNDATION_ASSERT_MSG( ( (uintptr_t)(addr) % (uintptr_t)(alignment) ) == 0, "Mis-aligned memory" ); } while(0) /*lint -restore */

#  if FOUNDATION_PLATFORM_ARCH_ARM
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) /*lint -save -e717 */ do { FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ); } while(0) /*lint -restore */
#else
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) /*lint -save -e717 */ do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0) /*lint -restore */
#endif

#else

#  define FOUNDATION_ASSERT( cond ) /*lint -save -e717 */ do { (void)sizeof( cond ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_MSG( cond, msg ) /*lint -save -e717 */ do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_MSGFORMAT( cond, msg, ... ) /*lint -save -e717 */ do { (void)sizeof( cond ); (void)sizeof( msg ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAIL( msg ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAIL_LOG( msg ) /*lint -save -e717 */ do { log_errorf( ERROR_ASSERT, "%s", msg ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAILFORMAT( msg, ... ) /*lint -save -e717 */ do { (void)sizeof( msg ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_FAILFORMAT_LOG( msg, ... ) /*lint -save -e717 */ do { log_errorf( ERROR_ASSERT, msg, __VA_ARGS__ ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_RETURN( cond ) /*lint -save -e717 */ do { if( !(cond) ) return; } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_ALIGNMENT( addr, alignment ) /*lint -save -e717 */ do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0) /*lint -restore */
#  define FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( addr, alignment ) /*lint -save -e717 */ do { (void)sizeof(addr); (void)sizeof( alignment ); } while(0) /*lint -restore */

#endif

#define FOUNDATION_COMPILETIME_ASSERT( cond, msg ) /*lint -save -e506 -e751 */ typedef char msg[(cond)?1:-1] /*lint -restore -e506 -e751 */

