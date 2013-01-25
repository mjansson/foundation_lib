/* test.h  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation.h>


#if defined( TEST_COMPILE ) && TEST_COMPILE
#  ifdef __cplusplus
#  define TEST_EXTERN extern "C"
#  else
#  define TEST_EXTERN extern
#  endif
#  define TEST_API
#else
#  ifdef __cplusplus
#  define TEST_EXTERN extern "C"
#  define TEST_API extern "C"
#  else
#  define TEST_EXTERN extern
#  define TEST_API extern
#  endif
#endif


typedef int (* test_fn)( void );

TEST_API void test_add_test( test_fn test, const char* group, const char* name );

#define _MAKE_TEST_FN( test_group, test_name ) FOUNDATION_PREPROCESSOR_JOIN( FOUNDATION_PREPROCESSOR_JOIN( test_group, test_name ), _fn )

#define DECLARE_TEST( test_group, test_name ) int _MAKE_TEST_FN( test_group, test_name )( void )
#define ADD_TEST( test_group, test_name ) test_add_test( _MAKE_TEST_FN( test_group, test_name ), FOUNDATION_PREPROCESSOR_TOSTRING( test_group ), FOUNDATION_PREPROCESSOR_TOSTRING( test_name ) )

#define EXPECT_EQ( var, expect ) do { if( (var) != (expect) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s != %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return -1; } } while(0)

